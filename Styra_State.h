/**
 * Styra_State implements the core state machine for all Styra prototypes
 * 
 * Copyright (C) 2016  Luke Hindman <luke@platypuscreations.net>
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */ 

#ifndef __STYRA_STATE_H__
#define __STYRA_STATE_H__

#include <Arduino.h>
#include <Styra_Config.h>
#include <Styra_Pointer.h>
#include <Styra_Controller.h>


/* States for the StyraState */
#define READY 1
#define LOOKUP_ACTION_TYPE 2
#define SEND_CLICK 3
#define SEND_DBL_CLICK 4
#define SEND_SEQ 5
#define SEND_LATCH 6
#define SEND_PRESS 7
#define WAIT_FINAL_RELEASE 8
#define WAIT_LATCH_RELEASE 9
#define WAIT_PRESS_RELEASE 10
#define WAIT_LATCH_PRESS 11
#define SEND_LATCH_RELEASE 12
#define SEND_PRESS_RELEASE 13
#define TOGGLE_MOUSE_STATE 14
#define SET_WHEEL_LOCK 15
#define WAIT_WHEEL_LOCK_RELEASE 16
#define SET_WHEEL_LOCK_RELEASE 17

/* Define action type for each button */
#define CLICK 1
#define DBL_CLICK 2
#define SEQUENCE 3
#define LATCH 4
#define PRESS 5
#define TOGGLE_MOUSE 6
#define WHEEL_LOCK 7

/* Define delay values */
#define KEY_SPACE_DELAY 10 /*ms*/
#define KEY_HOLD_DELAY 10 /*ms*/
#define DBL_CLICK_DELAY 100 /*ms*/

/* HID_DEVICE_TYPE */
#define STYRA_MOUSE 1
#define STYRA_KEYBOARD 0



class StyraState
{
public:
    StyraState(StyraConfig * config_ptr, StyraPointer * pointer_ptr, StyraController * controller_ptr);
    void begin();
    void update();


private:
    byte _styra_state;
    StyraMacro _styra_macro;
    uint8_t _current_button;

    StyraConfig * _styra_config;
    StyraPointer * _styra_pointer;
    StyraController * _styra_controller;
};


#endif /* __STYRA_STATE_H__ */
