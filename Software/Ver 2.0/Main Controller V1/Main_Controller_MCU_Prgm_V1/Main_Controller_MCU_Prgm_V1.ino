#include <LiquidCrystal_I2C.h>
#include  <Wire.h>
#include <EEPROM.h>
#include <LedBlink.h>

void(* resetFunc) (void) = 0;                           // for resting the system by going to start point

// *********************** Declaring all the pins in arduino *********************************************************************************

const byte interruptPin = 2;
const byte d3Pin = 8;  // check pin assigned in the interrupt section for the data
const byte d2Pin = 7;
const byte d1Pin = 6;
const byte d0Pin = 5;
const byte motorRelayPin = 12;
const byte motorONPin = A2;
const byte motorOFFPin = A3;
const byte voltPresentPin = 9;
const byte lowVoltPin = 10;
const byte dryRunPin = 11; 
const byte alarmLightPin = 13;
const byte alarmBuzzerPin = 4;
const byte onLEDPin = A1;
const byte offLEDPin = A0;
const byte inputSwitchPin = A7;
const byte lcdLEDBrightPin = 3;

LedBlink motorONLed(onLEDPin);
LedBlink motorOFFLed(offLEDPin);
LedBlink alarmLed(alarmLightPin);

// *************************************************** LCD Related *****************************************************************************

LiquidCrystal_I2C lcd(0x27,  16, 2);                    //initialization. the parameters are id, column, row

// LCD memory for sysmbol
byte powerSymbol[] = {0x02, 0x0C,0x18,0x1F,0x1F,0x06,0x0C,0x10};
byte abnormal[] = {0x0C, 0x0C,0x0C,0x0C,0x0C,0x00,0x0C,0x0C};
byte PumpRunningSymbol[] = {0x1F, 0x1F,0x1F,0x1F,0x1F,0x1F,0x04,0x1F};
byte alarmSymbol[] = {0x01, 0x03,0x07,0x1F,0x1F,0x07,0x03,0x01};
byte batterySymbol[] = {0x0E, 0x0E,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F};
byte transmitterGoodSymbol[] = {0x0E, 0x11,0x04,0x0A,0x00,0x04,0x04,0x04};
byte timerSymbol[] = {0x1F, 0x11,0x0A,0x04,0x04,0x0E,0x1F,0x1F};
byte autoSymbol[] = {0x0E, 0x0A,0x0E,0x0A,0x00,0x1F,0x11,0x1F};
byte leftArrowSymbol[] = {0x07,0x0E,0x1C,0x18,0x18,0x1C,0x0E,0x07};
byte rightArrowSymbol[] = {0x1C,0x0E,0x07,0x03,0x03,0x07,0x0E,0x1C};

int brightValue = 255;                         // LCD BAcklight brightness powerSymbol

// ************************************************* Menu, alarm and settings Related ***********************************************************

int buttonValue = 0;                            // variable for reading navigation buttons

bool pumpOperationModeSetting = true;           // normally auto
int pumpCutINLevelSetting = 2;                // normally 20% 1-10%, 2-20%, 3-30%, 4-40%, 5-50%,6-60%, 7-70%, 8-80%, 9-90%
int pumpScheduleTimeSetting = 1;                // normally off 1-off, 2-every 6hr, 3-every 8hr, 4-every 12hr, 5-every 24hr
bool lowVoltCutoffSetting = true;               // normally cutoff enabled
int pumpAoffTimerSetting = 3 ;                  // normally 45min  1-off, 2-30min, 3-45min, 4-60min, 5-90min, 6-120min
bool dryrunCutoffSetting = true;                //dryrun cutoff enabled
int brightValueSetting = 4;                     // brightness vale 1-10%, 2-25% 3-50%, 4-75%, 5-100%

unsigned long pumpScheduleTime = 0; // in millisecons
bool pumpScheduleTimeCounterFlag = true;
unsigned long previousCounterTimePumpSchedule = 0;
unsigned long currentCounterTimePumpSchedule = 0;
bool scheduleOnFlag = false;

