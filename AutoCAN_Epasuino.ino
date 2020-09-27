//datasheet at https://github.com/Atlantis-Specialist-Technologies/CAN485/blob/master/Documentation/Datasheet%20AT90CANXX.pdf
#include <ASTCanLib.h>
#include <math.h>
#include <AutoCAN.h>

#define DEBUG_KNOB true
#define DEBUG_MPH true
#define DEBUG_CAN true

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

float currentMph = 0.0;
float previousMph = 0.0;

//can bus variables ////////////////////////////////////////////////////////////

uint8_t canBuffer[8] = {};

#define MESSAGE_PROTOCOL  0     // CAN protocol (0: CAN 2.0A, 1: CAN 2.0B)
#define MESSAGE_LENGTH    8     // Data length: 8 bytes
#define MESSAGE_RTR       0     // rtr bit

volatile unsigned long canCount = 0;
volatile unsigned long canUnhandledCount = 0;

volatile st_cmd_t canMsg;

typedef struct {
  int16_t id;
  unsigned long counter;
  uint8_t* data;
} canData;

#define MS_BASE_ID    1512  // set this to match the MegaSquirt setting, default is 1512
#define MSG_MS_BASE   0     // array index of allCanMessages[] to find the base id
#define MSG_MS_PLUS1  1     // array index 
#define MSG_MS_PLUS2  2     // etc...
#define MSG_MS_PLUS3  3     // ...
#define MSG_MS_PLUS4  4     // last element of array

volatile canData* allCanMessages[5];  //array of all CAN messages we are interested in receiving

volatile canData canBase;
volatile canData canPlus1;
volatile canData canPlus2;
volatile canData canPlus3;
volatile canData canPlus4;
volatile canData canSpd;

uint8_t canBufferBase[8] = {};
uint8_t canBufferPlus1[8] = {};
uint8_t canBufferPlus2[8] = {};
uint8_t canBufferPlus3[8] = {};
uint8_t canBufferPlus4[8] = {};
uint8_t canBufferSpd[8] = {};

volatile canData canTemp;
uint8_t canBufferTemp[8] = {};

// can objects for sending messages
st_cmd_t txMsg;
uint8_t txBuffer[8] = {0,0,0,0,0,0,0,0};

unsigned long vssCanTest = 0;

uint8_t assistOutput[6][3] = {
  {0, 0, 0},
  {50, 25, 0},
  {75, 25, 25},
  {100, 50, 50},
  {100, 75, 50},
  {100, 100, 100}
};
uint8_t currentAssistOutput = 100;

float vss = 0.0;
float previousVss = 0.0;

ISR(CANIT_vect) {
  canCount++;

  unsigned i;   
  char save_canpage=CANPAGE;   
  
  unsigned mob = CANHPMOB; // get highest prio mob   
  CANPAGE = mob & 0xf0;   
  mob >>= 4; // -> mob number 0..15   
  //ASSERT( (CANSTMOB & ~0xa0) ==0); // allow only RX ready and DLC warning   
    
  canTemp.id = (CANIDT2>>5) | (CANIDT1 <<3);
  
  register char length; 
  length = CANCDMOB & 0x0f;
  for (i = 0; i <length; ++i)   
  {
    canTemp.data[i] = CANMSG;
  }   
  
  CANSTMOB = 0;           // reset INT reason   
  CANCDMOB = 0x80;        // re-enable RX on this channel   
  CANPAGE = save_canpage; // restore CANPAGE   

  if(true) 
  {
    switch(canTemp.id)
    {
      case MS_BASE_ID:
        allCanMessages[MSG_MS_BASE]->counter++;
        fillCanDataBuffer(MSG_MS_BASE, &canTemp);
        break;
      case MS_BASE_ID + 1:
        allCanMessages[MSG_MS_PLUS1]->counter++;
        fillCanDataBuffer(MSG_MS_PLUS1, &canTemp);
        break;
      case MS_BASE_ID + 2:
        allCanMessages[MSG_MS_PLUS2]->counter++;
        fillCanDataBuffer(MSG_MS_PLUS2, &canTemp);
        break;
      case MS_BASE_ID + 3:
        allCanMessages[MSG_MS_PLUS3]->counter++;
        fillCanDataBuffer(MSG_MS_PLUS3, &canTemp);
        break;
      case MS_BASE_ID + 4:
        allCanMessages[MSG_MS_PLUS4]->counter++;
        fillCanDataBuffer(MSG_MS_PLUS4, &canTemp);
        break;
      case CAN_SH_VSS_MSG_ID:
        //vssCounter++;
        canSpd.counter++;
        for(int i = 0; i < 2; i++)
        {
          canSpd.data[i] = canTemp.data[i];
        }       
        break;
      default:
        //vssCanTest++;
        break;
    }
  }
}

