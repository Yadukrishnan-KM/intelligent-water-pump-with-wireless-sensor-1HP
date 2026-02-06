#include <EEPROM.h>
#include "LowPower.h"

// Atmega328 pin assigning
const int ultrasonicTrigPin = 3;
const int ultrasonicechoPin = 4;
const int ultrasonicPwrPin = A2;
const int rfTxPwrPin = A3;
const int txEnablePin = 5;
const int d0Pin = 6;
const int d1Pin = 7;
const int d2Pin = 8;
const int d3Pin = 9;
const int inputSetPin = 2;
const int ledPin = A1;
const int solarChargePin = A0;

// all global variables
float finalDepth;
bool d[4];
bool input = false;
float minLevel, maxLevel;
unsigned long previousMillis = 0;        // will store last time for timeout
unsigned long currentMillis = 0;
int prgmCounter = 0;
int sleepPeriod = 0;

//reset timer variables
unsigned long previousMillisReset = 0;
unsigned long currentMillisReset = 0;

// changing variables
const float depthTolerance = .10;
unsigned long timeOut = 5000;           // milliseconds of timeout of option - initial level setup menu

void setup() {
  pinMode(ultrasonicTrigPin, OUTPUT);
  pinMode(ultrasonicechoPin, INPUT);
  pinMode(ultrasonicPwrPin, OUTPUT);
  pinMode(rfTxPwrPin, OUTPUT);
  pinMode(txEnablePin, OUTPUT);
  pinMode(d0Pin, OUTPUT);
  pinMode(d1Pin, OUTPUT);
  pinMode(d2Pin, OUTPUT);
  pinMode(d3Pin, OUTPUT);
  pinMode(inputSetPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(solarChargePin, INPUT_PULLUP);
  
  digitalWrite(ultrasonicTrigPin, LOW);
  digitalWrite(ultrasonicPwrPin, LOW);
  digitalWrite(d0Pin, LOW);
  digitalWrite(d1Pin, LOW);
  digitalWrite(d2Pin, LOW);
  digitalWrite(d3Pin, LOW);
  digitalWrite(ledPin, LOW);

  Serial.begin(9600);
                                                                                                     Serial.println("starting");
  initialSetup();
  EEPROM.get(0, minLevel); 
  EEPROM.get(4, maxLevel);
                                                                                                     Serial.print("min value eeprom is   ");
                                                                                                     Serial.println(minLevel);
                                                                                                     Serial.print("max value eeprom is  ");
                                                                                                     Serial.println(maxLevel);
}

void loop() {
 //                                                                                                    float minLevel = 50;
 //                                                                                                    float maxLevel = 0;
 finalDepth = ultrasonicRead();
 int wtrLvlPercent = constrain(map(round(finalDepth), round(minLevel), round(maxLevel), 1, 100),1 ,100);  // converting float to int also mapping to 1-100% also adding constains to avoid wrong values
                                                                                                     Serial.print("percentage is : ");
                                                                                                     Serial.println(wtrLvlPercent);
  if(wtrLvlPercent <= 10){                                                                                           
   d[3] = 0;d[2] = 0; d[1] = 0; d[0] = 1;
 }
  else if((wtrLvlPercent >= 15) && (wtrLvlPercent <= 23)){
   d[3] = 0;d[2] = 0; d[1] = 1; d[0] = 0;
 }
  else if((wtrLvlPercent >= 28) && (wtrLvlPercent <= 33)){
   d[3] = 0;d[2] = 0; d[1] = 1; d[0] = 1;
 } 
  else if((wtrLvlPercent >= 38) && (wtrLvlPercent <= 43)){
   d[3] = 0;d[2] = 1; d[1] = 0; d[0] = 0;
 }
  else if((wtrLvlPercent >= 48) && (wtrLvlPercent <= 53)){
   d[3] = 0;d[2] = 1; d[1] = 0; d[0] = 1;
 }
  else if((wtrLvlPercent >= 58) && (wtrLvlPercent <= 63)){
   d[3] = 0;d[2] = 1; d[1] = 1; d[0] = 0;
 }
  else if((wtrLvlPercent >= 68) && (wtrLvlPercent <= 73)){
   d[3] = 0;d[2] = 1; d[1] = 1; d[0] = 1;
 }
  else if((wtrLvlPercent >= 78) && (wtrLvlPercent <= 83)){
   d[3] = 1;d[2] = 0; d[1] = 0; d[0] = 0;
 }
  else if((wtrLvlPercent >= 88) && (wtrLvlPercent <= 93)){
   d[3] = 1;d[2] = 0; d[1] = 0; d[0] = 1;
 }
  else if ((wtrLvlPercent >= 98) && (wtrLvlPercent <= 100)){
   d[3] = 1;d[2] = 0; d[1] = 1; d[0] = 0;
 }
 rfTransmission();
 delay(800);
 if(prgmCounter == 2){
    int batV = readVcc();                                               // reading battery voltage in mV
                                                                        Serial.print("Battery voltage is : ");
                                                                        Serial.println(batV);
   if (batV <= 3400){                                                   // low battery if less than 3.4v
     d[3] = 1;d[2] = 1; d[1] = 0; d[0] = 0;
    }
   else{
     d[3] = 1;d[2] = 1; d[1] = 0; d[0] = 1;                             // battery good
    }

   rfTransmission();
   delay(800);

   int chargingVD = analogRead(solarChargePin);                                      // reading charging condition input voltage in digital 0-1023
                                                                          Serial.print("charging detection pin voltage in digital 0-1023 is : ");
                                                                          Serial.println(chargingVD);
   if(chargingVD <= 512){                                                    // less than half, charging else not
     d[3] = 1;d[2] = 1; d[1] = 1; d[0] = 0;
    }
    else
    {                                                 
     d[3] = 1;d[2] = 0; d[1] = 1; d[0] = 1;
    }

    rfTransmission();
    delay(800);
    prgmCounter = 0;
 }
 else prgmCounter++;
                                                                           Serial.print("prgm counter value is : ");
                                                                           Serial.println(prgmCounter);
 if((wtrLvlPercent >= 25) && (wtrLvlPercent <= 85)) sleepPeriod = 3;
 else if((wtrLvlPercent >= 18) && (wtrLvlPercent <= 92)) sleepPeriod = 1;
 else sleepPeriod = 0;
 for(int i = 0; i <= sleepPeriod; i++){
                                                                           Serial.println("going for sleep");
   delay(100);
   LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, 
            SPI_OFF, USART0_OFF, TWI_OFF);
 }
                                                                           Serial.println("waked up");            

}
