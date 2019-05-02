#include "plant.hpp"

void initPins() {
  pinMode(PUMP_0, OUTPUT);
  pinMode(PUMP_1, OUTPUT);
  pinMode(PUMP_2, OUTPUT);
  pinMode(PUMP_W, OUTPUT);
  pinMode(PUMP_D, OUTPUT);
  pinMode(VALVE, OUTPUT);

  digitalWrite(PUMP_0, HIGH);
  digitalWrite(PUMP_1, HIGH);
  digitalWrite(PUMP_2, HIGH);
  digitalWrite(PUMP_W, HIGH);
  digitalWrite(PUMP_D, HIGH);
  digitalWrite(VALVE, HIGH);
}

int executeStage( Stage s ) {
  
  if      ( s.type == STAGE_FILL ) immerse( s.pump, s.dur );
  else if ( s.type == STAGE_WASH ) washContinuous( s.dur );
  else if ( s.type == STAGE_DRY  ) dry( s.dur );
  else return STAGE_ERROR;

  return STAGE_SUCCESS;
}

void immerse( int pump, int dur ) {
  digitalWrite( pump+4, LOW );
  delay(900);
  digitalWrite( pump+4, HIGH);
  delay( dur*1000 );
  drain();
}

void wash( int dur ) {
  int count = 0;
  while(count < dur) {
    digitalWrite(PUMP_1, LOW);
    delay(700);
    digitalWrite(PUMP_1, HIGH);
    count++;
    drain();
    count += 4;
  }
}

void washContinuous( int dur ) {
  int count = 0;
  digitalWrite(PUMP_1, LOW);
  delay(500);
  count++;
  while(count < dur) {
    digitalWrite(PUMP_D, LOW);
    delay(1000);
    count++;
  }
  digitalWrite(PUMP_1, HIGH);
  drain();
}

void dry( int dur ) {
	digitalWrite( VALVE, LOW );
  delay( dur*1000 );
  digitalWrite( VALVE, HIGH );
}

void drain() {
  digitalWrite(PUMP_D, LOW);
  delay(2000);
  digitalWrite(PUMP_D, HIGH);
}
