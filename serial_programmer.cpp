/**
 * 
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

#include "serial_programmer.h"

void serial_programmer(StyraConfig * config_ptr) {
    int byte_count = 0;
    uint8_t byte_buffer = 0;
    SERIAL_UINT16_T int_buffer;
    int_buffer.value = 0;
    uint8_t tmp = 0;


    ButtonMacro macro;
    ConfigInfo info;
    ConfigTable table;
    byte * p;

    /* Connect to EEProm */
    config_ptr->begin();

    Serial.end();
    Serial.begin(9600);
    while (!Serial);
    delay(1000);
    Serial.write(READY);
    
    /* Wait for CONFIG packet */
    while (1) {
      if (Serial.available() && Serial.read() == CONFIG) {
         break;
      }
    }
    Serial.write(READY);

    /* Wait for control instruction */
    while (1) {
      if (Serial.available()) {
        byte_buffer = Serial.read() ;
        if (byte_buffer == GET_CONFIG_TABLE) {

          table = config_ptr->getConfigTableFromEEProm();
          Serial.write((char *) &table,sizeof(table));
                   
        } else if (byte_buffer == GET_CONFIG_INFO) {
          Serial.write(READY);
          /* Wait for page location for config  */
          while (1) {
            if (Serial.available() > 1) {
              int_buffer.bytes.lower = Serial.read();
              int_buffer.bytes.upper = Serial.read();
              break;
            }
          }

          info = config_ptr->getConfigInfoFromEEProm(int_buffer.value);
          Serial.write((char *) &info,sizeof(info));
          
        } else if (byte_buffer == GET_BUTTON_MACRO) {
          Serial.write(READY);
          /* Wait for page location for macro  */
          while (1) {
            if (Serial.available() > 1) {
              int_buffer.bytes.lower = Serial.read();
              int_buffer.bytes.upper = Serial.read();
              break;
            }
          }
          
          macro = config_ptr->getButtonMacroFromEEProm(int_buffer.value);
          Serial.write((char *) &macro,sizeof(macro));
          
        } else if (byte_buffer == STORE_CONFIG_TABLE) {
          Serial.write(READY);
          p = (byte *) &table;
          byte_count = 0;

          while (byte_count < sizeof(ConfigTable)){
            if (Serial.available()) {

              p[byte_count++] = Serial.read();
            }
          }

          config_ptr->storeConfigTable(table);
          Serial.write(READY);
          

        } else if (byte_buffer == STORE_CONFIG_INFO) {
          Serial.write(READY);
          /* Wait for page location for ConfigInfo record  */
        
          while (1) {
            if (Serial.available() > 1) {
              int_buffer.bytes.lower = Serial.read();
              int_buffer.bytes.upper = Serial.read();
              break;
            }
          }
          Serial.write(READY);

          p = (byte *) &info;
          byte_count = 0;
          while (byte_count < sizeof(ConfigInfo)){
            if (Serial.available()) {
              p[byte_count++] = Serial.read();
            }
          }
          Serial.write(READY);
          config_ptr->storeConfigInfo(info,int_buffer.value);
          Serial.write(READY);
        
        } else if (byte_buffer == STORE_BUTTON_MACRO) {
          Serial.write(READY);
          /* Wait for page location for ButtonMacro record  */
          while (1) {
            if (Serial.available() > 1) {
              int_buffer.bytes.lower = Serial.read();
              int_buffer.bytes.upper = Serial.read();
              break;
            }
          }
          Serial.write(READY);

          p = (byte *) &macro;
          byte_count = 0;
          while (byte_count < sizeof(ButtonMacro)){
            if (Serial.available()) {
              p[byte_count++] = Serial.read();
            }
          }
          config_ptr->storeButtonMacro(macro,int_buffer.value);
          Serial.write(READY);
          
        } else if (byte_buffer == DONE) {
          Serial.write(READY);
        }
      }
    }
}