unsigned long pumpAoffTime = 0;                 // in millisec
bool pumpAoffTimerFlag = true;
unsigned long previousCounterTimeAoffTimer = 0;
unsigned long currentCounterTimeAoffTimer = 0;

int homeScreenFlag = 0; // 0 - home screen - idle, 1 - menu - right button, 2 - alarm menu - left button
int mainmenuSelectCaseNo = 1;
int alarmSelectionCaseNo = 1;
//flag for entering into each edit section by pressing enter from mainmenu
bool operationModeEditFlag = false;
bool pumpCutINLevelEditFlag = false;
bool pumpScheduleEditFlag = false;
bool lowVoltCutoffEditFlag = false;
bool pumpAoffTimerEditFlag = false;
bool dryrunCutoffEditFlag = false;
bool brightValueEditFlag = false;
bool factoryResetEditFlag = false;
// Menu timeout control variables
unsigned long menuTimeoutPreviousMillis = 0;
unsigned long menuTimeoutCurrentMillis = 0;
bool menuTimeoutCounterFlag = false;

// transmitter missing timer variables
unsigned long previousCounterTransmitterTimeout = 0;
unsigned long currentCountertransmitterTimeout = 0;
bool transmitterTimeoutCounterFlag = true;
bool transmitterError = false;

//volatile because it is sharing between ISR(interrupt) and normal prgm
volatile bool b[4];
volatile bool d[4];
volatile bool batLow = false;
volatile bool solarCharg = false;

bool dryRunFlag = false;
bool lowVoltFlag = false;
bool voltPresentFlag = false;
bool waterLvlLowFlag = false;
//bool waterLvlHighFlag = false;
bool manualOnFlag = false;
//bool manualOffFlag = false;
bool motorStateMemoryFlag = false;
bool motorPinStatusFlag = false;
bool pwrFailFlag = false;


bool motorStateMemory = false;
bool dryRun = false;
bool lowVolt = false;

bool voltPresent = false;
int waterLvl = 0;

unsigned long previousMills0 = 0;
unsigned long currentMills0 = 0;
unsigned long previousMills1 = 0;
unsigned long currentMills1 = 0;
unsigned long previousMills2 = 0;
unsigned long currentMills2 = 0;
unsigned long previousMills3 = 0;
unsigned long currentMills3 = 0;

int dryrunCounter = 0; 
int lowVoltCounter = 0;
int voltPresentCounter = 0;
int voltPresentCounter1 = 0;

int onSwitchCounter = 0;
int offSwitchCounter = 0;
int resetSwitchCounter = 0;

// varables for motor on buzzer
unsigned long previousCountermotorONBuzzer = 0;
unsigned long currentCountermotorONBuzzer = 0;
bool motorOnBuzzertCounterFlag = true;

// alarm related

bool pumpRunningError = false; // only low volt, dryrun and power failure while pump running not critical
bool alarm = false;
bool sensorOutofRange = false;
bool pumpTimerAlarm = false;
bool pumpTxErrorAlarm = false;

// reset after 2 dyas

unsigned long currentMillisRest = 0; 

// vcc voltage
int batV = 0;

// buzzer sound during pump running variables
const long onDuration = 200;                       // ON time for buzzer during motor on
const long offDuration = 30000;                    // OFF time for buzzer during motor on
int BuzzState = HIGH;                               // initial state of buzzer
long rememberTime = 0;                             // this is used by the code


