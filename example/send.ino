// vim: ts=4:sw=4:et:tw=80

/*
  send.ino

  Sends code to RF433 Mhz transmitter device.
  The code is made of 32-bit code intended for "Automatismes De France" slaters.
  Tested with: Arduino Uno, Arduino Nano, Attiny 45

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

/*
  Note about ATTINY programming

  This code is mainly developped and tested with an Arduino.
  However it can be used on Attiny (tested only with an Attiny 45 though).
  Attiny compatibility has the following consequences on the code:
    1. No division (it takes too many cycles and can heavily distort signal
       timing). Note that it can be daunting to detect/test it: if dividing by
       1024 for example, it takes a few cycles as it executes as a register
       rotation (C' >> operator). But if dividing with a number not being a
       power of 2, the execution will take thousands of cycles.
    2. Avoid multiplications, hence the use of rf_table array, that'd make no
       sense on a code just intended for Arduino.
    3. Examination of F_CPU to scale delay duration in the code execution. This
       is dependant on the frequency at which Attiny is set when uploading code.

  With above precautions, the code is Attiny-proof.
*/

#define RF_TRANSMIT_PIN      PB3

#define RF_DEFAULT_REPEAT      8

const unsigned int RF_DELAYFACTOR =  114;
#define RF_FACTOR_TICK  10   // Smallest interval between two signal changes
#define RF_FACTOR_SEP   48   // Sequence separator
#define RF_FACTOR_LONG  500  // Long delay before transmission
#define RF_ZERO 0
#define RF_TICK 1
#define RF_SEP  2
#define RF_LONG 3
unsigned long rf_table[4];

void rf_send_signal(const byte val, const byte factor_index);
void rf_send_code(const uint32_t code);
void mydelay_us(unsigned long duration_us);
void mydelay_ms(unsigned long duration_ms);

void rf_init_rf_table() {
    rf_table[RF_ZERO] = 0;
    rf_table[RF_TICK] = (unsigned long)RF_FACTOR_TICK *
                        (unsigned long)RF_DELAYFACTOR;
    rf_table[RF_SEP] = (unsigned long)RF_FACTOR_SEP *
                       (unsigned long)RF_DELAYFACTOR;
    rf_table[RF_LONG] = (unsigned long)RF_FACTOR_LONG *
                        (unsigned long)RF_DELAYFACTOR;
}

void rf_send_instruction(const uint32_t code, byte repeat = RF_DEFAULT_REPEAT) {

    rf_send_signal(1, RF_LONG);
    rf_send_signal(0, RF_TICK);
    rf_send_signal(1, RF_SEP);

    for (int i = 0; i < repeat; ++i)
        rf_send_code(code);

    rf_send_signal(0, RF_TICK);
    rf_send_signal(1, RF_TICK);

// **WARNING**
//   The last signal sent is a '1' bit.
//   This ensures the transmitter is at rest when sending terminates.
//   Were the last bit a 0, that'd turn the PIN into HIGH and this'd activate
//   the transmitter.
//   Obviously, rf_send_instruction must leave the transmitter at rest when
//   leaving.

}

void rf_send_code(const uint32_t code) {
    uint32_t mask = 1ul << 31;

        // Send leading 0 (don't know why... seems to be part of protocol)
    rf_send_signal(0, RF_TICK);
    rf_send_signal(1, RF_TICK);

    while (mask) {
        byte b = mask & code ? 1 : 0;

        rf_send_signal(b, RF_TICK);
        rf_send_signal(1 - b, RF_TICK);

        mask >>= 1;
    }

    rf_send_signal(1, RF_SEP);
    rf_send_signal(0, RF_ZERO);

// **WARNING**
//   See comment in rf_send_instruction: here, the last bit sent is a '0',
//   meaning, **THE TRANSMITTER IS LEFT ACTIVE**.

}

void rf_send_signal(const byte val, const byte factor_index) {
    digitalWrite(RF_TRANSMIT_PIN, val ? LOW : HIGH);
    mydelay_us(rf_table[factor_index]);
}

#define MYDELAY_STEP        10000

#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || \
    defined(__AVR_ATtiny85__)

  // Attiny case

#if F_CPU == 16000000UL
#define MYDELAY_TIMER_SHIFT 4
#elif F_CPU == 8000000UL
#define MYDELAY_TIMER_SHIFT 3
#else
#define MYDELAY_TIMER_SHIFT 0
#endif

#else

  // non-Attiny case

#define MYDELAY_TIMER_SHIFT 0

#endif

void mydelay_us(unsigned long duration_us) {
    duration_us >>= MYDELAY_TIMER_SHIFT;

    while (duration_us >= MYDELAY_STEP) {
        duration_us -= MYDELAY_STEP;
        delayMicroseconds(MYDELAY_STEP);
    }
    delayMicroseconds(duration_us);
}

void mydelay_ms(unsigned long duration_ms) {
    duration_ms >>= MYDELAY_TIMER_SHIFT;

    while (duration_ms >= MYDELAY_STEP) {
        duration_ms -= MYDELAY_STEP;
        delay(MYDELAY_STEP);
    }
    delay(duration_ms);
}

void setup() {
    rf_init_rf_table();
    pinMode(RF_TRANSMIT_PIN, OUTPUT);
}

void loop() {
    rf_send_instruction(0xABCD0123);
    mydelay_ms(5000);
}

