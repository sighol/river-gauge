#include <SoftwareSerial.h>

SoftwareSerial simSerial(11, 3); // RX, TX

struct ReadResult
{
	int timeouts;
	String result;
	bool isOk;
};

typedef struct ReadResult ReadResult;

enum Status {
	ERROR,
	NOT_READY,
	READY,
	CONNECT_OK,
	CONNECT_FAIL,
	ALREADY_CONNECT,
	SEND_OK,
	SEND_FAIL,
	DATA_ACCEPT,
	CLOSED,
	ARROW,
	OK,
};

class Sim800
{
private:
	SoftwareSerial* serial;
	bool DEBUG = true;

	const char* _networkAPN = "internet.public";
	const char* _userName = "dj";
	const char* _passWord = "dj";

	// The _responseInfo list and the `Status` enum must be coherent.
	static const uint8_t _responseInfoSize = 13;
	const char* _responseInfo[_responseInfoSize] =
	{
		"ERROR",
		"NOT READY",
		"READY",
		"CONNECT OK",
		"CONNECT FAIL",
		"ALREADY CONNECT",
		"SEND OK",
		"SEND FAIL",
		"DATA ACCEPT",
		"CLOSED",
		">",
		"OK"
	};

	void _enableBearerProfile();
	void _disableBearerProfile();


public:
	enum Status
	{
		ERROR,
		NOT_READY,
		READY,
		CONNECT_OK,
		CONNECT_FAIL,
		ALREADY_CONNECT,
		SEND_OK,
		SEND_FAIL,
		DATA_ACCEPT,
		CLOSED,
		UNKNOWN,
		OK,
		TIMEOUT = 99,
	};
	Sim800(SoftwareSerial* serial)
	{
		this->serial = serial;
	}

	String readRaw()
	{
		uint16_t timeout=0;
		while (!serial->available() && timeout<10000)
		{
			delay(10);
			timeout++;
		}

		if(serial->available())
		{
			String output = serial->readString();
			if (this->DEBUG)
			{
				printDebug(&output);
			}

			return output;
		}

		return "BAD";
	}

	void printDebug(String *output)
	{
		Serial.print("  Result: {{{");
		Serial.print(*output);
		Serial.println("}}}");
	}

	/// Waits for a response and returns with a status code. The timeout is in
	/// milliseconds.

	Status checkResponse(uint16_t timeout);

	bool gprsConnect();
	bool gprsDisconnect();


	Status closeHttp()
	{
		serial->print("AT+HTTPTERM\r\n");
		return checkResponse(10000);
	}

	Status initHttp()
	{
		closeHttp();
		serial->print(F("AT+HTTPINIT\r"));
		return checkResponse(10000);
	}


	void testHttp()
	{
		if (!gprsConnect())
		{
			Serial.println("Failed to initialize GPRS");
			return;
		}

		delay(1000);
		Serial.println("Done waiting!");

		if (initHttp() != OK)
		{
			Serial.println("Failed to initialize HTTP");
			return;
		}

		serial->print(F("AT+HTTPPARA=\"CID\",1\r\n"));
		checkResponse(10000);

		serial->print(F("AT+HTTPPARA=\"URL\","));

		serial->print(F("http://hvorerdetvann.com/post-gauge-data"));
		serial->print(F("\r\n"));
		checkResponse(10000);

		serial->print(F("AT+HTTPACTION=0\r\n"));
		checkResponse(10000);

		serial->print(F("AT+HTTPREAD\r\n"));
		String buffer = readRaw();

		if (!gprsDisconnect())
		{
			Serial.println("Failed to diconnect GPRS");
			return;
		}
	}
};

Sim800::Status Sim800::checkResponse(uint16_t timeout)
{
	unsigned long t = millis();

	while (millis() < t + timeout)
	{
		if (serial->available()) //check if the device is sending a message
		{
			String tempData = serial->readString(); // reads the response

			delay(50); // Stupid, I know.... Please remove me.
			if (DEBUG)
			{
				printDebug(&tempData);
			}

			char *mydataIn = strdup(tempData.c_str()); // convertss to char data from

			for (byte i = 0; i < _responseInfoSize; i++)
			{
				if((strstr(mydataIn, _responseInfo[i])) != NULL)
				{
					return i;
				}
			}
		}
	}

	return TIMEOUT;
}

 void Sim800::_enableBearerProfile()
{
	// This function enable and set the bearer profile for time, location and GPRS service
	String buffer;
	serial->print(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n" ));
	checkResponse(1000);  // set bearer parameter

	serial->print(F("AT+SAPBR=3,1,\"APN\",\""));
	serial->print(_networkAPN);
	serial->print(F("\"\r\n")); // set apn
	checkResponse(1000);

	serial->print(F("AT+SAPBR=3,1,\"USER\",\""));
	serial->print(_userName);
	serial->print(F("\"\r\n")); // set username
	checkResponse(1000);

	serial->print(F("AT+SAPBR=3,1,\"PWD\",\""));
	serial->print(_passWord);
	serial->print(F("\"\r\n")); // set password
	checkResponse(1000);

	serial->print(F("AT+SAPBR=1,1\r\n")); // activate bearer context
	checkResponse(1000);

	serial->print(F("AT+SAPBR=2,1\r\n ")); // get context ip address
	checkResponse(1000);
}

void Sim800::_disableBearerProfile(){
	serial->print(F("AT+SAPBR=0,1\r\n"));

	checkResponse(20000);
}

bool Sim800::gprsDisconnect()
{
	serial->print(F("AT+CIPSHUT\r\n"));
	Status result = checkResponse(65000);
	if(result != OK)
		return false;
	delay(30);
	serial->print(F("AT+CGATT=0\r\n"));
	result = checkResponse(65000);
	if(result != OK)
		return false;
	// disable bearer profile
	_disableBearerProfile();
	return true;
}

bool Sim800::gprsConnect(){
	// This function connects to the internet
	gprsDisconnect();
	String buffer;
	_enableBearerProfile(); // Activate the GPRS connectivity
	// attach the GPRS service
	serial->print(F("AT+CGATT=1\r\n"));
	Status result = checkResponse(20000);
	delay(20);
	if (result != OK)
		return false;

	serial->print(F("AT+CIPMUX=0\r\n"));
	result = checkResponse(10000);
	if (result != OK)
		return false;
	delay(10);
	serial->print(F("AT+CSTT=\""));
	serial->print(_networkAPN);
	serial->print(F("\",\""));
	serial->print(_userName);
	serial->print(F("\",\""));
	serial->print(_passWord);
	serial->print(F("\"\r\n")); //

	result = checkResponse(60000);
	if(result != OK)
		return false;
	delay(10);
	serial->print(F("AT+CIICR\r\n"));
	result = checkResponse(85000);
	if(result != OK)
		return false;
	delay(10);
	serial->print(F("AT+CIFSR\r\n"));
	result = checkResponse(5000); // this basically change to IP STATUS
	// but at the stage I believe IP will already be available
	return true;
}


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

	Sim800 sim(&simSerial);
	sim.testHttp();
	// sim.sendHttpGet(F("http://hvorerdetvann.com"));
}

void sim_loop() {
}

