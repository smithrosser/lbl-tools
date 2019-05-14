#include "plant.hpp"

void initPins() {
  pinMode(PUMP_0, OUTPUT);
  pinMode(PUMP_1, OUTPUT);
  pinMode(PUMP_2, OUTPUT);
  pinMode(PUMP_W, OUTPUT);
  pinMode(PUMP_D, OUTPUT);
  pinMode(VALVE, OUTPUT);
  pinMode(A5, INPUT);

  digitalWrite(PUMP_0, HIGH);
  digitalWrite(PUMP_1, HIGH);
  digitalWrite(PUMP_2, HIGH);
  digitalWrite(PUMP_W, HIGH);
  digitalWrite(PUMP_D, HIGH);
  digitalWrite(VALVE, HIGH);
}

int executeStage( Stage s ) { // interprets stage type, and calls appropriate function
  
  if      ( s.type == STAGE_FILL ) immerseOptical( s.pump, s.dur );
  else if ( s.type == STAGE_WASH ) washContinuous( s.dur );
  else if ( s.type == STAGE_DRY  ) dry( s.dur );
  else return STAGE_ERROR;

  return STAGE_SUCCESS;
}

void immerse( int pump, int dur ) { // `dumb' immersion, fills for specific time determined by vessel
  digitalWrite( pump+4, LOW );
  delay(1500);
  digitalWrite( pump+4, HIGH);
  delay( dur*1000 );
  drain();
}

void immerseOptical( int pump, int dur ) {  // `smart' immersion, uses optical fibre to detect if full
  int thres = 600; // 600 indicates `full'
  int count = 0;
  
  while(analogRead(A5) < thres && count < 1200) {
    digitalWrite(pump+4, LOW);
    delay(50);
    count += 50;  // count ensures no overflow if optical detection fails
  }
  
  digitalWrite(pump+4, HIGH);
  delay(dur*1000);
  drain();
}

void wash( int dur ) {  // sequential fill and drain wash
  int count = 0;
  while(count < dur) {
    digitalWrite(PUMP_W, LOW);
    delay(1200);
    digitalWrite(PUMP_W, HIGH);
    count++;
    drain();
    count += 4;
  }
}

void washContinuous( int dur ) {  // continuous flow wash, wash & drain pumps operate simultaneously
  int count = 0;
  digitalWrite(PUMP_W, LOW);
  delay(1200);
  count++;
  while(count < dur) {
    digitalWrite(PUMP_D, LOW);
    delay(1000);
    count++;
  }
  digitalWrite(PUMP_W, HIGH);
  drain();
}

void dry( int dur ) {
	digitalWrite( VALVE, LOW );
  delay( dur*1000 );
  digitalWrite( VALVE, HIGH );
}

void drain() {
  digitalWrite(PUMP_D, LOW);
  delay(2500);
  digitalWrite(PUMP_D, HIGH);
}

void flush_() {               // runs each pump for a while, ensures
  digitalWrite(PUMP_D, LOW);
  
  digitalWrite(PUMP_0, LOW);
  delay(2000);
  digitalWrite(PUMP_0, HIGH);

  digitalWrite(PUMP_1, LOW);
  delay(2000);
  digitalWrite(PUMP_1, HIGH);

  digitalWrite(PUMP_2, LOW);
  delay(2000);
  digitalWrite(PUMP_2, HIGH);

  washContinuous(3);
}