void fillCanDataBuffer(int index, canData* canTemp)
{
  for(int i = 0; i < 8; i++)
  {
    allCanMessages[index]->data[i] = canTemp->data[i];
  }
}


void setup() {
  canInit(500000);
  Serial.begin(1000000);
  pinMode(POS_1_PIN, INPUT_PULLUP);
  pinMode(POS_2_PIN, INPUT_PULLUP);
  pinMode(POS_3_PIN, INPUT_PULLUP);
  pinMode(POS_4_PIN, INPUT_PULLUP);
  pinMode(POS_5_PIN, INPUT_PULLUP);
  pinMode(POS_6_PIN, INPUT_PULLUP);
  
#pragma region setup can bus

  txMsg.pt_data = &txBuffer[0];      // reference message data to transmit buffer

  clearBuffer(&canBufferTemp[0]);
  canTemp.data = &canBufferTemp[0];

  canBase.id = MS_BASE_ID;
  canBase.counter = 0;
  clearBuffer(&canBufferBase[0]);
  canBase.data = &canBufferBase[0];
  allCanMessages[0] = &canBase;

  canPlus1.id = MS_BASE_ID + 1;
  canPlus1.counter = 0;
  clearBuffer(&canBufferPlus1[0]);
  canPlus1.data = &canBufferPlus1[0];
  allCanMessages[1] = &canPlus1;
  
  canPlus2.id = MS_BASE_ID + 2;
  canPlus2.counter = 0;
  clearBuffer(&canBufferPlus2[0]);
  canPlus2.data = &canBufferPlus2[0];
  allCanMessages[2] = &canPlus2;
  
  canPlus3.id = MS_BASE_ID + 3;
  canPlus3.counter = 0;
  clearBuffer(&canBufferPlus3[0]);
  canPlus3.data = &canBufferPlus3[0];
  allCanMessages[3] = &canPlus3;

  canPlus4.id = MS_BASE_ID + 4;
  canPlus4.counter = 0;
  clearBuffer(&canBufferPlus4[0]);
  canPlus4.data = &canBufferPlus4[0];
  allCanMessages[4] = &canPlus4;

  canSpd.id = CAN_SH_VSS_MSG_ID;
  canSpd.counter = 0;
  clearBuffer(&canBufferSpd[0]);
  canSpd.data = &canBufferSpd[0];

  CANSTMOB |= (1 << RXOK);
  CANGIE |= (1 << ENRX);

  CANIE1 |= (1 << IEMOB14);
  CANIE1 |= (1 << IEMOB13);
  CANIE1 |= (1 << IEMOB12);
  CANIE1 |= (1 << IEMOB11);
  CANIE1 |= (1 << IEMOB10);
  CANIE1 |= (1 << IEMOB9);
  CANIE1 |= (1 << IEMOB8);

  CANIE2 |= (1 << IEMOB7);
  CANIE2 |= (1 << IEMOB6);
  CANIE2 |= (1 << IEMOB5);
  CANIE2 |= (1 << IEMOB4);
  CANIE2 |= (1 << IEMOB3);
  CANIE2 |= (1 << IEMOB2);
  CANIE2 |= (1 << IEMOB1);
  CANIE2 |= (1 << IEMOB0);

  CANGIE |= (1 << ENIT);

  clearBuffer(&canBuffer[0]);
  canMsg.cmd      = CMD_RX_DATA;
  canMsg.pt_data  = &canBuffer[0];
  canMsg.ctrl.ide = MESSAGE_PROTOCOL; 
  canMsg.id.std   = 0;
  canMsg.id.ext   = 0;
  canMsg.dlc      = MESSAGE_LENGTH;
  canMsg.ctrl.rtr = MESSAGE_RTR;

  while(can_cmd(&canMsg) != CAN_CMD_ACCEPTED);

  if(DEBUG_CAN)
  {
    Serial.println("CAN bus initialized");
  }

  #pragma endregion


  Serial.println("Finished initialization");
}

