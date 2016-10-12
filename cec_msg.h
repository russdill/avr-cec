/*
 * Copyright (C) 2016 Russ Dill <russ.dill@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */
#ifndef _CEC_MSG_H_
#define _CEC_MSG_H_

/*
 * Used as a response to indicate that the device does not support the
 * requested message type, or that it cannot execute it at the present time.
 */
#define CEC_MSG_FEATURE_ABORT			0x00

enum {
	CEC_MSG_ABORT_REASON_OPCODE,	/* Unrecognized opcode */
	CEC_MSG_ABORT_REASON_MODE,	/* Not in correct mode to respond */
	CEC_MSG_ABORT_REASON_SOURCE,	/* Cannot provide source */
	CEC_MSG_ABORT_REASON_OPERAND,	/* Invalid operand */
	CEC_MSG_ABORT_REASON_REFUSED,	/* Refused */
	CEC_MSG_ABORT_REASON_UNABLE,	/* Unable to determine */
};

/*
 * Sent by a source device to the TV whenever it enters the active state
 * (alternatively it may send <Text View On>).
 */
#define CEC_MSG_IMAGE_VIEW_ON			0x04

/*
 * Used to tune to next highest service in a tuner's service list. Can be used
 * for PIP.
 */
#define CEC_MSG_TUNER_STEP_INCREMENT		0x05

/*
 * Used to tune to next lowest service in a tuner's service list. Can be used
 * for PIP.
 */
#define CEC_MSG_TUNER_STEP_DECREMENT		0x06

/*
 * Use by a tuner device to provide its status to the initiator of the <Give
 * Tuner Device Status> message.
 */
#define CEC_MSG_TUNER_DEVICE_STATUS		0x07

/* Used to request the status of a tuner device. */
#define CEC_MSG_GIVE_TUNER_DEVICE_STATUS	0x08

/* Attempt to record the specified source. */
#define CEC_MSG_RECORD_ON			0x09

/*
 * Used by a Recording Device to inform the initiator of the message <Record
 * On> about its status.
 */
#define CEC_MSG_RECORD_STATUS			0x0a

/* Requests a device to stop a recording. */
#define CEC_MSG_RECORD_OFF			0x0b

/*
 * As <Image View On>, but should also remove any text, menus and PIP windows
 * from the TV's display.
 */
#define CEC_MSG_TEXT_VIEW_ON			0x0d

/* Request by the Recording Device to record the presently displayed source. */
#define CEC_MSG_RECORD_TV_SCREEN		0x0f

/*
 * Used to request the status of a device, regardless of whether or not it is
 * the current active source.
 */
#define CEC_MSG_GIVE_DECK_STATUS		0x1a

enum {
	CEC_MSG_STATUS_REQUEST_ON = 1,
	CEC_MSG_STATUS_REQUEST_OFF,
	CEC_MSG_STATUS_REQUEST_ONCE,
};

/*
 * Used to provide a deck's status to the initiator of the <Give Deck Status>
 * message.
 */
#define CEC_MSG_DECK_STATUS			0x1b

enum {
	CEC_MSG_DECK_INFO_PLAY = 0x11,	/* Play */
	CEC_MSG_DECK_INFO_RECORD,	/* Record */
	CEC_MSG_DECK_INFO_PLAY_REVERSE,	/* Play Reverse */
	CEC_MSG_DECK_INFO_STILL,	/* Still */
	CEC_MSG_DECK_INFO_SLOW,		/* Slow */
	CEC_MSG_DECK_INFO_SLOW_REVERSE,	/* Slow Reverse */
	CEC_MSG_DECK_INFO_FAST_FOWARD,	/* Fast Forward */
	CEC_MSG_DECK_INFO_FAST_REVERSE,	/* Fast Reverse */
	CEC_MSG_DECK_INFO_NO_MEDIA,	/* No Media */
	CEC_MSG_DECK_INFO_STOP,		/* Stop */
	CEC_MSG_DECK_INFO_WIND,		/* Skip Forward / Wind */
	CEC_MSG_DECK_INFO_REWIND,	/* Skip Reverse / Rewind */
	CEC_MSG_DECK_INFO_IDX_FORWARD,	/* Index Search Forward */
	CEC_MSG_DECK_INFO_IDX_REVERSE,	/* Index Search Reverse */
	CEC_MSG_DECK_INFO_OTHER,	/* Other Status */
};

