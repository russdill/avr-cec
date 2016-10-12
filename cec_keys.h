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
#ifndef _CEC_KEYS_H
#define _CEC_KEYS_H

#define CEC_KEY_SELECT				0x00
#define CEC_KEY_UP				0x01
#define CEC_KEY_DOWN				0x02
#define CEC_KEY_LEFT				0x03
#define CEC_KEY_RIGHT				0x04
#define CEC_KEY_RIGHT_UP			0x05
#define CEC_KEY_RIGHT_DOWN			0x06
#define CEC_KEY_LEFT_UP 			0x07
#define CEC_KEY_LEFT_DOWN			0x08
/*
 * This triggers the initial display that a device shows. It is device-
 * dependent and can be, for example, a contents menu, setup menu, favorite
 * menu or other menu. The actual menu displayed may also depend on the
 * device's current state.
 */
#define CEC_KEY_ROOT_MENU			0x09
#define CEC_KEY_SETUP_MENU			0x0a
#define CEC_KEY_CONTENTS_MENU			0x0b
#define CEC_KEY_FAVORITE_MENU			0x0c
#define CEC_KEY_EXIT				0x0d
/*
 * This triggers the display of the main menu available for the currently
 * playing media, e.g. DVD/BD Top Menu to select Language, Subtitle, Scene,
 * Bonus, Start of Film, etc.
 */
#define CEC_KEY_MEDIA_TOP_MENU			0x10
/*
 * This triggers the display of a context-sensitive media-related menu (e.g.
 * DVD Menu or BD Popup Menu), typically containing functions to adapt the
 * playback of the currently playing content.
 */
#define CEC_KEY_MEDIA_CONTEXT_SENSITIVE_MENU	0x11
/*
 * Selects an available Number Entry Mode that may be implemented on a
 * device, such as: 1-12-key entry mode, 0-9-key entry mode, single or
 * multiple digit entry.
 */
#define CEC_KEY_NUM_ENTRY			0x1d
#define CEC_KEY_11				0x1e
#define CEC_KEY_12				0x1f
#define CEC_KEY_0				0x20
#define CEC_KEY_1				0x21
#define CEC_KEY_2				0x22
#define CEC_KEY_3				0x23
#define CEC_KEY_4				0x24
#define CEC_KEY_5				0x25
#define CEC_KEY_6				0x26
#define CEC_KEY_7				0x27
#define CEC_KEY_8				0x28
#define CEC_KEY_9				0x29
#define CEC_KEY_DOT				0x2a
#define CEC_KEY_ENTER				0x2b
#define CEC_KEY_CLEAR				0x2c
#define CEC_KEY_NEXT_FAVORITE			0x2f
#define CEC_KEY_CHANNEL_UP			0x30
#define CEC_KEY_CHANNEL_DOWN			0x31
#define CEC_KEY_PREVIOUS_CHANNEL		0x32
#define CEC_KEY_SOUND_SELECT			0x33
#define CEC_KEY_INPUT_SELECT			0x34
#define CEC_KEY_DISPLAY_INFORMATION		0x35
#define CEC_KEY_HELP				0x36
#define CEC_KEY_PAGE_UP 			0x37
#define CEC_KEY_PAGE_DOWN			0x38
#define CEC_KEY_POWER				0x40
#define CEC_KEY_VOLUME_UP			0x41
#define CEC_KEY_VOLUME_DOWN			0x42
#define CEC_KEY_MUTE				0x43
#define CEC_KEY_PLAY				0x44
#define CEC_KEY_STOP				0x45
#define CEC_KEY_PAUSE				0x46
#define CEC_KEY_RECORD				0x47
#define CEC_KEY_REWIND				0x48
#define CEC_KEY_FAST_FORWARD			0x49
#define CEC_KEY_EJECT				0x4a
#define CEC_KEY_FORWARD 			0x4b
#define CEC_KEY_BACKWARD			0x4c
#define CEC_KEY_STOP_RECORD			0x4d
#define CEC_KEY_PAUSE_RECORD			0x4e
#define CEC_KEY_ANGLE				0x50
#define CEC_KEY_SUB_PICTURE			0x51
#define CEC_KEY_VIDEO_ON_DEMAND 		0x52
#define CEC_KEY_ELECTRONIC_PROGRAM_GUIDE	0x53
#define CEC_KEY_TIMER_PROGRAMMING		0x54
#define CEC_KEY_INITIAL_CONFIGURATION		0x55
#define CEC_KEY_SELECT_BROADCAST_TYPE		0x56
#define CEC_KEY_SELECT_SOUND_PRESENTATION	0x67
#define CEC_KEY_PLAY_FUNCTION			0x60
#define CEC_KEY_PAUSE_PLAY_FUNCTION		0x61
#define CEC_KEY_RECORD_FUNCTION 		0x62
#define CEC_KEY_PAUSE_RECORD_FUNCTION		0x63
#define CEC_KEY_STOP_FUNCTION			0x64
#define CEC_KEY_MUTE_FUNCTION			0x65
#define CEC_KEY_RESTORE_VOLUME_FUNCTION 	0x66
#define CEC_KEY_TUNE_FUNCTION			0x67
#define CEC_KEY_SELECT_MEDIA_FUNCTION		0x68
#define CEC_KEY_SELECT_AV_INPUT_FUNCTION	0x69
#define CEC_KEY_SELECT_AUDIO_INPUT_FUNCTION	0x6a
#define CEC_KEY_POWER_TOGGLE_FUNCTION		0x6b
#define CEC_KEY_POWER_OFF_FUNCTION		0x6c
#define CEC_KEY_POWER_ON_FUNCTION		0x6d

#define CEC_KEY_F1				0x71
#define CEC_KEY_BLUE				0x71
#define CEC_KEY_F2				0x72
#define CEC_KEY_RED				0x72
#define CEC_KEY_F3				0x73
#define CEC_KEY_GREEN				0x73
#define CEC_KEY_F4				0x74
#define CEC_KEY_YELLOW				0x74
#define CEC_KEY_F5				0x75
/*
 * This is used, for example, to enter or leave a digital TV data broadcast
 * application.
 */
#define CEC_KEY_DATA				0x76

#endif
