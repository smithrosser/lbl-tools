#include "lbl.hpp"
#include "parse.hpp"
#include "plant.hpp"

Stage session[MAX_SESSION_SIZE];
State currentState;
String cmd;

int sessionCount, execCount;


void setup() {

  initPins();
  resetSession();
  currentState = IDLE;
  
  Serial.begin(9600);
}
 
void loop() {

  cmd = readSerialInput();
  switch(currentState) {
    
    case IDLE:
    if( !isEmpty(cmd) && cmd.equals("chk") )
      Serial.print("idl");
    else if( !isEmpty(cmd) && cmd.equals("sts") ) {
      currentState = READ;
    }
    else if( !isEmpty(cmd) && cmd.equals("fsh") ) {
      Serial.print("bsy");
      flush_();
      Serial.print("dne");
    }
    else if( !isEmpty(cmd) && cmd.equals("drn") ) {
      Serial.print("bsy");
      drain();
      Serial.print("dne");
    }
    break;

    case READ:
    if( !isEmpty(cmd) ) {
      // end command received?
      if( cmd.equals("ens") ) {
        currentState = WAIT;
        break;
      }
      // otherwise, receive stages from LbL Tools
      if( parseStageFromSerial(cmd, session[sessionCount]) == PARSE_SUCCESS ) {
        sessionCount++;
      }
//      else {
//        Serial.print("err");
//        resetSession();
//      }
    } 
    break;

    case WAIT:
    if( !isEmpty(cmd) && cmd.equals("chk") )
      Serial.print("rdy");
    else if( !isEmpty(cmd) && cmd.equals("dep") ) {
      currentState = EXEC;
      Serial.print("bsy");
    }
    break;

    case EXEC:
    if(execCount > sessionCount) {
      Serial.print("dne");
      resetSession();
      break;
    }
      
    executeStage(session[execCount]);
    execCount++;
    break;

    default:
    break;
  }
}

String readSerialInput() {
  String inputString = "";
  char inputChar;

  while( Serial.available() ) {
    delay(5);
    if( Serial.available() > 0 ) {
      inputChar = Serial.read();
      inputString += inputChar;
    }
  }

  return inputString;
}

void resetSession() {
  for( int i=0; i<MAX_SESSION_SIZE; i++ ) {
    resetStage(session[i]);
  }
  currentState = IDLE;
  sessionCount = execCount = 0;
}