/* Used by a TV or another device to indicate the menu language. */
#define CEC_MSG_SET_MENU_LANGUAGE		0x32

/* Used to clear an Analogue timer block of a device. */
#define CEC_MSG_CLEAR_ANALOGUE_TIMER		0x33

/* Used to set a single timer block on an Analogue Recording Device. */
#define CEC_MSG_SET_ANALOGUE_TIMER		0x34

/* Used to send timer status to the initiator of a <Set Timer> message. */
#define CEC_MSG_TIMER_STATUS			0x35

/*
 * Switches one or all devices into standby mode. Can be used as a broadcast
 * message or be addressed to a specific device. See section CEC 13.3 for
 * important notes on the use of this message
 */
#define CEC_MSG_STANDBY				0x36

/* Used to control the playback behaviour of a source device. */
#define CEC_MSG_PLAY				0x41

#define CEC_MSG_PLAY_MODE_MIN_SPEED	0x01
#define CEC_MSG_PLAY_MODE_MEDIUM_SPEED	0x02
#define CEC_MSG_PLAY_MODE_MAX_SPEED	0x03

#define CEC_MSG_PLAY_MODE_FORWARD	0x04
#define CEC_MSG_PLAY_MODE_REVERSE	0x08

#define CEC_MSG_PLAY_MODE_FAST		0x00
#define CEC_MSG_PLAY_MODE_SLOW		0x10
#define CEC_MSG_PLAY_MODE_PLAY		0x20

enum {
	CEC_MSG_PLAY_MODE_FAST_FORWARD_MIN_SPEED =	0x05,
	CEC_MSG_PLAY_MODE_FAST_FORWARD_MEDIUM_SPEED =	0x06,
	CEC_MSG_PLAY_MODE_FAST_FORWARD_MAX_SPEED =	0x07,

	CEC_MSG_PLAY_MODE_SLOW_FORWARD_MIN_SPEED =	0x15,
	CEC_MSG_PLAY_MODE_SLOW_FORWARD_MEDIUM_SPEED =	0x16,
	CEC_MSG_PLAY_MODE_SLOW_FORWARD_MAX_SPEED =	0x17,

	CEC_MSG_PLAY_MODE_FAST_REVERSE_MIN_SPEED =	0x09,
	CEC_MSG_PLAY_MODE_FAST_REVERSE_MEDIUM_SPEED =	0x0a,
	CEC_MSG_PLAY_MODE_FAST_REVERSE_MAX_SPEED =	0x0b,

	CEC_MSG_PLAY_MODE_SLOW_REVERSE_MIN_SPEED =	0x19,
	CEC_MSG_PLAY_MODE_SLOW_REVERSE_MEDIUM_SPEED =	0x1a,
	CEC_MSG_PLAY_MODE_SLOW_REVERSE_MAX_SPEED =	0x1b,

	CEC_MSG_PLAY_MODE_PLAY_REVERSE =		0x20,
	CEC_MSG_PLAY_MODE_PLAY_FORWARD =		0x24,
	CEC_MSG_PLAY_MODE_PLAY_STILL =			0x25,
};

/* Used to control a device's media functions. */
#define CEC_MSG_DECK_CONTROL			0x42

enum {
	CEC_MSG_DECK_CONTROL_MODE_SKIP_FORWARD = 1,
	CEC_MSG_DECK_CONTROL_MODE_SKIP_REVERSE,
	CEC_MSG_DECK_CONTROL_MODE_STOP,
	CEC_MSG_DECK_CONTROL_MODE_EJECT,
};

/*
 * Used to give the status of a <Clear Analogue Timer>, <Clear Digital Timer>
 * or <Clear External Timer> message.
 */
#define CEC_MSG_TIMER_CLEARED_STATUS		0x43

/*
 * Used to indicate that the user pressed a remote control button or switched
 * from one remote control button to another.
 */
#define CEC_MSG_USER_CONTROL_PRESSED		0x44

/*
 * Indicates that user released a remote control button (the last one indicated
 * by the <User Control Pressed> message)
 */
#define CEC_MSG_USER_CONTROL_RELEASED		0x45

