#include <SoftwareSerial.h>

SoftwareSerial simSerial(11, 3); // RX, TX


void sim_setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.println("Hardware serial working");

  // set the data rate for the SoftwareSerial port
  simSerial.begin(9600);
  while (!simSerial) {
    ;
  }

  Serial.println("SIM800 serial working");

  String response = askSimCard("at+gmr");
  Serial.print("Message sent: ");
  Serial.println(response);
}

void sim_loop() {
  while (simSerial.available()) {
    byte simSerialResult = simSerial.read();
    Serial.write(simSerialResult);
  }
}

String askSimCard(String question) {
  simSerial.print(question);
  simSerial.print("\r");

  char response[100];
  int i;
  for (i = 0; simSerial.available(); i++)
  {
    response[i] = simSerial.read();
    Serial.println(response[i]);
  }

  response[i] = '\0';

  return String(response);
}