void loop() {
  currentMillis = millis();
  
  if(true) {
    
    noInterrupts();
    processCanMessages();
    previousMph = currentMph;
    currentMph = engine_spd.currentValue;
    previousVss = vss;
    vss = engine_vss.currentValue;
    interrupts();

    if(currentMph != previousMph && DEBUG_MPH)
    {
      Serial.print(currentMph);
      Serial.println("mph");
      //Serial.print(" ");
      //Serial.println(vss);
    }

    //calculate assist level
    byte newAssistMode = getMode(assistMode);
    if(newAssistMode != assistMode) {
      if(DEBUG_KNOB)
      {
        Serial.print("new assist mode: ");
        Serial.print(newAssistMode);
        Serial.print(" - ");
        Serial.println(epasModeDescriptions[newAssistMode]);
      }
      assistMode = newAssistMode;
      sendToCan(assistMode);
    }

    uint8_t previousAssistOutput = currentAssistOutput;
    currentAssistOutput = getDesiredAssistLevel(assistMode, currentMph);

    if(previousAssistOutput != currentAssistOutput)
    {
      Serial.print("assist: ");
      Serial.print(currentAssistOutput);
      Serial.println("%");
    }

    switch(assistMode) {
      case 0:
        //newAssistValue = calculateMode1(mph);
        break;
      case 1:
        //newAssistValue = calculateMode2(mph);
        break;
      case 2:
        //newAssistValue = calculateMode3(mph);
        break;
      case 3:
        //newAssistValue = calculateMode4(mph);
        break;
      case 4:
        //newAssistValue = calculateMode5(mph);
        break;
      case 5:
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

uint8_t getDesiredAssistLevel(uint8_t mode, float mph)
{
  uint8_t speedZone = 0;
  if(mph > 29.9)
  {
    speedZone = 1;
  }
  if(mph > 59.9)
  {
    speedZone = 2;
  }

  return assistOutput[mode][speedZone];
}

void processCanMessages()
{
    engine_vss.currentValue = ((allCanMessages[MSG_MS_PLUS4]->data[0] * 256) + allCanMessages[MSG_MS_PLUS4]->data[1]) / 10.0;
    engine_spd.currentValue = ((canSpd.data[0] * 256) + canSpd.data[1]) / 10.0;
    
}

byte getMode(byte previousMode) {
  byte mode = previousMode;
  if(!digitalRead(POS_1_PIN)) {
    mode = 0;
  }
  else if(!digitalRead(POS_2_PIN)) {
    mode = 1;
  }
  else if(!digitalRead(POS_3_PIN)) {
    mode = 2;
  }
  else if(!digitalRead(POS_4_PIN)) {
    mode = 3;
  }
  else if(!digitalRead(POS_5_PIN)) {
    mode = 4;
  }
  else if(!digitalRead(POS_6_PIN)) {
    mode = 5;
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

//send steering mode to the CAN bus in case anyone needs to read the status
void sendToCan(byte modeIndex) {

  txBuffer[0] = modeIndex;
  
  // Setup CAN packet.
  txMsg.ctrl.ide = MESSAGE_PROTOCOL;    // Set CAN protocol (0: CAN 2.0A, 1: CAN 2.0B)
  txMsg.id.std   = CAN_EPAS_MSG_ID;     // Set message ID
  txMsg.dlc      = MESSAGE_LENGTH;      // Data length: 8 bytes
  txMsg.ctrl.rtr = MESSAGE_RTR;         // Set rtr bit
  txMsg.pt_data = &txBuffer[0];         // reference message data to transmit buffer

  // Send command to the CAN port controller
  txMsg.cmd = CMD_TX_DATA;       // send message
  // Wait for the command to be accepted by the controller
  while(can_cmd(&txMsg) != CAN_CMD_ACCEPTED);
  //Serial.println("command accepted?");
  // Wait for command to finish executing
  while(can_get_status(&txMsg) == CAN_STATUS_NOT_COMPLETED);
  //Serial.println("send complete?");

}
