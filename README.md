## AVR CEC

This is an Consumer Electronics Control (CEC) implementation for Atmel AVRs.
CEC is a control link feature of HDMI for communication between devices and
a TV. This library currently is concentrated on supporting the ATTiny line
of Atmel AVRs, but should be extendable to others.

The library is broken into three main parts:

* Protocol implementation
* Address assignment
* Hardware drivers

## Protocol implementation

The protocol implementation includes a receive engine and transmit engine.
Both operate simultaneously, with the receive engine cross-checking the
transmit engine to help detect interference on the CEC line. The engines
are responsible for communicating to/from the user app and passing bits to/
receiving bits from the hardware drivers.

### Receive interface

The receive interface consists of a single receive buffer. The buffer
contains a header byte giving state plus up to 16 bytes received via CEC.
The internal state machine fills the buffer and when the reception is
complete it fills in the header byte. When the user app has finished
processing the message, it should clear the header byte. Until the user
app clears the header byte, no additional messages can be received and
are not acknowledged.

The lower 6 bits of the message give the length of the message. Bit 7 is
set if the message was nack'd by ourselves or another entity on the bus.
Bit 6 is set if the buffer was not large enough to hold the message. If
the length of the message was beyond 63 bytes, the stored bit-field storing
the length overflows into the nack and overflow bits, potentially giving
a false nack indication and also giving a shorter than actual length.

The receive engine is responsible for requesting the underlying hardware
driver to ack/nack receive frames. In order to ack unicast frames addressed
to us, we must have one or more logical addresses assigned by the address
assignment module.

### Transmit interface

The transmit interface consists of a transmit buffer, a length indicator,
and a state:

```c
unsigned char transmit_buf[CEC_BUFFER_SIZE];
unsigned char transmit_buf_end;
unsigned char transmit_state;
```

The relevant states are:

```c
#define TRANSMIT_IDLE		0
#define TRANSMIT_FAILED		_BV(0)
#define TRANSMIT_PEND		_BV(1)
```

Any states higher than TRANSMIT_PEND are internal and indicate that the
interface is busy. TRANSMIT_IDLE indicates that the transmit interface
is free and that the last sent message did not fail. TRANSMIT_FAILED
indicates that the transmit interface is free but that the last sent
message failed. TRANSMIT_PEND indicates that the user application has
queued a message and is waiting for the transmit engine to process it.

In order to send a message, the user application should verify that the
current transmit state is either IDLE or FAILED, then load the buffer, then
set transmit_buf_end, then set the transmit state to PEND. Note that
transmit_buf_end contains an index to the last byte in the message. For
instance, for a 1 byte message (header only) transmit_buf_end should be 0.

Additionally, if the compile flag CEC_ERR_STATS is set, the transmit
interface provides additional information regarding the cause of
transmit failures. Rather than a single transmit_state byte, it has a
7 byte transmit_state_buf:

```c
unsigned char transmit_state_buf[7];
#define transmit_state transmit_state_buf[0]
```

The additional bytes contain counts of failure types. Since messages
are retried up to 5 times, some of these fields may be sent even in
the case of a successful transmission:

```c
#define CEC_ERR_ARB_LOST	1
#define CEC_ERR_NACK		2
#define CEC_ERR_NO_EOM		3
#define CEC_ERR_LOW_DRIVE	4
#define CEC_ERR_HALT		5
#define CEC_ERR_HW		6
```

ARB_LOST indicates that another host sent a 0 while we we sending a 1.
NACK indicates that the destination did not ack our message, or in
the case of broadcast messages an entity nack'd. NO_EOM indicates that
the message terminated before an EOM (End-of-Message) signal was
received. LOW_DRIVE indicates that the line was driven low when not
permitted by the protocol. HALT simply indicates that the message was
terminated due to cec_halt() being called by the user application. HW
indicates a failure on the receive hardware's part, for example if the
driver's periodic function is not called often enough.

## Address assignment

CEC devices required a logical address to transmit on the bus. AVR CEC has a
number of modules supporting different methods of address assignment. The
relevant functions for dealing with the address assignment module are:

```c
bool cec_addr_match(unsigned char addr);
bool cec_addr_ready(void);
unsigned char cec_addr_build(unsigned char source, unsigned char target);
```

cec_addr_match returns true if the passed address matches one of our own
assigned logical addresses. It can be used to check if a message is
addressed to us and also to check if a message is from us. This is helpful
since the receive interface passes all received message, including those
not addressed to us and even those sent by us.

cec_addr_ready returns true if the address assignment process has completed.
This usually isn't needed as the address assignment process always empties
the receive buffer and fills the transmit buffer.

cec_addr_build is used to build the header byte of the transmit buffer. The
header byte includes the source address and the target address. For some
address assignment modules, the source address is ignored as it is managed
by the address assignment module. For others it is required.

