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

#include "Styra_State.h"
 
//#define SERIAL_DEBUG

/* HACK:  This function is a work around a ChromeOS bug that requires a shift report to be sent
 *  before sending a shifted character.  This has only been tested for the SEQ type, though it could
 *  likely be ported to the CLICK, PRESS, LATCH and DOUBLE-CLICK action types if required.
 */

/* Define shift mask array */
uint8_t shift_mask[16] = {0,0,0,0,0b11111110,0b00001111,0,0b00000100,0b11111111,0b11111111,0b11111111,0b11000111,0b00000001,0,0,0b01101000};

void keyboardWriteCharacter(byte key) {
  uint8_t shift_mask_byte,shift_mask_offset;
  shift_mask_byte = key/8;
  shift_mask_offset = key % 8;
  if ((shift_mask[shift_mask_byte] & 1<<shift_mask_offset) == 0 ){
      Keyboard.write(char(key));
  } else {
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press(char(key));
      delay(KEY_HOLD_DELAY);
      Keyboard.releaseAll();
  }
}


StyraState::StyraState(StyraConfig * config_ptr, StyraPointer * pointer_ptr, StyraController * controller_ptr) {

  _styra_state = READY;

  _styra_controller = controller_ptr;
  _styra_pointer = pointer_ptr;
  _styra_config = config_ptr;
}



void StyraState::begin() {
#ifdef SERIAL_DEBUG
  Serial.println("Initializing Styra Controller Boards");
#endif
  _styra_controller->begin();
#ifdef SERIAL_DEBUG
  Serial.println("Initializing Configuration Manager");
#endif
  _styra_config->begin();
#ifdef SERIAL_DEBUG
  Serial.println("Initializing Keyboard Device");
#endif
  Keyboard.begin();
#ifdef SERIAL_DEBUG
  Serial.println("Initializing Mouse Device");
#endif
  Mouse.begin();
#ifdef SERIAL_DEBUG
  Serial.println("Initializing Styra Pointer Device");
#endif
  _styra_pointer->begin();
#ifdef SERIAL_DEBUG
  Serial.println("Loading Config From EEProm");
#endif
  // ConfigInfo conf_info = _styra_config->getConfigInfo(_styra_config->getActiveConfig() );
  ConfigInfo conf_info = _styra_config->getConfigInfo();
#ifdef SERIAL_DEBUG
  Serial.print("Config Name: ");
  Serial.println(conf_info.name);
  Serial.print("Num Defined Macros: ");
  Serial.println(conf_info.num_macros);
  Serial.println("State: READY");
#endif


}

