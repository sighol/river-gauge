#include <SoftwareSerial.h>

// Connect RX to PIN3 on the atmega and TX to PIN2 and GND to GND.

const int RX = 2;
const int TX = 3;

SoftwareSerial in(RX, TX);

void setup() {
  Serial.begin(9600);
  in.begin(9600);
}

void loop() {
  sim_loop();
}

bool char_is_newline = true;

void set_time(char* b) {
  unsigned long t = millis();
  double seconds = t / 1000.0;
  int minutes = int(seconds / 60);
  seconds -= minutes * 60;

  char secstr[6];
  dtostrf(seconds, 5, 2, secstr);
  sprintf(b, "%02u:%s", minutes, secstr);
}

void sim_loop() {
  if (in.available()) {
    char c = in.read();
    if (c == '\n') {
      char_is_newline = true;
    } else if (char_is_newline) {
      char timestr[20];
      set_time(timestr);
      Serial.print("[");
      Serial.print(timestr);
      Serial.print("]: ");
      char_is_newline = false;
    }
    Serial.write(c);
  }

  if (Serial.available()) {
    in.write(Serial.read());
  }
}
