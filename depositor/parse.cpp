/*
 * SERIAL PARSER
 * 
 * Parses the string received over the serial connection
 * and creates a Stage object with the relevant parameters.
 * 
 * Usage:
 * parseStageFromSerial( String to be parsed, Stage object to be modified ); 
 * 
 */

#include "parse.hpp"

int parseStageFromSerial( String str, Stage& s ) {

  if( str.length() > MAX_STR_LENGTH )
    return PARSE_ERROR;

  // split string based on delimiters ':' and ';'
  // place them in an array for further handling
  
  char serialString[MAX_STR_LENGTH];
  str.toCharArray(serialString, str.length());
  
  char* ptr;
  char* tokens[2];

  byte index = 0;
  ptr = strtok(serialString, ":;");

  while( ptr != NULL ) {
    tokens[index] = ptr;
    index++;
    ptr = strtok(NULL, ":;");
  }

  // call parsers for type and params

  if      ( parseType(tokens[0], s) != PARSE_SUCCESS )    return PARSE_ERROR;
  else if ( parseParams(tokens[1], s) != PARSE_SUCCESS )  return PARSE_ERROR;

  return PARSE_SUCCESS;
}

int parseType( char* token, Stage& s ) {

  if      ( strcmp(token, "f") == 0 ) s.type = STAGE_FILL;
  else if ( strcmp(token, "w") == 0 ) s.type = STAGE_WASH;
  else if ( strcmp(token, "d") == 0 ) s.type = STAGE_DRY;
  else return PARSE_ERROR;

  return PARSE_SUCCESS;
}

int parseParams( char* token, Stage& s ) {

  // check if multiple params: if not, single argument will be 'dur'

  if( strstr(token, ",") == NULL ) {
    if( isInteger(token) ) {
      s.dur = atoi(token);
      return PARSE_SUCCESS;
    }
    else return PARSE_ERROR;
  }

  // tokenize the token, this time by ','

  char* ptr;
  char* paramTokens[2];

  byte index = 0;
  ptr = strtok(token, ",");

  while(ptr != NULL && index < 2){
    paramTokens[index] = ptr;
    index++;
    ptr = strtok(NULL, ",");
  }

  // convert tokens to relevant ints ('pump' and 'duration')

  if( isInteger(paramTokens[0]) && isInteger(paramTokens[1]) ) {
    
    if( atoi(paramTokens[0]) <= MAX_PUMP_ID )   // only three immerse pumps available
      s.pump = atoi(paramTokens[0]);
    else return PARSE_ERROR;
    
    s.dur = atoi(paramTokens[1]);
  }
  else return PARSE_ERROR;

  return PARSE_SUCCESS; 
}

bool isInteger( const char* str ) {
  
  for (int i=0; i<strlen(str); i++) {
    if( !isdigit(str[i]) )
      return false;
  }
  return true;
}

bool isEmpty( const String str ) {
  return (str.length() == 0);
}