void StyraState::update() {
  /* TODO:  The device_type needs to be looked up in the config for
   *   each macro, not set here for ALL macros. */
    byte _styra_device_type = STYRA_MOUSE;

    _styra_pointer->update();
    uint8_t shift_mask_byte,shift_mask_offset;
    byte key;
    _styra_controller->update();
    if (_styra_state == READY) {
        _current_button = _styra_controller->getPressedButton();
        if (_current_button != NONE_PRESSED)  {
          _styra_state = LOOKUP_ACTION_TYPE;
#ifdef SERIAL_DEBUG
            Serial.print("Processing Button: ");
            Serial.println(_current_button);
            Serial.println("State: LOOKUP_ACTION_TYPE");
#endif

        }

    }
    if (_styra_state == LOOKUP_ACTION_TYPE) {

        _styra_macro.setMacro(_styra_config->getMacro(_current_button));
        byte action = _styra_macro.getActionType();
        if (action == CLICK) {
            _styra_state = SEND_CLICK;
#ifdef SERIAL_DEBUG
            Serial.println("State: SEND_CLICK");
#endif
        } else if (action == DBL_CLICK) {
            _styra_state = SEND_DBL_CLICK;
#ifdef SERIAL_DEBUG
            Serial.println("State: SEND_DBL_CLICK");
#endif
        } else if (action == SEQUENCE) {
            _styra_state = SEND_SEQ;
#ifdef SERIAL_DEBUG
            Serial.println("State: SEND_SEQ");
#endif
        } else if (action == LATCH) {
            _styra_state = SEND_LATCH;
#ifdef SERIAL_DEBUG
            Serial.println("State: SEND_LATCH");
#endif
        } else if (action == PRESS) {
            _styra_state = SEND_PRESS;
#ifdef SERIAL_DEBUG
            Serial.println("State: SEND_PRESS");
#endif
        } else if (action == TOGGLE_MOUSE) {
            _styra_state = TOGGLE_MOUSE_STATE;
#ifdef SERIAL_DEBUG
            Serial.println("State: TOGGLE_MOUSE_STATE");
#endif
        } else if (action == WHEEL_LOCK) {
            _styra_state = SET_WHEEL_LOCK;
#ifdef SERIAL_DEBUG
            Serial.println("State: SET_WHEEL_LOCK");
#endif
        }

    }

    if (_styra_state == SEND_CLICK) {

        if (_styra_device_type == STYRA_MOUSE) {
          Mouse.click();
        } else {
          while ((key = _styra_macro.getNextKey()) != 0) {
              Keyboard.press(key);
              delay(KEY_SPACE_DELAY);
          }
          delay(KEY_HOLD_DELAY);
          Keyboard.releaseAll();
        }

        _styra_state = WAIT_FINAL_RELEASE;
#ifdef SERIAL_DEBUG
        Serial.println("State: WAIT_FINAL_RELEASE");
#endif
    }

    if (_styra_state == SEND_DBL_CLICK) {

      if (_styra_device_type == STYRA_MOUSE) {
        Mouse.click();
        delay(DBL_CLICK_DELAY);
        Mouse.click();
      } else {
        while ((key = _styra_macro.getNextKey()) != 0) {
            Keyboard.press(key);
            delay(KEY_SPACE_DELAY);
        }
        delay(KEY_HOLD_DELAY);
        Keyboard.releaseAll();
        _styra_macro.resetKeyIndex();
        delay(DBL_CLICK_DELAY);
        while ((key = _styra_macro.getNextKey()) != 0) {
            Keyboard.press(key);
            delay(KEY_SPACE_DELAY);
        }
        delay(KEY_HOLD_DELAY);
        Keyboard.releaseAll();
      }
        _styra_state = WAIT_FINAL_RELEASE;
#ifdef SERIAL_DEBUG
        Serial.println("State: WAIT_FINAL_RELEASE");
#endif
    }

    if (_styra_state == SEND_SEQ) {

        while ((key = _styra_macro.getNextKey()) != 0) {
            keyboardWriteCharacter (key);
            delay(KEY_SPACE_DELAY);
        }
        _styra_state = WAIT_FINAL_RELEASE;
#ifdef SERIAL_DEBUG
        Serial.println("State: WAIT_FINAL_RELEASE");
#endif
    }

    if (_styra_state == SEND_LATCH) {
      if (_styra_device_type == STYRA_MOUSE) {
        Mouse.press();
      } else {
        while ((key = _styra_macro.getNextKey()) != 0) {
            Keyboard.press(key);
            delay(KEY_SPACE_DELAY);
        }
      }

        _styra_state = WAIT_LATCH_RELEASE;
#ifdef SERIAL_DEBUG
        Serial.println("State: WAIT_LATCH_RELEASE");
#endif
    }

    if (_styra_state == SET_WHEEL_LOCK) {
      if (_styra_device_type == STYRA_MOUSE) {
        _styra_pointer->enableWheelLock();
      }
        _styra_state = WAIT_WHEEL_LOCK_RELEASE;
#ifdef SERIAL_DEBUG
        Serial.println("State: WAIT_WHEEL_LOCK_RELEASE");
#endif
    }


    if (_styra_state == SEND_PRESS) {
      if (_styra_device_type == STYRA_MOUSE) {
        Mouse.press();
      } else {
        while ((key = _styra_macro.getNextKey()) != 0) {
            Keyboard.press(key);
            delay(KEY_SPACE_DELAY);
        }
      }
        _styra_state = WAIT_PRESS_RELEASE;
#ifdef SERIAL_DEBUG
        Serial.println("State: WAIT_PRESS_RELEASE");
#endif
    }

    if (_styra_state == TOGGLE_MOUSE_STATE) {
        _styra_pointer->disable(!_styra_pointer->isDisabled());

        _styra_state = WAIT_FINAL_RELEASE;
#ifdef SERIAL_DEBUG
        Serial.println("State: WAIT_FINAL_RELEASE");
#endif
    }

    if (_styra_state == WAIT_FINAL_RELEASE) {

        if (_styra_controller->getButtonState(_current_button) == RELEASED) {
            _styra_state = READY;
#ifdef SERIAL_DEBUG
            Serial.println("State: READY");
#endif
        }
    }

    if (_styra_state == WAIT_LATCH_RELEASE) {

        if (_styra_controller->getButtonState(_current_button) == RELEASED) {
            _styra_state = SEND_LATCH_RELEASE;
#ifdef SERIAL_DEBUG
            Serial.println("State: SEND_LATCH_RELEASE");
#endif
        }
    }

    if (_styra_state == WAIT_WHEEL_LOCK_RELEASE) {

        if (_styra_controller->getButtonState(_current_button) == RELEASED) {
            _styra_state = SET_WHEEL_LOCK_RELEASE;
#ifdef SERIAL_DEBUG
            Serial.println("State: SET_WHEEL_LOCK_RELEASE");
#endif
        }
    }


    if (_styra_state == WAIT_PRESS_RELEASE) {

        if (_styra_controller->getButtonState(_current_button) == RELEASED) {
          if (_styra_device_type == STYRA_MOUSE) {
            Mouse.release();
          } else {
            Keyboard.releaseAll();
          }
            _styra_state = READY;
#ifdef SERIAL_DEBUG
            Serial.println("State: READY");
#endif
        }
    }

    if (_styra_state == SEND_LATCH_RELEASE) {
        if (_styra_controller->getButtonState(_current_button) == PRESSED) {
          if (_styra_device_type == STYRA_MOUSE) {
            Mouse.release();
          } else {
            Keyboard.releaseAll();
          }
            _styra_state = WAIT_FINAL_RELEASE;
#ifdef SERIAL_DEBUG
            Serial.println("State: WAIT_FINAL_RELEASE");
#endif
        }
    }

    if (_styra_state == SET_WHEEL_LOCK_RELEASE) {
        if (_styra_controller->getButtonState(_current_button) == PRESSED) {
          if (_styra_device_type == STYRA_MOUSE) {
            _styra_pointer->disableWheelLock();
          }
            _styra_state = WAIT_FINAL_RELEASE;
#ifdef SERIAL_DEBUG
            Serial.println("State: WAIT_FINAL_RELEASE");
#endif
        }
    }

}
