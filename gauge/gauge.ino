#include <SoftwareSerial.h>
#include "sim800.h"
#include "ultrasound.h"

SoftwareSerial simSerial(11, 3); // RX, TX

void connectHardwareSerial()
{
    // Open serial communications and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println("Hardware serial working");
}

void connectSoftwareSerial()
{
    simSerial.begin(9600);
    while (!simSerial) {
        ;
    }
    Serial.println("SIM800 serial working");
}

Sim800 sim(&simSerial);
// Ultra ultra (9, 10);

void setup() {
    connectHardwareSerial();
    connectSoftwareSerial();

    // ultra.start();
    Serial.println(F("Done with setup"));
}

double distance = 0.0;

void loop() {
    Serial.println(F("Started loop"));

    delay(1000);
    // double distance = ultra.readDistance();
    // Serial.print("Distance is: ");
    // Serial.print(distance);
    // Serial.println(" cm");

    distance++;

    Serial.print(F("The distance is: "));
    Serial.println(distance);

    char url[100];
    memset(url, 0, 100);
    strcat(url, "http://hvorerdetvann.com/post_saggrenda_data/");
    char distance_str[10];
    memset(distance_str, 'a', 10);
    dtostrf(distance, 2, 2, distance_str);
    strcat(url, distance_str);

    Serial.print(F("URL is: "));
    Serial.println(url);
    sim.sendHttpGet(url);
}
