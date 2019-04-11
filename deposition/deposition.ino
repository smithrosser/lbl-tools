#include "deposition.hpp"
#include <cstring>

Stage session[MAX_STAGES];
State currentState;
int sessionCount = 0;

void setup() {
    pinMode(PUMP_1, OUTPUT);
    pinMode(PUMP_2, OUTPUT);
    pinMode(PUMP_3, OUTPUT);
    pinMode(PUMP_W, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    resetSession();
    resetDevice();
    Serial.begin(9600);
}

void loop() {
    delay(1000);
    switch(currentState) {
        case IDLE :
            Serial.print("IDLE state\n");
            currentState = ( waitForCommand() == SESSION_RECVD ) ? READ : IDLE;
            break;
        case READ :
            Serial.print("READ state\n");
            if( retrieveSessionStage() == PARSE_ERROR ) {
                Serial.print("Parsing error!");
            }
            break;
        case EXEC :
            Serial.print("EXEC state\n");
            break;
    }
}

// STATE OPERATIONS

int waitForCommand() {
    String input = readSerial();
    
    if( input.indexOf("hnd") != -1 ) {
        Serial.print("shk");
        return HANDSHAKE_RECVD;
    }
    else if( input.indexOf("snd") != -1 )
        return SESSION_RECVD;
    else
        return NOTHING_RECVD;
}

// SERIAL OPERATIONS

String readSerial() {
    char inputChar;
    String inputString = "";

    while( Serial.available() && inputString.length() <= MAX_CMD_LENGTH ) {
        delay(5);
            if( Serial.available() > 0) {
                inputChar = Serial.read();
                inputString += inputChar;
            }
            if( inputString.endsWith(";") ) // catcher for command strings
                return inputString;
    }
    return inputString;
}

int parseSessionString(String s) {
    char* point;
    point = strtok(str, ".");

    while(point != NULL)
      Serial.println("Parsed: " + point);
      point = strtok(NULL, ".");
}

bool isInteger(String s) {
  for(int i=0; i<s.length(); i++) {
      if( !isDigit(s.charAt(i)) )
          return false;
  }
  return true;  
}

//  RESETS

void resetSession() {
    for(int i=0; i<MAX_STAGES; i++)
        session[i].type = session[i].pump = session[i].dur = -1;
}

void resetDevice() {
    digitalWrite(PUMP_1, HIGH);
    digitalWrite(PUMP_2, HIGH);
    digitalWrite(PUMP_3, HIGH);
    digitalWrite(PUMP_W, HIGH);

    currentState = IDLE;
}
