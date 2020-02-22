/*
  File: WebServer.ino
  This example creates a simple web server on your Arduino Uno WiFi. It serves a
  simple web page which shows values of the analog pins, and refreshes every 20 seconds.
  Please type on your browser http://<IP>/arduino/webserver/ or http://<hostname>.local/arduino/webserver/

  Note: works only with Arduino Uno WiFi Developer Edition.

  http://www.arduino.org/learning/tutorials/boards-tutorials/webserver
*/

#include <Wire.h>
#include <AM2320.h>
AM2320 th;

void setup() {
  Serial.begin(9600);
  while (!Serial) {;}

  Serial.println("Hello");
}

void loop() {
  th.Read();
  char tempstr[10];
  char humstr[10];
  char msg[100];
  dtostrf(th.t, 5, 2, tempstr);
  dtostrf(th.h, 5, 2, humstr);
  sprintf(msg, "Temp is %s and Humidity is %s", tempstr, humstr);
  Serial.println(msg);

delay(100);
}


