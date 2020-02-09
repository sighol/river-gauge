class Ultra {
private:
  int trigPin;
  int echoPin;

public:
  Ultra(int trigPin, int echoPin);

  void start();
  void stop();

  double readMicroseconds();

  double readDistance();
};

Ultra::Ultra(int trigPin, int echoPin) {
  this->trigPin = trigPin;
  this->echoPin = echoPin;
}


void Ultra::start() {
  Serial.println("Starting ultra");
  pinMode(this->trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(this->echoPin, INPUT); // Sets the echoPin as an Input
}

void Ultra::stop() {
  // Not implemented..
}

double Ultra::readMicroseconds() {
  // Clean trigger pint
  digitalWrite(this->trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(this->trigPin, HIGH);
  digitalWrite(13, HIGH);
  delayMicroseconds(10);
  digitalWrite(this->trigPin, LOW);
  digitalWrite(13, LOW);

  return pulseIn(this->echoPin, HIGH);
}

/// Gets the distance in centimeters
double Ultra::readDistance() {
  return readMicroseconds() * 0.034 / 2;
}
