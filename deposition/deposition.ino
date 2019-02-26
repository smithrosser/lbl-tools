#define MAX_STAGES 32
#define END 0
#define FILL 1
#define WASH 2
#define DRY 3

#define PUMP_1 4
#define PUMP_2 5
#define PUMP_3 6
#define PUMP_W 7
#define PUMP_D 8
#define VALVE_D 9

int count;
int sType[MAX_STAGES], sPump[MAX_STAGES], sDur[MAX_STAGES];
String inputString = "";
bool cmdLoop;

void setup() {
  Serial.begin(9600);
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PUMP_1, OUTPUT);
  pinMode(PUMP_2, OUTPUT);
  pinMode(PUMP_3, OUTPUT);
  pinMode(PUMP_W, OUTPUT);

  digitalWrite(PUMP_1, HIGH);
  digitalWrite(PUMP_2, HIGH);
  digitalWrite(PUMP_3, HIGH);
  digitalWrite(PUMP_W, HIGH);
}

// Initial state (waits for specific three-letter commands).
void loop() {    
    while( Serial.available() ) {
      delay(3);   // allow buffer to fill
      if( Serial.available() > 0 ) {
        char inputChar = Serial.read();    // read serial char
        inputString += inputChar;             // and add it to string
      }
      if( inputString.length() > 2 && inputString.indexOf("hnd") >= 0 ) {       // LbL Tools sends the command "hnd" to the device, and waits for the word "shk" (handshake)
        Serial.print("shk");
        inputString = "";  
      }
      else if( inputString.length() > 2 && inputString.indexOf("snd") >= 0 ) {  // LbL Tools sends the command "snd" ('send') when it wants to send the session
        inputString = "";
        retrieveCommands();
        beginLbL();
      }
    }
}

// Parses input serial stream into commands
void retrieveCommands() {
  Serial.print("rcv");      // once LbL Tools receives the signal "rcv" ('receiving'), the session stream begins
  String cmd = "";
  int type, dur, pump;
  resetSession(); count = 0; cmdLoop = true;
  while(cmdLoop) {
    while( Serial.available() ) {
      delay(3);
      if( Serial.available() > 0 ) {
        char inputChar = Serial.read();
        inputString += inputChar;
        if( inputString.endsWith(";") ) {   // build up input string until ';'
          for (int i=0; i<inputString.length(); i++) {
             if(inputString.substring(i, i+1).equals(":")) {
              if( cmd.equals("f") )
                type = FILL;
              else if( cmd.equals("w") )
                type = WASH;
              else if( cmd.equals("d") )
                type = DRY;
              cmd = "";
             }
             else if(inputString.substring(i, i+1).equals(",")) {
              pump = cmd.toInt();
              cmd = "";
             }
             else if(inputString.substring(i, i+1).equals(";")) {
              if( cmd.equals("end") ) {
                type = END;
                cmdLoop = false;
              }
              else
                dur = cmd.toInt();
              sType[count] = -1; sDur[count] = -1; sPump[count] = -1;
              switch(type) {
                case FILL : sType[count] = FILL; sDur[count] = dur; sPump[count] = pump; break;
                case WASH : sType[count] = WASH; sDur[count] = dur; break;
                case DRY  : sType[count] = DRY; sDur[count] = dur; break;
                case END  : sType[count] = END; break;
                default   : break;
              }
              cmd = "";
              count++;
             }
             else {
                cmd.concat(inputString.substring(i, i+1));
             }
          }
          inputString = "";
        }
      }
    }
  }
  Serial.print("rdy");
}

void beginLbL() {
  delay(50);
  Serial.print("bsy");
  for( int i=0; i<count; i++ ) {  
    switch(sType[i]) {
      case FILL : immerse(sPump[i], sDur[i]); break;
      case WASH : wash(sDur[i]); break;
      case DRY : dry(sDur[i]); break;
      case END : break;
      default : break;
    }
  }
  Serial.print("dne");
}

void immerse(int pump, int dur) {
  digitalWrite(pump+4, LOW);
  delay(dur*1000);
  digitalWrite(pump+4, HIGH); 
}

void wash(int dur) {
  digitalWrite(PUMP_W, LOW);
  delay(dur*1000);
  digitalWrite(PUMP_W, HIGH);
}

void dry(int dur) {
  digitalWrite(LED_BUILTIN, HIGH); 
  delay(dur*1000); 
  digitalWrite(LED_BUILTIN, LOW);
}

void resetSession() {
  for(int i=0; i<count; i++) {
    sType[i] = -1;
    sPump[i] = -1;
    sDur[i] = -1;
  }
}
