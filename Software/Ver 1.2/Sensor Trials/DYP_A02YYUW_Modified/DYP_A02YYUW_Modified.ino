/*
Auto water pump TX unit sensor reading code

It is a function which is using in Tx unit for reliable level measuremets. The sensor using here is DYPA02YYUW ultrasonic level sensor.

Author: Yadukrishnan K M
version V1.0           
Last modified  08/03/2025
*/

#include <SoftwareSerial.h>
#define ultrasonicechoPin 11
#define ultrasonicTrigPin 10
#define ultrasonicPwrPin 12
float depthTolerance = 0.05;
SoftwareSerial mySerial(ultrasonicechoPin,ultrasonicTrigPin); // RX, TX

void setup(){
 Serial.begin(57600);
 mySerial.begin(9600); 
}

void loop(){

  float measurement = ultrasonicRead();
  Serial.print("measurement is :"); Serial.println(measurement);
  delay(5000);
}

float ultrasonicRead() {

  // All the variables for this fuction
  long previousMillis_waitforSensor = 0;
  long currentMillis_waitforSensor = 0;
  long timeout_waitforSensor = 1000;                         // sensor waiting period for scuceess communcation
  float finalDepth = 0;
  bool trueDepthFlag, mainTimeOutFlag;
  long previousMillis_WaitForCorrectValue = millis();
  long currentMillis_WaitForCorrectValue = 0;
  long timeout_WaitForCorrectValue = 10000;                  // Sensor read fuction timeout if measurement fails

  // to ensure it will go through the while loop atleast once
  trueDepthFlag = false;                 
  mainTimeOutFlag = false;

  digitalWrite(ultrasonicPwrPin, HIGH);                      // to turn on the ultrasonic sensor module
  delay(100);                                                // to settile down the module

  while (!trueDepthFlag && !mainTimeOutFlag) {               // to find the true value by measuring multiple times
  
   unsigned char data[4] = {0};
   float depth[3] = {0};

    for (int i = 0; i < 3; i++) {                            // to meausre 3 times sequencly

      previousMillis_waitforSensor = millis();

    // sensor data read
      do {
        for (int i = 0; i < 4; i++) data[i] = mySerial.read();
        currentMillis_waitforSensor = millis();
        if ((currentMillis_waitforSensor - previousMillis_waitforSensor) >= timeout_waitforSensor) {
          Serial.println("Sensor timeout happened!");
          break;
        }
      } while (mySerial.read() == 0xff);

      mySerial.flush();

      if (data[0] == 0xff) {
        int sum;
        float Depth;
        sum = (data[0] + data[1] + data[2]) & 0x00FF;
        if (sum == data[3]) {
          Depth = (data[1] << 8) + data[2];
          Depth = Depth / 10;
          depth[i] = Depth;
          if (Depth > 3) {
            Serial.print("Depth = "); Serial.print(Depth); Serial.println("cm");
          } else {

            Serial.println("Below the lower limit");
          }
        } else Serial.println("Data ERROR");
      } else Serial.println("Start byte ERROR");
      delay(75);
    }

    finalDepth = (depth[0] + depth[1] + depth[2]) / 3;               // find average of 3 values
    Serial.print("Inside ultrasonic read while loop final depth is :"); Serial.println(finalDepth);
    Serial.print(" depth0 is :"); Serial.println(depth[0]);
    Serial.print(" depth1 is :"); Serial.println(depth[1]);
    Serial.print(" depth2 is :"); Serial.println(depth[2]);
    if (finalDepth <= (depth[1] - (depth[1] * depthTolerance)) || finalDepth >= (depth[1] + (depth[1] * depthTolerance))) {  // to find the final value is true or not
      trueDepthFlag = false;                                                                                                 // ensure while loop again
      Serial.println("final depth is false, average not matching");
    } else {
      if (finalDepth >= 400){
        trueDepthFlag = false;                                       // reject the measurement if it is above 4 meter
        Serial.println("final depth is false, above 4 meter");
      }
      else{
        trueDepthFlag = true;                                        // exit while loop
        Serial.println("final depth is true");
      }
    }
    currentMillis_WaitForCorrectValue = millis();
    if ((currentMillis_WaitForCorrectValue - previousMillis_WaitForCorrectValue) >= timeout_WaitForCorrectValue){
      mainTimeOutFlag = true;                                        // for avoiding infine running of while loop, timeout is decided in the top
      Serial.println("Timeout and exiting while loop and sensor read function");
    }

  }

  digitalWrite(ultrasonicPwrPin, LOW);                               // turn off ultrasonic sensor module

// only returning the measured depth if it is valied otherwise zero
  if(trueDepthFlag){
  Serial.print("ultrasonic read successful final depth is :"); Serial.println(finalDepth);
  return finalDepth;
  }
  else {
    Serial.println("ultrasonic read failed");
    return 0;
  }
}