
/* ultrasonicMain.ino
*
* Author: Yadukrishnan K M
* Last Updated: 8/dec/2025
* Arduino IDE: 2.3.6
* Libraries: <SoftwareSerial.h>
*/

/************************************************************************************************/

// The code for testing the ultrasonicSensorCall function in  arduino

/************************************************************************************************/

// ============ DEBUG CONTROL ============
#define DEBUG_SENSOR       // Comment this line to disable debug output (simple debug)
#define DEBUG_SENSOR_DEEP  // Comment this line to disable debug output (deep debug)
// =======================================

#include <SoftwareSerial.h>

SoftwareSerial sensorSerial(8, 9);  // RX=8 (from sensor TX), TX=9 (to sensor RX)

// ============ CONFIGURATION THRESHOLDS ============
#define INVALID_DATA_THRESHOLD 10     // Max retries for communication errors (<4 bytes)
#define HEADER_ERROR_THRESHOLD 20     // Max retries for header errors
#define CHECKSUM_FAIL_THRESHOLD 5     // Max retries for checksum errors
#define OUT_OF_RANGE_THRESHOLD 5      // Max retries for out-of-range readings
#define UNSTABLE_READING_THRESHOLD 6  // Max retries for unstable reading between consecutive 2 readings
#define TOLERANCE_BAND_CM 3           // Max allowed difference between dual reads (cm)

/************************************************************************************************/

/*
 * Function: setup
 * Purpose: Initialize serial communications
 * Runs once when Arduino starts
 */

void setup() {
  // Initialize hardware serial for debugging
  Serial.begin(9600);

  // Initialize software serial for sensor communication
  sensorSerial.begin(9600);

  delay(100);  // Let everything stabilize

  Serial.println(F("\n\n================================"));
  Serial.println(F("Ultrasonic Sensor - Dual Read"));
  Serial.println(F("================================"));
  Serial.println(F("Status Codes:"));
  Serial.println(F("  -1 = Not attempted"));
  Serial.println(F("   0 = Success"));
  Serial.println(F("   1 = <4 bytes"));
  Serial.println(F("   2 = Bad header"));
  Serial.println(F("   3 = Checksum error"));
  Serial.println(F("   4 = Out of range"));
  Serial.println(F("   5 = Unstable (dual reads differ)"));

#ifdef DEBUG_SENSOR
  Serial.println(F("\n*** SIMPLE DEBUG MODE ENABLED ***"));
#else
  Serial.println(F("\n*** SIMPLE DEBUG MODE DISABLED ***"));
  Serial.println(F("(To enable, uncomment #define DEBUG_SENSOR)"));
#endif

#ifdef DEBUG_SENSOR_DEEP
  Serial.println(F("\n*** DEEP DEBUG MODE ENABLED ***"));
#else
  Serial.println(F("\n*** DEEP DEBUG MODE DISABLED ***"));
  Serial.println(F("(To enable, uncomment #define DEBUG_SENSOR_DEEP)"));
#endif
  Serial.println(F("================================\n"));
  delay(1000);
}

/************************************************************************************************/

/*
 * Function: loop
 * Purpose: Main program loop - continuously reads sensor
 * Runs repeatedly after setup completes
 */

void loop() {

  unsigned long startTime = millis();  // Or micros() for µs

  float distance = -1;
  int status = -1;

  // Perform sensor reading
  ultrasonicSensorCall(distance, status);

  unsigned long endTime = millis();  // Or micros()
  unsigned long elapsed = endTime - startTime;

  // Print all necessary data irrespective of debug enable
  Serial.println(F(">>> RESULT <<<"));
  Serial.print(F(" | Distance: "));
  if (status == 0) {
    Serial.print(distance);
    Serial.println(F(" cm"));
  } else {
    Serial.print(F("N/A Error: "));
    Serial.println(status);
  }
  Serial.println(F("-----------------------------------\n"));
  Serial.print(" | Execution Time: ");
  Serial.print(elapsed);
  Serial.println(" ms");  // Change to " µs" if using micros()
  Serial.println(F("-----------------------------------\n"));

  delay(4000);  // Wait 4 seconds before next reading
}
//End of file ultrasonicMain.ino