#include <SoftwareSerial.h>

// Connect RX to PIN3 on the atmega and TX to PIN2 and GND to GND.

const int RX = 2;
const int TX = 3;

SoftwareSerial in(RX, TX);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
  }

  Serial.println("Hardware serial running");
  
  in.begin(9600);
  while (!in) {
  }
  Serial.println("Software serial running");
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  sim_loop();
}

bool next_newline = true;

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
      next_newline = true;
    } else if (next_newline) {
      char timestr[20];
      set_time(timestr);
      Serial.print("[");
      Serial.print(timestr);
      Serial.print("]: ");
      next_newline = false;
    }
    Serial.write(c);
  }

  if (Serial.available()) {
    in.write(Serial.read());
  }
}