### cec_addr_dev_type

This is the traditional CEC address assignment method based on device type.
This module is used if the CEC_DEV_TYPE compile time macro is defined. The
module attempts to allocate an address allowed by the assigned CEC_DEV_TYPE.
If it fails, it uses the unassigned address. Calls to cec_build_addr ignore
the source argument.

### cec_addr_bitfield

This address assignment module consists of a bit-field of currently assigned
logical addresses. The module does not check if addresses are already in use
and leaves that responsibility to the user application. Currently assigned
logical addresses can be set by modifying the logical_addresses global
variable. A call to cec_init() clears the currently assigned logical
addresses. Calls to cec_build_addr require a valid source argument.

This module is used if the CEC_LOGICAL_ADDRESS_BITFIELD compile time macro
is defined.

### cec_addr_fixed

This module operates with a fixed logical address defined at compile time.
The source argument to cec_build_addr is ignored. This module is used by
defining the logical address in the compile time macro
CEC_FIXED_LOGICAL_ADDRESS.

### cec_addr_none

This module is used if the device cannot have a logical address, not even
the unregistered address. This module is used for devices that only listen.


## Hardware drivers

The hardware drivers are responsible for sending and receiving individual
CEC frames. Drivers can handle receive only, transmit only, or both.

### cec_usi

This driver uses the Universal Serial Interface (USI) to send and receive
CEC frames. USI lets us send and receive a byte's worth of bits at a time.
We setup the USI hardware so that a single byte represents a single CEC
frame (2.4ms). This means that each bit represents a time period of 300uS.
Sending frames is very straightforward, but receiving frames involves
tracking the number of ticks (300uS units) between each rising and falling
edge.

Acking and nacking provides a bit of a complication as we may not know that
we need to ack until we've processed the EOM frame, which is right before
the ACK frame. In order to handle this situation, we poke our ack bits
directly in the USI shift buffer rather than waiting for them to be loaded
from the buffer register.

The USI driver is extremely tolerant to latency, usually tolerating up to
nearly 4.8ms. Some acks though require a maximum of a few hundred
microseconds. However, if the the ack is missed, the initiator will
retransmit.

The USI driver also provides a cec_usi_frame_hook() callback to provide
the user application with a once every 2.4ms callback. This function is
called from within cec_periodic, so any latency in calling cec_periodic
is passed onto cec_usi_frame_hook.

### cec_receive_raw

This driver processes input CEC frames by reading directly from the IO port.
It requires a time delta to be passed to the periodic function. The latency
for correct operation is around ~200uS, but if the driver detects a missed
window it will stop acking causing the initiator to resend.

### cec_transmit_pwm

This driver sends CEC frames by using the PWM interface. This creates a very
simple latency interface because the difference between a CEC 0 frame and
a CEC 1 frame is just duty cycle. The latency tolerance is up to nearly
4.8ms. This driver currently requires the delta parameter, but could be
modified to count it's own frames instead.

## Additional functions:

cec_init() - Initializes and starts the CEC framework.
cec_halt() - Stops the CEC framework.
cec_periodic(unsigned int delta) - Must be called periodically.
cec_pin_config() - Configures the CEC pins.
cec_pin_unconfig() - Undoes the CEC pin configuration.


## Example application

```c
#include <avr/io.h>

#define CEC_USI
#define TCNT0_ROLLOVER_PERIOD_US	300	/* Required for CEC USI */

#define CEC_DDR         DDRB
#define CEC_PIN         PINB
#define CEC_PORT        PORTB
#define CEC_PBIN        PB0
#define CEC_PBOUT       PB1
#define CEC_DEV_TYPE    CEC_DEV_PLAYBACK_DEVICE

#include "avr-cec/cec.c"

int main(void)
{
	cec_init();

	for (;;) {
		cec_periodic(0);

		if (transmit_state < TRANSMIT_PEND) {
			/* Send out any CEC messages */
			transmit_buf[0] = cec_addr_build(0, CEC_ADDR_TV);
			transmit_buf[1] = CEC_MSG_GET_CEC_VERSION;
			transmit_buf_end = 1;
			transmit_state = TRANSMIT_PEND;
		}

		if (cec_receive_buf[0]) {
			unsigned char len;
			unsigned char target;
			unsigned char source;
			bool nack;
			bool overrun;

			len = cec_receive_buf[0] & 0x3f;
			nack = (cec_receive_buf[0] & CEC_STATUS_NACK) != 0;
			overrun = (cec_receive_buf[0] & CEC_STATUS_OVERRUN) != 0;

			source = cec_receive_buf[1] >> 4;
			target = cec_receive_buf[1] & 0xf;

			if (!nack && !overrun) {
				/* Process message */
			}

			/* Indicate we are done with the buffer */
			cec_receive_buf[0] = 0;
		}
	}

	return 0;
}
```
