// vim: ts=4:sw=4:et:tw=80

/*
  send.ino

  Uses Adf library to send code to RF433 Mhz transmitter device.

  Schema: data on PIN PB3, that is:
    Arduino: 'data' of RF433 transmitter needs be plugged on PIN 'D3'.
    Attiny *5: 'data' of RF433 transmitter needs be plugged on PIN number 2.
*/

/*
  Copyright 2020 Sébastien Millet

  send.ino is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  send.ino is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses>.
*/

#include "adf.h"

Adf adf;

void setup() {

}

void loop() {
    adf.rf_send_instruction(0xABCD0123);
    adf.mydelay_ms(5000);
}

