/*
MIT License

Copyright (c) 2024 Alexandre Allier

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
  DTMF tone chart

  0b0001 = 1
  0b0010 = 2
  0b0011 = 3
  0b0100 = 4
  0b0101 = 5
  0b0110 = 6
  0b0111 = 7
  0b1000 = 8
  0b1001 = 9
  0b1010 = 0
  0b1011 = *
  0b1100 = #
  0b1101 = A
  0b1110 = B
  0b1111 = C
  0b0000 = D
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                  Variables to modify
#define bin_0 0       // MT8870 Q1 Pin
#define bin_1 1       // MT8870 Q2 Pin
#define bin_2 2       // MT8870 Q3 Pin
#define bin_3 3       // MT8870 Q4 Pin
#define dtmf_pin 4    // MT8870 EST Pin
#define button_pin 9  // Button to activate system manually, this won't keep it activated forever
#define relay 10      // Relay Pin

// To define the relay states
#define OFF HIGH
#define ON LOW

/*
  This is the list of tones required for the module to activate. You have the flexibility to include or exclude tones as needed.
  There is a table containing binary codes and their corresponding values to facilitate easy modifications.
  Typically, radios utilize the first three tones as the device identifier, separated by an asterisk, followed by a 4-digit code for activation.
  This is similar to the DTMF kill code found in some radios, which consists of 5 digits after the asterisk.
  If you want to add or remove codes you'll also need to modify dtmf_input() to add or remove codes.
*/
int dtmf_1 = 0b0001;  // 1
int dtmf_2 = 0b1010;  // 0
int dtmf_3 = 0b0010;  // 2
int dtmf_4 = 0b1011;  // *
int dtmf_5 = 0b0111;  // 7
int dtmf_6 = 0b0111;  // 7
int dtmf_7 = 0b0111;  // 7
int dtmf_8 = 0b0111;  // 7

/*
  Here you can set how much time the relay cycle would be running. I recommend multiples of 4, since the relay takes 4 seconds for each cycle.
  Also here you can set the time the relay will be turned on and off.
*/
const unsigned long active_time = 60000;
const unsigned long relay_time_on = 2000;
const unsigned long relay_time_off = 2000;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int relay_on = 0;
int counter = 0;
int dtmf_state = 0;
int last_dtmf_state = 0;

uint8_t input;
bool signal;

bool relay_state;
bool button_state;
bool button_key;
bool cycle;

unsigned long t_relay;
unsigned long t_dtmf_check;
unsigned long t_reset_dtmf;
unsigned long t_reset_relay;

//////////////////////////////////////////////////      Void Setup      //////////////////////////////////////////////////
void setup() {
  pinMode(bin_0, INPUT);
  pinMode(bin_1, INPUT);
  pinMode(bin_2, INPUT);
  pinMode(bin_3, INPUT);
  pinMode(dtmf_pin, INPUT);
  pinMode(button_pin, INPUT);
  pinMode(relay, OUTPUT);

  digitalWrite(relay, OFF);

  cycle = false;
}

//////////////////////////////////////////////////      Void Loop       //////////////////////////////////////////////////
void loop() {
  dtmf_input();
  dtmf_check();

  relay_loop();
  button();

  reset_dtmf();
  reset_relay();
}

//////////////////////////////////////////////////      DTMF Input      //////////////////////////////////////////////////
void dtmf_input() {
  signal = digitalRead(dtmf_pin);
  input = (0b0000 | (digitalRead(bin_0) << 0) | (digitalRead(bin_1) << 1) | (digitalRead(bin_2) << 2) | (digitalRead(bin_3) << 3));

  if (input == dtmf_1 && relay_on == 0 && counter == 0) {
    relay_on = 1;
  }
  if (input == dtmf_2 && relay_on == 1 && counter == 1) {
    relay_on = 2;
  }
  if (input == dtmf_3 && relay_on == 2 && counter == 2) {
    relay_on = 3;
  }
  if (input == dtmf_4 && relay_on == 3 && counter == 3) {
    relay_on = 4;
  }
  if (input == dtmf_5 && relay_on == 4 && counter == 4) {
    relay_on = 5;
  }
  if (input == dtmf_6 && relay_on == 5 && counter == 5) {
    relay_on = 6;
  }
  if (input == dtmf_7 && relay_on == 6 && counter == 6) {
    relay_on = 7;
  }
  if (input == dtmf_8 && relay_on == 7 && counter == 7) {
    relay_on = 8;
  }
  if (signal == LOW && relay_on == 8) {
    cycle = true;
    relay_on = 0;
  }
}

//////////////////////////////////////////////////      DTMF Check      //////////////////////////////////////////////////
void dtmf_check() {
  int dtmf_reading = digitalRead(dtmf_pin);

  if (dtmf_reading != last_dtmf_state) {
    t_dtmf_check = millis();
  }

  if ((millis() - t_dtmf_check) > 50) {
    if (dtmf_reading != dtmf_state) {
      dtmf_state = dtmf_reading;

      if (dtmf_state == LOW) {
        counter++;
      }
    }
  }

  last_dtmf_state = dtmf_reading;
}

//////////////////////////////////////////////////      Relay Loop      //////////////////////////////////////////////////
void relay_loop() {
  if (cycle == true) {
    relay_state = OFF;
    digitalWrite(relay, relay_state);
    if (millis() - t_relay >= 0) {
      if (relay_state == OFF) {
        relay_state = ON;
      } else {
        relay_state = OFF;
      }
    }
    if (millis() - t_relay >= relay_time_off) {
      digitalWrite(relay, relay_state);
      if (millis() - t_relay >= relay_time_on + relay_time_off) {
        t_relay = millis();
      }
    }

  } else if (cycle == false) {
    digitalWrite(relay, OFF);
  }
}

//////////////////////////////////////////////////      Button Loop     //////////////////////////////////////////////////
void button() {
  button_state = digitalRead(button_pin);
  if (button_state == HIGH) {
    if (button_key == true) {
      cycle = true;
      button_key = false;
    }
  } else if (button_state == LOW) {
    button_key = true;
  }
}

//////////////////////////////////////////////////      Reset DTMF      //////////////////////////////////////////////////
void reset_dtmf() {
  if (signal == HIGH) {
    t_reset_dtmf = millis();
  } else if (signal == LOW && millis() - t_reset_dtmf >= 2000) {
    relay_on = 0;
    counter = 0;
  }
}

//////////////////////////////////////////////////      Reset Relay     //////////////////////////////////////////////////
void reset_relay() {
  if (cycle == false) {
    t_reset_relay = millis();
  }
  if (cycle == true && millis() - t_reset_relay >= active_time) {
    cycle = false;
  }
}