void setup() {
  // declaring all pinmodes
  pinMode(interruptPin, INPUT);
  pinMode(motorONPin, INPUT);
  pinMode(motorOFFPin, INPUT);
  pinMode(d0Pin, INPUT);
  pinMode(d1Pin, INPUT);
  pinMode(d2Pin, INPUT);
  pinMode(d3Pin, INPUT);
  pinMode(dryRunPin, INPUT_PULLUP);
  pinMode(lowVoltPin, INPUT_PULLUP);
  pinMode(voltPresentPin, INPUT_PULLUP);
  pinMode(motorRelayPin, OUTPUT);
  pinMode(alarmLightPin, OUTPUT);
  pinMode(alarmBuzzerPin, OUTPUT);
  pinMode(onLEDPin, INPUT);
  pinMode(offLEDPin, OUTPUT);
  pinMode(inputSwitchPin, INPUT);
  pinMode(lcdLEDBrightPin, OUTPUT);
  // starting interrupt
  attachInterrupt(digitalPinToInterrupt(interruptPin), interupt, RISING);

 Serial.begin(9600);
 // starting lcd related
 lcd.init();                                            // initialize lcd screen
 lcd.createChar(0, powerSymbol);                        // ac present and good symbol  
 lcd.createChar(1, abnormal);                           // abonrmal symbol is !
 lcd.createChar(2, PumpRunningSymbol);                  // pump running and good symbol 
 lcd.createChar(3, alarmSymbol);                        // alarm symbol
 lcd.createChar(4, batterySymbol);                      // battery symbol
 lcd.createChar(5, transmitterGoodSymbol);              // transmitterGoodSymbol   
 lcd.createChar(6, timerSymbol);                        // timer Symbol 
 lcd.createChar(7, autoSymbol);                         // auto operation Symbol 
 //lcd.backlight();                                        // turn on the backlight using i2c
 analogWrite(lcdLEDBrightPin, brightValue);             // to turn on backlight using PWM
 startUpScreen();                                       // only use in setup for working after reset, the first startup messgae screen with led working checks
 initialFactoryReset();
}