/*
 * Used to request the preferred OSD name of a device for use in menus
 * associated with that device.
 */
#define CEC_MSG_GIVE_OSD_NAME			0x46

/*
 * Used to set the preferred OSD name of a device for use in menus associated
 * withthat device.
 */
#define CEC_MSG_SET_OSD_NAME			0x47

/* Used to send a text message to output on a TV. */
#define CEC_MSG_SET_OSD_STRING			0x64

enum {
	CEC_MSG_DISP_CONTROL_DEFAULT = 0,	/* Display for default time */
	CEC_MSG_DISP_CONTROL_CLEARED = 0x40,	/* Display until cleared */
	CEC_MSG_DISP_CONTROL_CLEAR_PREV = 0x80,	/* Clear previous message */
};

/*
 * Used to set the name of a program associated with a timer block. Sent
 * directly after sending a <Set Analogue Timer> or <Set Digital Timer>
 * message. The name is then associated with that timer block.
 */
#define CEC_MSG_SET_TIMER_PROGRAM_TITLE		0x67

/*
 * A device implementing System Audio Control and which has volume control RC
 * buttons (eg TV or STB) requests to use System Audio Mode to the amplifier
 */
#define CEC_MSG_SYSTEM_AUDIO_MODE_REQUEST	0x70

/* Requests an amplifier to send its volume and mute status */
#define CEC_MSG_GIVE_AUDIO			0x71

/* Turns the System Audio Mode On or Off. */
#define CEC_MSG_SET_SYSTEM_AUDIO_MODE		0x72

/* Reports an amplifier's volume and mute status */
#define CEC_MSG_REPORT_AUDIO_STATUS		0x7a

/* Requests the status of the System Audio Mode */
#define CEC_MSG_GIVE_SYSTEM_AUDIO_MODE_STATUS	0x7d

/* Reports the current status of the System Audio Mode */
#define CEC_MSG_SYSTEM_AUDIO_MODE_STATUS	0x7e

/*
 * Sent by a CEC Switch when it is manually switched to inform all other
 * devices on the network that the active route below the switch has changed.
 */
#define CEC_MSG_ROUTING_CHANGE			0x80

/* Sent by a CEC Switch to indicate the active route below the switch. */
#define CEC_MSG_ROUTING_INFORMATION		0x81

/*
 * Used by a new source to indicate that it has started to transmit a stream OR
 * used in response to a <Request Active Source>
 */
#define CEC_MSG_ACTIVE_SOURCE			0x82

/* A request to a device to return its physical address. */
#define CEC_MSG_GIVE_PHYSICAL_ADDRESS		0x83

/*
 * Used to inform all other devices of the mapping between physical and logical
 * address of the initiator.
 */
#define CEC_MSG_REPORT_PHYSICAL_ADDRESS		0x84

enum {
	CEC_MSG_DEVICE_TYPE_TV,		/* TV */
	CEC_MSG_DEVICE_TYPE_REC,	/* Recording Device */
	CEC_MSG_DEVICE_TYPE_RESERVED,	/* Reserved */
	CEC_MSG_DEVICE_TYPE_TUNER,	/* Tuner */
	CEC_MSG_DEVICE_TYPE_PLAY,	/* Playback Device */
	CEC_MSG_DEVICE_TYPE_AUDIO,	/* Audio System */
	CEC_MSG_DEVICE_TYPE_SWITCH,	/* Pure CEC Switch */
	CEC_MSG_DEVICE_TYPE_VIDEO,	/* Video Processor */
};

/* Used by a new device to discover the status of the system. */
#define CEC_MSG_REQUEST_ACTIVE_SOURCE		0x85

/*
 * Used by the TV to request a streaming path from the specified physical
 * address.
 */
#define CEC_MSG_SET_STREAM_PATH			0x86

/* Reports the vendor ID of this device. */
#define CEC_MSG_DEVICE_VENDOR_ID		0x87

/* Allows vendor specific commands to be sent between two devices. */
#define CEC_MSG_VENDOR_COMMAND			0x89

/* Indicates that a remote control button has been depressed. */
#define CEC_MSG_VENDOR_REMOTE_BUTTON_DOWN	0x8a

/*
 * Indicates that a remote control button (the last button pressed indicated by
 * the Vendor Remote Button Down message) has been released.
 */
