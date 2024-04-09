/* Copyright 2024  C. Krammel (c.krammel@gmx.de)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#define USE_I2C								//define inter controller communications for split setup using I2C
#define MASTER_LEFT							//define primary Keyboard Half based on USB connection to PC (only left will ever be connected)
#define SPLIT_MAX_CONNECTION_ERRORS 10		//Timeout to try detect secondary halft (right), if not detect assume single half operation
#define SPLIT_CONNECTION_CHECK_TIMEOUT 5000 //check every 5s (5k ms) for reconnection of the Secondary Half (right)

#define LOCKING_SUPPORT_ENABLE				//Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap
#define LOCKING_RESYNC_ENABLE				//Locking modifier resynchronize enfocement

/* mouse function config */
#define MOUSEKEY_INTERVAL       20
#define MOUSEKEY_DELAY          0
#define MOUSEKEY_TIME_TO_MAX    60
#define MOUSEKEY_MAX_SPEED      7
#define MOUSEKEY_WHEEL_DELAY 0