#ifndef DEVICE_PARSE_H
#define DEVICE_PARSE_H

#include <Arduino.h>
#include <string.h>
#include "lbl.hpp"

#define MAX_STR_LENGTH 32
#define MAX_TOKENS 2
#define MAX_PUMP_ID 2

#define PARSE_SUCCESS 0
#define PARSE_ERROR 1

int parseStageFromSerial( String str, Stage& s );
int parseType( char* token, Stage& s );
int parseParams( char* token, Stage& s );

bool isInteger( const char* str );
bool isEmpty( const String str );

#endif
