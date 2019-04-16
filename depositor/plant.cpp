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
  else if ( s.type == STAGE_WASH ) wash( s.dur );
  else if ( s.type == STAGE_DRY  ) dry( s.dur );
  else return STAGE_ERROR;

  return STAGE_SUCCESS;
}

void immerse( int pump, int dur ) {
  digitalWrite( pump+4, LOW );
  delay( dur*1000 );
  digitalWrite( pump+4, HIGH ); 
}

void wash( int dur ) {
  digitalWrite( PUMP_W, LOW );
  delay( dur*1000 );
  digitalWrite( PUMP_W, HIGH );
}

void dry( int dur ) {
	digitalWrite( VALVE, LOW );
  delay( dur*1000 );
  digitalWrite( VALVE, HIGH );
}
