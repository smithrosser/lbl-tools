#ifndef PLANT_H
#define PLANT_H

#include <Arduino.h>
#include "lbl.hpp"

#define PUMP_0 4
#define PUMP_1 5
#define PUMP_2 6
#define PUMP_W 7
#define PUMP_D 8
#define VALVE 9

#define STAGE_SUCCESS 0
#define STAGE_ERROR 1

void initPins();

int executeStage( Stage s );
void immerse( int pump, int dur );
void immerseOptical( int pump, int dur );
void wash( int dur );
void washContinuous( int dur );
void dry( int dur );
void drain();
void flush_();

#endif