void loop() {
// comapring the waterlevel and saving to an variable waterLvl in percentage
 if(!d[3] && !d[2] && !d[1] && d[0])  waterLvl = 10;
 else if (!d[3] && !d[2] && d[1] && !d[0])  waterLvl = 20;
 else if (!d[3] && !d[2] && d[1] && d[0])  waterLvl = 30;
 else if (!d[3] && d[2] && !d[1] && !d[0])  waterLvl = 40;
 else if (!d[3] && d[2] && !d[1] && d[0])  waterLvl = 50;
 else if (!d[3] && d[2] && d[1] && !d[0])  waterLvl = 60;
 else if (!d[3] && d[2] && d[1] && d[0])  waterLvl = 70;
 else if (d[3] && !d[2] && !d[1] && !d[0])  waterLvl = 80;
 else if (d[3] && !d[2] && !d[1] && d[0])  waterLvl = 90;
 else if (d[3] && !d[2] && d[1] && !d[0])  waterLvl = 100;

 //                                                                                                        Serial.print("tank level is ");
  //                                                                                                       Serial.println(waterLvl);
 batV = readVcc();                                           // rewading vcc voltage in mV
 pwrBordReading();
 lcdRefresh();
 //                                                                                                        Serial.print("flags are dryrun, lowvolt, voltpresent: ");
 //                                                                                                        Serial.print(dryRunFlag);Serial.print("  ");Serial.print(lowVoltFlag);Serial.print("  ");Serial.println(voltPresentFlag);

// ****************************************************************************************************************************************************************************************************

// reading manual switches saving status to variable manualOnFlag (on and off having time delay need to press and hold) (for turning ON and OFF the pump)

 bool switchON = digitalRead(motorONPin);
 if(switchON) onSwitchCounter++;
 else onSwitchCounter = 0;
 if(onSwitchCounter == 35){
  manualOnFlag = true;
  onSwitchCounter = 0;
 }
 bool switchOFF = digitalRead(motorOFFPin);
 if(switchOFF) offSwitchCounter++;
 else offSwitchCounter = 0;
 if(offSwitchCounter == 35){
   offSwitchCounter = 0;                                 // manual off disabling all the motor on flags
   manualOnFlag = false;
   waterLvlLowFlag = false;
   scheduleOnFlag = false;
   currentCounterTimePumpSchedule = 0;
   previousCounterTimePumpSchedule = 0;
   pumpScheduleTimeCounterFlag = true;
   dryrunCounter = 0;                                                  // reseting the dryrunCounter and flag after off
   dryRunFlag = false;
 }

// ****************************************************************************************************************************************************************************************************
// Alarm resetiing by press and hold on down button

 buttonValue = buttonRead(inputSwitchPin);                   
 if(buttonValue == 2) resetSwitchCounter++;
 else resetSwitchCounter = 0;
 if(resetSwitchCounter == 35){
   resetSwitchCounter = 0;
   dryrunCounter = 0;                                                // reseting the dryrunCounter and flag 
   dryRunFlag = false;
   pumpTimerAlarm = false;                                           // reseting alarms
   transmitterError = false; previousCounterTransmitterTimeout = 0; currentCountertransmitterTimeout = 0; transmitterTimeoutCounterFlag = true;        // for resetting the transmitter missing timer
   batLow = false;
   sensorOutofRange = false;
   pumpTxErrorAlarm = false;
 }



// ****************************************************************************************************************************************************************************************************

 // checking waterlevel for setting and resetting the flags if auto selected (for tuning ON and OFF the pump)
 if(!pumpTxErrorAlarm && !dryRunFlag && !transmitterError && !pumpTimerAlarm){
   switch(pumpCutINLevelSetting){
     case 1: 
     if(pumpOperationModeSetting && waterLvl == 10)  waterLvlLowFlag = true;
     break;
     case 2: 
     if(pumpOperationModeSetting && (waterLvl == 10 || waterLvl == 20))  waterLvlLowFlag = true;
     break;
     case 3: 
     if(pumpOperationModeSetting && (waterLvl == 10 || waterLvl == 20 || waterLvl == 30))  waterLvlLowFlag = true;
     break;
     case 4: 
     if(pumpOperationModeSetting && (waterLvl == 10 || waterLvl == 20 || waterLvl == 30 || waterLvl == 40))  waterLvlLowFlag = true;
     break;
     case 5: 
     if(pumpOperationModeSetting && (waterLvl == 10 || waterLvl == 20 || waterLvl == 30 || waterLvl == 40 || waterLvl == 50))  waterLvlLowFlag = true;
     break;
     case 6: 
     if(pumpOperationModeSetting && (waterLvl == 10 || waterLvl == 20 || waterLvl == 30 || waterLvl == 40 || waterLvl == 50 || waterLvl == 60))  waterLvlLowFlag = true;
     break;
     case 7: 
     if(pumpOperationModeSetting && (waterLvl == 10 || waterLvl == 20 || waterLvl == 30 || waterLvl == 40 || waterLvl == 50 || waterLvl == 60 || waterLvl == 70))  waterLvlLowFlag = true;
     break;
     case 8: 
     if(pumpOperationModeSetting && (waterLvl == 10 || waterLvl == 20 || waterLvl == 30 || waterLvl == 40 || waterLvl == 50 || waterLvl == 60 || waterLvl == 70 || waterLvl == 80))  waterLvlLowFlag = true;
     break;
     case 9: 
     if(pumpOperationModeSetting && (waterLvl == 10 || waterLvl == 20 || waterLvl == 30 || waterLvl == 40 || waterLvl == 50 || waterLvl == 60 || waterLvl == 70 || waterLvl == 80 || waterLvl == 90))  waterLvlLowFlag = true;
     break;
     default:
     waterLvlLowFlag = false;
     break;
   }
 }

   if (waterLvl == 100) {                                                       // tank full disable all the motor on flags
     waterLvlLowFlag = false;
     manualOnFlag = false;
     scheduleOnFlag = false;
     currentCounterTimePumpSchedule = 0;
     previousCounterTimePumpSchedule = 0;
     pumpScheduleTimeCounterFlag = true;
   }
   //                                                                                                     Serial.print("flags are manualOn, waterLvlLow: ");
   //                                                                                                     Serial.print(manualOnFlag);Serial.print("  ");Serial.println(waterLvlLowFlag);


// ******************************************************************************************************************************************************************************************************

// motor schedule timer (for turning ON the pump)
if(!pumpTxErrorAlarm && !dryRunFlag && !transmitterError && !pumpTimerAlarm){
    if(pumpScheduleTimeSetting != 1){
  if(pumpScheduleTimeSetting == 2) pumpScheduleTime = 21600000;              // 6Hr in millisec
  else if(pumpScheduleTimeSetting == 3) pumpScheduleTime = 28800000;         // 8Hr in millisec
  else if(pumpScheduleTimeSetting == 4) pumpScheduleTime = 43200000;         // 12Hr in millisec
  else if(pumpScheduleTimeSetting == 5) pumpScheduleTime = 86400000;         // 24Hr in millisec
  if(pumpScheduleTimeCounterFlag){
    previousCounterTimePumpSchedule = millis();
    pumpScheduleTimeCounterFlag = false;
  }
  currentCounterTimePumpSchedule = millis();
  if((currentCounterTimePumpSchedule - previousCounterTimePumpSchedule) >= pumpScheduleTime) {
    scheduleOnFlag = true;
    currentCounterTimePumpSchedule = 0;
    previousCounterTimePumpSchedule = 0;
    pumpScheduleTimeCounterFlag = true;
  }
 // else scheduleOnFlag = false;
}
else{
  scheduleOnFlag = false;
  currentCounterTimePumpSchedule = 0;
  previousCounterTimePumpSchedule = 0;
  pumpScheduleTimeCounterFlag = true;
}

}


//***********************************************************************************************************************************************************************************************************

// pump running off timer. restriting max continious running time of pump (for turning OFF the pump)

if(pumpAoffTimerSetting != 1){
 if(motorPinStatusFlag){
  if(pumpAoffTimerSetting == 2) pumpAoffTime = 1800000;                     // 30min
  else if(pumpAoffTimerSetting == 3) pumpAoffTime = 2700000;                // 45min
  else if(pumpAoffTimerSetting == 4) pumpAoffTime = 3600000;                // 60min
  else if(pumpAoffTimerSetting == 5) pumpAoffTime = 5400000;                // 90min
  else if(pumpAoffTimerSetting == 6) pumpAoffTime = 7200000;                // 120min
  if(pumpAoffTimerFlag){
    previousCounterTimeAoffTimer = millis();
    pumpAoffTimerFlag = false;
  }
  currentCounterTimeAoffTimer = millis();
  if((currentCounterTimeAoffTimer - previousCounterTimeAoffTimer) >= pumpAoffTime){
     pumpTimerAlarm = true;
     waterLvlLowFlag = false;
     manualOnFlag = false;
     scheduleOnFlag = false;
     currentCounterTimePumpSchedule = 0;
     previousCounterTimePumpSchedule = 0;
     pumpScheduleTimeCounterFlag = true;
     pumpAoffTimerFlag = true;
  }
 }
 else{
  previousCounterTimeAoffTimer = 0;
  currentCounterTimeAoffTimer = 0;
  pumpAoffTimerFlag = true;
 }
}
else{
  previousCounterTimeAoffTimer = 0;
  currentCounterTimeAoffTimer = 0;
  pumpAoffTimerFlag = true;
}

// ************************************************************************************************************************************************************************************************************
// if transmitter error while running by auto or schedule operation will terminated and only allows manual operation during transmitter error

if(transmitterError && (waterLvlLowFlag || scheduleOnFlag)){
 waterLvlLowFlag = false;
 scheduleOnFlag = false;
 pumpTxErrorAlarm = true;
}

//************************************************************************************************************************************************************************************************************

// motor on off section

if(waterLvlLowFlag || manualOnFlag || scheduleOnFlag) motorStateMemory = true;                   // variable for entering the code for turning on pump motor
else motorStateMemory = false;

// if on command is present checking all the necessary condition and decide to turn on
if (motorStateMemory){                                                         // entering to the pump on codes
  if(!dryRunFlag && !lowVoltFlag && voltPresentFlag){                          // checking all the flags before turning on the motor
      if(motorOnBuzzertCounterFlag){
        previousCountermotorONBuzzer = millis();
        motorOnBuzzertCounterFlag = false;
      }
      currentCountermotorONBuzzer = millis();
      if((currentCountermotorONBuzzer - previousCountermotorONBuzzer) >= 2500){
       digitalWrite(motorRelayPin, HIGH);
       motorPinStatusFlag = true;
       pumpRunningError = false;
       Serial.println("inside on");
       buzzer();
      }
      else{
        digitalWrite(alarmBuzzerPin, HIGH);                  // turn on buzzer 2.5 sec before turrning on motor
      }

  }
  else{
    digitalWrite(motorRelayPin, LOW);
    motorPinStatusFlag = false;
    pumpRunningError = true;
    motorOnBuzzertCounterFlag = true;                        // ensure that in next on sequence buzzer will work first
    digitalWrite(alarmBuzzerPin, LOW);                       // ensure that buzzer is off
    BuzzState = HIGH;                                        // initial state of buzzer
  }
}
else {
  digitalWrite(motorRelayPin, LOW);
  motorPinStatusFlag = false;
   //                                                                                                                                                   Serial.print("inside off");
  motorOnBuzzertCounterFlag = true;                        // ensure that in next on sequence buzzer will work first
  digitalWrite(alarmBuzzerPin, LOW);                       // ensure that buzzer is off
  pumpRunningError = false;
  BuzzState = HIGH;                                        // initial state of buzzer
}

// ************************************************************************************************************************************************************************************************************

// motor power led indications
if(motorPinStatusFlag){
   digitalWrite(onLEDPin, HIGH);
   digitalWrite(offLEDPin, LOW);
   }
else{
 if(!motorStateMemory){
   digitalWrite(onLEDPin, LOW);
   if(!voltPresent) digitalWrite(offLEDPin, HIGH);  // on keeps on if ac present else both turning off
   else digitalWrite(offLEDPin, LOW);
   }
 if(motorStateMemory){
   motorONLed.ledBlink (HIGH, 500);
   motorOFFLed.ledBlink (LOW, 500);
  }
}
// ************************************************************************************************************************************************************************************************************
 // checking for transmitter timeout of 5 min, timer reset inside ISR

 if(transmitterTimeoutCounterFlag){
  previousCounterTransmitterTimeout = millis();
  transmitterTimeoutCounterFlag = false;
 }
 currentCountertransmitterTimeout = millis();
 if((currentCountertransmitterTimeout - previousCounterTransmitterTimeout) >= 300000){           // transmitter error time is 5 min
  transmitterError = true;
 }
 else {
  transmitterError = false;
 }

// *************************************************************************************************************************************************************************************************************

 // alarm handling section also alarm buzzer and led

 
 if(pumpRunningError || transmitterError || batLow || sensorOutofRange ||  pumpTimerAlarm || pumpTxErrorAlarm){
  alarmLed.ledBlink (HIGH, 500);
  //digitalWrite(alarmLightPin, HIGH);
  alarm = true;
 }
 else{
  alarmLed.cancelLedBlink(LOW);
  //digitalWrite(alarmLightPin, LOW);
  alarm = false;
 }

// *************************************************************************************************************************************************************************************************************

// reset section after 10 days for resfreshing everything if motor not running otherwise waiting for pump off

currentMillisRest = millis();
//                                                                                          Serial.println(currentMillisRest);    
if(!motorStateMemory){
   if(currentMillisRest >= 864000000){  //172800000 in millisecond for 10 days 
   resetFunc(); //making system reset
  }
}





 //Serial.println("-----------------------------------------------------------------------------------------------------------------------------------------");
 delay(80);
} 
