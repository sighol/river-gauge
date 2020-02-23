#include <SoftwareSerial.h>

class Sim800 {
public:
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
        UNKNOWN,
        OK,
        TIMEOUT = 99,
    };
private:
    SoftwareSerial* serial;
    bool DEBUG = true;

    const char* _networkAPN = "telenor.smart";
    const char* _userName = "dj";
    const char* _passWord = "dj";

    // The _responseInfo list and the `Status` enum must be coherent.
    static const uint8_t _responseInfoSize = 12;
    const char* _responseInfo[_responseInfoSize] = {
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

public:

    Status init();
    Status enableBearerProfile();
    Status disableBearerProfile();

    Status initHttp();
    Status closeHttp();

    // Reads everything on the serial buffer. Useful for emptying the buffer.
    String readRaw();

    void printDebug(String *output);

    // Waits for a response and returns with a status code. The timeout is in
    // milliseconds.
    Status checkResponse(uint16_t timeout);


    Sim800(SoftwareSerial* serial) {
        this->serial = serial;
    }

    Status checkConnection();

    Status sendHttpGet(char* url);
};

Sim800::Status Sim800::checkConnection() {
    serial->print(F("AT\r\n"));
    Status status = checkResponse(5000);
    if (status != OK) {
        Serial.println("Failed to connect to SIM800");
    }

    return status;
}

Sim800::Status Sim800::init() {
    // Turn on debug mode
    serial->print(F("at+cmee=2\r\n"));
    Status status = checkResponse(5000);
    if (status != OK) {
        Serial.println("Failed to connect to SIM800");
    }

    // For some reason, the SIM may deactivate different functions. So CFUN=1 will
    // enable all functionality.
    serial->print(F("AT+CFUN=1\r\n"));
    status = checkResponse(5000);
    if (status != OK) {
        Serial.println("Failed to connect to SIM800");
    }

    return status;
}


String Sim800::readRaw() {
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

void Sim800::printDebug(String *output) {
    Serial.print("  Result: {{{");
    Serial.print(*output);
    Serial.println("}}}");
    Serial.flush();
}

Sim800::Status Sim800::checkResponse(uint16_t timeout) {
    unsigned long t = millis();

    delay(1000);

    while (serial->available()) {
      Serial.write(serial->read());
    }

    return OK;

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

            char *mydataIn = strdup(tempData.c_str()); // convert to char data from

            for (byte i = 0; i < _responseInfoSize; i++)
            {
                if((strstr(mydataIn, _responseInfo[i])) != NULL)
                {
                    return (Sim800::Status)i;
                }
            }
        }
    }

    return TIMEOUT;
}


Sim800::Status Sim800::enableBearerProfile() {
    Status status;
    Serial.println("Enabling Bearer profile");

    // Set bearer parameter
    serial->print(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n" ));
    status = checkResponse(10000);
    if (status != OK) {
        Serial.println("Failed to set bearer parameter");
        return status;
    }

    // Set APN
    serial->print(F("AT+SAPBR=3,1,\"APN\",\""));
    serial->print(_networkAPN);
    serial->print(F("\"\r\n"));
    status = checkResponse(1000);
    if (status != OK) {
        Serial.println("Failed to set APN");
        return status;
    }

//    // Set username
//    serial->print(F("AT+SAPBR=3,1,\"USER\",\""));
//    serial->print(_userName);
//    serial->print(F("\"\r\n"));
//    status = checkResponse(1000);
//    if (status != OK) {
//        Serial.println("Failed to set username");
//        return status;
//    }
//
//    // Set password
//    serial->print(F("AT+SAPBR=3,1,\"PWD\",\""));
//    serial->print(_passWord);
//    serial->print(F("\"\r\n"));
//    status = checkResponse(1000);
//    if (status != OK) {
//        Serial.println("Failed to set password");
//        return status;
//    }

    // activate bearer context
    serial->print(F("AT+SAPBR=1,1\r\n"));
    status = checkResponse(10000);
    if (status != OK) {
        Serial.println("Failed to activate bearer context. (No IP)");
        return status;
    }

    delay(2000);

    // get context ip address
    serial->print(F("AT+SAPBR=2,1\r\n "));
    status = checkResponse(10000);
    if (status != OK) {
        Serial.println("Failed to get IP address");
        return status;
    }

    return OK;
}

Sim800::Status Sim800::disableBearerProfile() {
    serial->print(F("AT+SAPBR=0,1\r\n"));

    Status status = checkResponse(20000);
    if (status != OK) {
        Serial.println("Disable berarer profile");
        return status;
    }

    return OK;
}

Sim800::Status Sim800::initHttp() {
    // closeHttp();
    serial->print(F("AT+HTTPINIT\r"));
    return checkResponse(10000);
}

Sim800::Status Sim800::closeHttp() {
    serial->print("AT+HTTPTERM\r\n");
    return checkResponse(10000);
}

Sim800::Status Sim800::sendHttpGet(char* url) {
//    Status status = _enableBearerProfile();
//    if (status != Status::OK) {
//        Serial.println(F("Failed to initialize IP"));
//        return status;
//    }
//
//    if ((status = initHttp()) != OK) {
//        Serial.println("Failed to initialize HTTP");
//        return status;
//    }

    serial->print(F("AT+HTTPPARA=\"CID\",1\r\n"));
    Status status = checkResponse(10000);
    if (status != OK) {
        Serial.println("Failed to set CID");
        return status;
    }

    serial->print(F("AT+HTTPPARA=\"URL\","));
    serial->print(url);
    serial->print(F("\r\n"));
    status = checkResponse(10000);
    if (status != OK) {
        Serial.println(F("Failed to set HTTP address"));
        return status;
    }

    serial->print(F("AT+HTTPACTION=0\r\n"));
    status = checkResponse(10000);
    if (status != OK) {
        Serial.println(F("Failed to set HTTP action"));
        return status;
    }

    serial->print(F("AT+HTTPREAD\r\n"));
    String buffer = readRaw();

    return OK;
}
