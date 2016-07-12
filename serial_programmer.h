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

#ifndef __SERIAL_PROGRAMMER_H__
#define __SERIAL_PROGRAMMER_H__

#include <Styra_Config.h>

/* Define programmer messages */
#define READY 200
#define CONFIG 201
#define STORE_BUTTON_MACRO 202
#define STORE_CONFIG_INFO 203
#define STORE_CONFIG_TABLE 204
#define GET_BUTTON_MACRO 205
#define GET_CONFIG_INFO 206
#define GET_CONFIG_TABLE 207
#define DONE 254

 typedef union {
  
  struct {
    byte lower: 8;
    byte upper: 8;
  } bytes;
  uint16_t value;
  
} SERIAL_UINT16_T;

void serial_programmer(StyraConfig * config_ptr);

#endif /*  __SERIAL_PROGRAMMER_H__ */

