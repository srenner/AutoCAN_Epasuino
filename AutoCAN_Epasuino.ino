//datasheet at https://github.com/Atlantis-Specialist-Technologies/CAN485/blob/master/Documentation/Datasheet%20AT90CANXX.pdf
#include <ASTCanLib.h>
#include <math.h>

#define DEBUG_KNOB true
#define DEBUG_MPH false

//pins used on board
byte const POS_1_PIN = 2;
byte const POS_2_PIN = 3;
byte const POS_3_PIN = 4;
byte const POS_4_PIN = 5;
byte const POS_5_PIN = 6;
byte const POS_6_PIN = 7;

//other constants
byte const KNOB_BUFFER_LENGTH = 255;                        //length of potentiometer buffer

byte assistMode = 5;                                        //
unsigned long currentMillis = 0;                            //now
unsigned long lastMillis = 0;                               //used to cut time into slices of SPEED_CALC_INTERVAL


byte oldAssistValue = 0;
byte newAssistValue = 0;


void setup() {
  Serial.begin(9600);
  pinMode(POS_1_PIN, INPUT_PULLUP);
  pinMode(POS_2_PIN, INPUT_PULLUP);
  pinMode(POS_3_PIN, INPUT_PULLUP);
  pinMode(POS_4_PIN, INPUT_PULLUP);
  pinMode(POS_5_PIN, INPUT_PULLUP);
  pinMode(POS_6_PIN, INPUT_PULLUP);
  
  Serial.println("Finished initialization");
}

void loop() {
  currentMillis = millis();
  
  if(true) {
    
    float mph = getSpeed();

    //calculate assist level
    byte newAssistMode = getMode(assistMode);
    if(newAssistMode != assistMode) {
      Serial.print("new assist mode: ");
      Serial.println(newAssistMode);
      assistMode = newAssistMode;
    }

    switch(assistMode) {
      case 1:
        //newAssistValue = calculateMode1(mph);
        break;
      case 2:
        //newAssistValue = calculateMode2(mph);
        break;
      case 3:
        //newAssistValue = calculateMode3(mph);
        break;
      case 4:
        //newAssistValue = calculateMode4(mph);
        break;
      case 5:
        //newAssistValue = calculateMode5(mph);
        break;
      case 6:
        //newAssistValue = calculateMode6(mph);
        break;
    }

    if(newAssistValue != oldAssistValue) {
      sendToPot(newAssistValue);
      oldAssistValue = newAssistValue;
    }
    
    lastMillis = currentMillis;
  }
}

byte getMode(byte previousMode) {
  byte mode = previousMode;
  if(!digitalRead(POS_1_PIN)) {
    mode = 1;
  }
  else if(!digitalRead(POS_2_PIN)) {
    mode = 2;
  }
  else if(!digitalRead(POS_3_PIN)) {
    mode = 3;
  }
  else if(!digitalRead(POS_4_PIN)) {
    mode = 4;
  }
  else if(!digitalRead(POS_5_PIN)) {
    mode = 5;
  }
  else if(!digitalRead(POS_6_PIN)) {
    mode = 6;
  }
  return mode;
}

float getSpeed() {
  //todo
  return 0.0;
}

void sendToPot(byte pos) {
  if(DEBUG_KNOB) {
    Serial.print("setting digital knob to position ");
    Serial.println(pos);  
  }
  //todo
}

void sendToCan() {
  //send steering mode to the CAN bus in case anyone needs to read the status
}