#define CEC_MSG_VENDOR_REMOTE_BUTTON_UP		0x8b

/* Requests the Vendor ID from a device. */
#define CEC_MSG_GIVE_DEVICE_VENDOR_ID		0x8c

/*
 * A request from the TV for a device to show/remove a menu or to query if a
 * device is currently showing a menu.
 */
#define CEC_MSG_MENU_REQUEST			0x8d

enum {
	CEC_MSG_MENU_REQUEST_TYPE_ACTIVATE,
	CEC_MSG_MENU_REQUEST_TYPE_DEACTIVATE,
	CEC_MSG_MENU_REQUEST_TYPE_QUERY,
};

/*
 * Used to indicate to the TV that the device is showing/has removed a menu and
 * requests the remote control keys to be passed though.
 */
#define CEC_MSG_MENU_STATUS			0x8e

enum {
	CEC_MSG_MENU_STATE_ACTIVATED,
	CEC_MSG_MENU_STATE_DEACTIVATED,
};

/* Used to determine the current power status of a target device */
#define CEC_MSG_GIVE_DEVICE_POWER_STATUS	0x8f

/* Used to inform a requesting device of the current power status */
#define CEC_MSG_REPORT_POWER_STATUS		0x90

enum {
	CEC_MSG_POWER_STATUS_ON,	/* On */
	CEC_MSG_POWER_STATUS_STANDBY,	/* Standy */
	CEC_MSG_POWER_STATUS_2ON,	/* In transition Standby to On */
	CEC_MSG_POWER_STATUS_2STANDBY,	/* In transition On to Standby */
};

/*
 * Sent by a device capable of character generation (for OSD and Menus) to a TV
 * in order to discover the currently selected Menu language. Also used by a TV
 * during installation to discover the currently set menu language of other
 * devices.
 */
#define CEC_MSG_GET_MENU_LANGUAGE		0x91

/* Directly selects an Analogue TV service */
#define CEC_MSG_SELECT_ANALOGUE_SERVICE		0x92

/* Directly selects a Digital TV, Radio or Data Broadcast Service */
#define CEC_MSG_SELECT_DIGITAL_SERVICE		0x93

/* Used to set a single timer block on a Digital Recording Device. */
#define CEC_MSG_SET_DIGITAL_TIMER		0x97

/* Used to clear a Digital timer block of a device. */
#define CEC_MSG_CLEAR_DIGITAL_TIMER		0x99

/* Used to control audio rate from Source Device. */
#define CEC_MSG_SET_AUDIO_RATE			0x9a

/*
 * Used by the currently active source to inform the TV that it has no video to
 * be presented to the user, or is going into standby as the result of a local
 * user command on the device.
 */
#define CEC_MSG_INACTIVE_SOURCE			0x9d

/*
 * Used to indicate the supported CEC version, in response to a <Get CEC
 * Version>
 */
#define CEC_MSG_CEC_VERSION			0x9e

enum {
	CEC_MSG_CEC_VERSION_1_1,	/* Version 1.1 */
	CEC_MSG_CEC_VERSION_1_2,	/* Version 1.2 */
	CEC_MSG_CEC_VERSION_1_2A,	/* Version 1.2a */
	CEC_MSG_CEC_VERSION_1_3,	/* Version 1.3 */
	CEC_MSG_CEC_VERSION_1_3A,	/* Version 1.3a */
	CEC_MSG_CEC_VERSION_1_4,	/* Version 1.4 */
};

/* Used by a device to enquire which version of CEC the target supports */
#define CEC_MSG_GET_CEC_VERSION			0x9f

/*
 * Allows vendor specific commands to be sent between two devices or broadcast.
 */
#define CEC_MSG_VENDOR_COMMAND_WITH_ID		0xa0

/* Used to clear an External timer block of a device. */
#define CEC_MSG_CLEAR_EXTERNAL_TIMER		0xa1

/* Used to set a single timer block to record from an external device. */
#define CEC_MSG_SET_EXTERNAL_TIMER		0xa2

/* This message is reserved for testing purposes. */
#define CEC_MSG_ABORT				0xff

/*
 * Used by any device for device discovery - similar to ping in other
 * protocols.
 */
#define CEC_MSG_POLLING_MESSAGE

#endif
