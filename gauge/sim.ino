#include <SoftwareSerial.h>

SoftwareSerial mySerial(11, 3); // RX, TX

void sim_setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.println("Hardware serial working");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
}

void sim_loop() { // run over and over
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
}