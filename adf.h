// vim: ts=4:sw=4:et:tw=80

/*
  adf.h

  The 'Adf' library header file
*/

/*
  Copyright 2020 Sébastien Millet

  Adf is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  Adf is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this program. If not, see
  <https://www.gnu.org/licenses>.
*/

#ifndef _ADF_H
#define _ADF_H

#include <Arduino.h>

#define RF_TRANSMIT_PIN      PB3
#define RF_DEFAULT_REPEAT      8

class Adf {

    private:

        unsigned long rf_table[4];
        bool has_initialized;

        void rf_init_rf_table();

        void rf_send_code(const uint32_t code);
        void rf_send_signal(const byte val, const byte factor_index);

    public:

        Adf();
        void init();

        void rf_send_instruction(const uint32_t code,
                                 byte repeat = RF_DEFAULT_REPEAT);
        void mydelay_us(unsigned long duration_us);
        void mydelay_ms(unsigned long duration_ms);
};

#endif // _ADF_H

