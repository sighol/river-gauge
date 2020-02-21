#include <SoftwareSerial.h>


const int RX = 2;
const int TX = 3;

SoftwareSerial in(RX, TX);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
  }

  Serial.println("FWD: Hardware serial running");
  
  in.begin(9600);
  while (!in) {
  }
  Serial.println("FWD: Software serial running");
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  sim_loop();
}

void sim_loop() { // run over and over
  if (in.available()) {
    Serial.write(in.read());
  }

  if (Serial.available()) {
    in.write(Serial.read());
  }
}
