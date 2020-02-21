#include "wdsleep.h"
#include "ultrasound.h"
#include "sim800.h"

// http://www.gammon.com.au/forum/?id=11149
// http://www.engblaze.com/hush-little-microprocessor-avr-and-arduino-sleep-mode-basics/
// http://www.circuitbasics.com/wp-content/uploads/2015/11/SRD-05VDC-SL-C-Datasheet.pdf
// https://www.faranux.com/wp-content/uploads/2016/12/SIM800-Series_AT-Command-Manual_V1.09.pdf
// Smart side for å teste http GET-kall: https://webhook.site/#!/480b8799-0b9f-4145-a356-01df766fd0cd/10e62448-4d9b-4bf8-89a2-be7e27754f37/1
// Atemega pinout: https://www.arduino.cc/en/Hacking/PinMapping168

// Sim800L koder:
// atz             Reset all parameters
// at+cmee=2       Enter debug mode: It'll show more error info
// AT&F            Factory reset
// AT+CFUN=1       Enable all phone functionalities
// AT+COPS?        

// Husk å sette CFUN til 1, som er full funksjonalitet.

// Pass på å ha nok strøm inn mot sim800l. Vi fikk det til å fungere med 5.4 V.
// Når sim800L binker annethvert sekund, har den koblet seg opp.
// Når den er oppe, og fungerer, kan man ringe til SIM-kortet. Den vil da si "RING" for hvert ring i konsollen.
 
const int LED = 13;
const int RELAY = 7;

const int SIM_RX = 10;
const int SIM_TX = 11;

const int ULTRA_TRIG = 90;
const int ULTRA_ECHO = 100;

SoftwareSerial simSerial(SIM_RX, SIM_TX); // RX, TX
Sim800 sim(&simSerial);
Ultra ultra (ULTRA_TRIG, ULTRA_ECHO);

void connectHardwareSerial() {
    // Open serial communications and wait for port to open:
    Serial.begin(9600);

    unsigned long t = millis();

    // If HW serial is not connected, wait maximum 5 seconds
    while (!Serial || t + 5000 > millis()) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
    
    Serial.println("Hardware serial working");
}

void connectSoftwareSerial() {
    simSerial.begin(9600);
    
    unsigned long t = millis();
    
    while (!simSerial || t + 5000 > millis()) {
        ;
    }
    Serial.println("SIM800 serial working");
}

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(LED, OUTPUT);
  pinMode(RELAY, OUTPUT);

  connectHardwareSerial();
  connectSoftwareSerial();

  Serial.println("Hello Karsten and Sigurd. I am woke!");

  while (simSerial.available()) {
    Serial.write(simSerial.read());
  }

  delay(3000);
  simSerial.println("AT\r\n");

  while (simSerial.available()) {
    Serial.write(simSerial.read());
  }
}

double getDistance() {
  ultra.start();

  const int N = 10;
  double distances[N];
  
  for (int i = 0; i < N; i++) {
    distances[i] = ultra.readDistance();
    Serial.print(F("Got distance: "));
    Serial.print(distances[i]);
    Serial.println(F(" cm"));

    delay(50);
  }

  insertionSort(distances, N);
  printArray(distances, N);

  double finalDistance = distances[N / 2];
  Serial.print(F("Got total distance: "));
  Serial.print(finalDistance);
  Serial.println(F(" cm"));
  return finalDistance;
}

void onWakeup() {
  Serial.println("Woke up");
  digitalWrite(RELAY, HIGH);

  unsigned long t = millis();

  delay(4000);

  Serial.println("Reading distance...");
  //double distance = getDistance();
  double distance = 13.37;
  Serial.print("Distance is ");
  Serial.print(distance);
  Serial.println(" cm");

  Serial.println("Waiting until SIM800 is believed to be ready...");
  while (millis() < t + 15000) {
    delay(100);
  }

  Serial.println("Talking to SIM800...");
  initHttp();

  Serial.println("HTTP Ready. Sending...");
  digitalWrite(LED, HIGH);
  send(distance);
  digitalWrite(LED, LOW);
  
  digitalWrite(RELAY, LOW);
  Serial.println("Done working. Going to sleep. ZZZzzzzz");
}

void initHttp() {
  sim.enableBearerProfile();
  sim.initHttp();
}

void insertionSort(double arr[], int n) { 
  double key;
    int i, j; 
    for (i = 1; i < n; i++) { 
        key = arr[i]; 
        j = i - 1; 
  
        /* Move elements of arr[0..i-1], that are 
          greater than key, to one position ahead 
          of their current position */
        while (j >= 0 && arr[j] > key) { 
            arr[j + 1] = arr[j]; 
            j = j - 1; 
        } 
        arr[j + 1] = key; 
    } 
}

void printArray(double arr[], int n) 
{ 
    int i; 
    for (i = 0; i < n; i++) {
      Serial.print(arr[i]);
      Serial.print(", ");
    }
    Serial.println(""); 
} 

void turnOffPins() {
  //To reduce power, setup all pins as inputs with no pullups
  for(int x = 1 ; x < 18 ; x++){
    pinMode(x, INPUT);
    digitalWrite(x, HIGH); //changed from low to high which was 400ua change
  }
}

void send(double distance) {
  char url[100];
  memset(url, 0, 100);
  strcat(url, "http://webhook.site/69aa92ee-768a-44c6-9bad-fe2d41862b68?distance=");
  char distance_str[10];
  memset(distance_str, 0, 10);
  dtostrf(distance, 2, 2, distance_str);
  strcat(url, distance_str);
  
  sim.sendHttpGet(url);
}

// the loop function runs over and over again forever
void loop() {
  onWakeup();

  myWatchdogEnable (0b100001);  // 8 seconds
  myWatchdogEnable (0b100001);  // 8 seconds
  myWatchdogEnable (0b100001);  // 8 seconds
  myWatchdogEnable (0b100001);  // 8 seconds
  myWatchdogEnable (0b100001);  // 8 seconds
  myWatchdogEnable (0b100001);  // 8 seconds
  myWatchdogEnable (0b100001);  // 8 seconds
}

void sim_loop() { // run over and over
  if (simSerial.available()) {
    Serial.write(simSerial.read());
  }
  if (Serial.available()) {
    simSerial.write(Serial.read());
  }
}
