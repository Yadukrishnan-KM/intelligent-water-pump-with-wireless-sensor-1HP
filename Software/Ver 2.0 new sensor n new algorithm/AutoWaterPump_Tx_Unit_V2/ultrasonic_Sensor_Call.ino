
/* ultrasonic_Sensor_Call.ino
*
* Author: Yadukrishnan K M, Jahnavi K
* Last Updated: 8/dec/2025
* Arduino IDE: 2.3.6
* Libraries: <SoftwareSerial.h>
*/

/************************************************************************************************/

/*
 * Ultrasonic Sensor Interface Sketch for DYP-A02YYTW-V2.0 (Arduino)
 * 
 * Purpose: Reliable UART-controlled ultrasonic distance measurement (3-450 cm)
 * with error handling, retries, and stability checks for industrial/harsh use.
 * 
 * Features:
 * - Dual consecutive reads for stability (tolerance: 5 cm default).
 * - Adaptive retries: Comm err (10x), Header (20x), Checksum (5x), OOR (5x), Unstable (6x).
 * - Debug: #define DEBUG_SENSOR (basic) / DEBUG_SENSOR_DEEP (detailed) for Serial output.
 *
 * Status Codes:
 * -1 = Not attempted
 *  0 = Success
 *  1 = Communication error due to byte loss (<4 bytes)
 *  2 = Bad header
 *  3 = Checksum mismatch
 *  4 = Out of range (valid: 3cm to 450cm)
 *  5 = Dual reads differ too much (unstable)
 * 
 * Usage: In loop(), call ultrasonicSensorCall(distCm, status);
 *        - distCm: Valid distance (cm) or -1 (fail)
 *        - status: 0=OK, 1=Comm Err, 2=Header, 3=Checksum, 4=OOR, 5=Unstable
 * 
 */

/************************************************************************************************/

/*
 * DYP-A02YYTW-V2.0 UART-Controlled Ultrasonic Sensor Overview
 * 
 * This waterproof (IP67-rated probe) ultrasonic sensor measures distances via echolocation:
 * Emits 40 kHz pulse, measures time-of-flight (temp-compensated) for 3-450 cm range.
 * 
 * Key Specs:
 * - Range: 3-450 cm (1 mm resolution, ±(1 + 0.3% dist) cm accuracy, 3 cm blind zone)
 * - Voltage: 3.3-5V (≤8 mA active, ≤5 µA standby)
 * - Temp: -15°C to +60°C
 * - UART: 9600 baud, 8N1; Trigger with any byte (e.g., 0x55) for 4-byte response:
 *   [0xFF | High Byte | Low Byte | Checksum] → dist_mm = (High<<8 | Low); cm = /10
 *   Checksum: (0xFF + High + Low) & 0xFF
 * 
 * Pinout (4-pin connector):
 * 1. VCC → Arduino 5V
 * 2. GND → Arduino GND
 * 3. RX (sensor input/trigger) → Microcontroller (TX)
 * 4. TX (sensor output) → Microcontroller (RX)
 * 
 * Wiring Notes: Use SoftwareSerial for pins to avoid hardware UART conflict in ATmega328.
 * Trigger every 200-500 ms; validate checksum/range for reliability.
 */

/************************************************************************************************/

/*
Note: Use the below codes in the main.ino (your main project file) file.

// ============ DEBUG CONTROL ============
#define DEBUG_SENSOR       // Comment this line to disable debug output (simple debug)
#define DEBUG_SENSOR_DEEP  // Comment this line to disable debug output (deep debug)
// =======================================

#include <SoftwareSerial.h>

SoftwareSerial sensorSerial(8, 9);  // RX=8 (from sensor TX), TX=9 (to sensor RX) // default pins

// ============ CONFIGURATION THRESHOLDS ============
#define INVALID_DATA_THRESHOLD 10    // Max retries for communication errors (<4 bytes)
#define HEADER_ERROR_THRESHOLD 20    // Max retries for header errors
#define CHECKSUM_FAIL_THRESHOLD 5    // Max retries for checksum errors
#define OUT_OF_RANGE_THRESHOLD 5     // Max retries for out-of-range readings
#define UNSTABLE_READING_THRESHOLD 6 // Max retries for unstable reading between consecutive 2 readings
#define TOLERANCE_BAND_CM 3          // Max allowed difference between dual reads (cm)
// ==================================================
*/

/************************************************************************************************/

/*
 * Function: sensorRead
 * Purpose: Performs a single sensor reading cycle
 * 
 * Process:
 * 1. Flushes any stale data from serial buffer
 * 2. Sends trigger command (0x55) to sensor
 * 3. Waits for 4-byte response: [Header][DataHigh][DataLow][Checksum]
 * 4. Validates header (must be 0xFF)
 * 5. Verifies checksum
 * 6. Checks if distance is within valid range (30-4500mm)
 * 
 * Parameters:
 * - status (output): Error code or 0 for success
 * - distCm (output): Distance in centimeters (-1 if invalid)
 * 
 * Returns: void (uses reference parameters)
 */

void sensorRead(float& distCm, int& status) {
  distCm = -1;  // Default: invalid distance
  status = -1;  // Default: not attempted

#ifdef DEBUG_SENSOR
  Serial.println(F("  [sensorRead] Starting new read..."));
#endif

  // Flush any stale data from previous reads, included timeout
  int flushCount = 0;
  while (sensorSerial.available() && flushCount++ < 100) {
    sensorSerial.read();
  }

  // Send trigger command
  sensorSerial.write(0x55);

#ifdef DEBUG_SENSOR_DEEP
  Serial.println(F("  [sensorRead] Trigger sent (0x55), waiting 150ms..."));
#endif

  delay(150);  // Wait for sensor response

  // Check if we received 4 bytes
  if (sensorSerial.available() < 4) {
    status = 1;
#ifdef DEBUG_SENSOR_DEEP
    Serial.print(F("  [sensorRead] ERROR: Communication error (<4 bytes) - Only "));
    Serial.print(sensorSerial.available());
    Serial.println(F(" bytes received (need 4)"));
#endif
    return;
  }

  // Read 4-byte response packet
  byte header = sensorSerial.read();
  byte dataHigh = sensorSerial.read();
  byte dataLow = sensorSerial.read();
  byte receivedChecksum = sensorSerial.read();

#ifdef DEBUG_SENSOR_DEEP
  Serial.print(F("  [sensorRead] Raw bytes: Header=0x"));
  Serial.print(header, HEX);
  Serial.print(F(" DataH=0x"));
  Serial.print(dataHigh, HEX);
  Serial.print(F(" DataL=0x"));
  Serial.print(dataLow, HEX);
  Serial.print(F(" Checksum=0x"));
  Serial.println(receivedChecksum, HEX);
#endif

  // Validate header byte
  if (header != 0xFF) {
    status = 2;
#ifdef DEBUG_SENSOR_DEEP
    Serial.print(F("  [sensorRead] ERROR: Bad header 0x"));
    Serial.print(header, HEX);
    Serial.println(F(" (expected 0xFF)"));
#endif
    return;
  }

  // Calculate distance from high/low bytes
  int distanceMm = (dataHigh * 256) + dataLow;
  distCm = distanceMm / 10.0;

#ifdef DEBUG_SENSOR_DEEP
  Serial.print(F("  [sensorRead] Distance calculated: "));
  Serial.print(distanceMm);
  Serial.print(F("mm = "));
  Serial.print(distCm);
  Serial.println(F("cm"));
#endif

  // Verify checksum
  byte expectedChecksum = (0xFF + dataHigh + dataLow) & 0xFF;
  if (receivedChecksum != expectedChecksum) {
    status = 3;
#ifdef DEBUG_SENSOR_DEEP
    Serial.print(F("  [sensorRead] ERROR: Checksum mismatch - Got 0x"));
    Serial.print(receivedChecksum, HEX);
    Serial.print(F(", Expected 0x"));
    Serial.println(expectedChecksum, HEX);
#endif
    return;
  }

  // Check if distance is within valid range
  bool inRange = (distanceMm >= 30) && (distanceMm <= 4500);
  if (!inRange) {
    status = 4;
#ifdef DEBUG_SENSOR_DEEP
    Serial.print(F("  [sensorRead] ERROR: Out of range - "));
    Serial.print(distanceMm);
    Serial.println(F("mm (valid: 30-4500mm)"));
#endif
    return;
  }

  // Success!
  status = 0;
#ifdef DEBUG_SENSOR
  Serial.print(F("  [sensorRead] SUCCESS: "));
  Serial.print(distCm);
  Serial.print(F(" cm"));
#endif
}

/************************************************************************************************/

/*
 * Function: ultrasonicSensorCall
 * Purpose: Main sensor interface - performs dual reading with retry logic
 * 
 * Process:
 * 1. Takes two sensor readings
 * 2. For each reading, retries on errors up to threshold limits
 * 3. Compares both readings for consistency (within TOLERANCE_BAND_CM)
 * 4. Returns average distance if both readings are stable
 * 
 * Retry Limits (automatically adjusted based on error type): Listed above
 * 
 * Parameters:
 * - distanceCm (output): Final distance in cm (or -1 on failure)
 * - status (output): Final status code
 * 
 * Returns: void (uses reference parameters)
 */

void ultrasonicSensorCall(float& distanceCm, int& status) {

  float readCm[2];  // Storage for dual readings

#ifdef DEBUG_SENSOR
  Serial.println(F("\n========== ultrasonicSensorCall START =========="));
#endif

  // Retry loop for unstable readings, Dual reads differ too much > TOLERANCE_BAND_CM
  for (int h = 0; h < UNSTABLE_READING_THRESHOLD; h++) {
    // refresh the variable values to default
      status = -1;
      distanceCm = -1;

    // Perform two readings with retry logic
    for (int i = 0; i < 2; i++) {
      // refresh the variable to default
      readCm[i] = -1;

#ifdef DEBUG_SENSOR
      Serial.print(F("\n[Main] === Reading #"));
      Serial.print(i + 1);
      Serial.println(F(" ==="));
#endif

      // Only proceed if previous status was success (0) or initial state (-1)
      if (status == -1 || status == 0) {
        int loopCount = HEADER_ERROR_THRESHOLD;  // Default retry limit

        // Retry loop with adaptive threshold
        for (int j = 0; j < loopCount; j++) {
#ifdef DEBUG_SENSOR_DEEP
          Serial.print(F("[Main] Attempt "));
          Serial.print(j + 1);
          Serial.print(F(" of "));
          Serial.println(loopCount);
#endif

          // Perform single sensor read
          sensorRead(readCm[i], status);

          // Adjust retry limit based on error type
          switch (status) {
            case 0:  // Success
              {
#ifdef DEBUG_SENSOR
                Serial.print(F("[Main] Read #"));
                Serial.print(i + 1);
                Serial.print(F(" SUCCESS: "));
                Serial.print(readCm[i]);
                Serial.print(F(" cm"));
                Serial.print(F("  , with no of attempts: "));
                Serial.print(j + 1);
                Serial.print(F(" of "));
                Serial.println(loopCount);
#endif
                loopCount = -1;  // Signal to exit retry loop, no loop required
                break;
              }
            case 1:  // Communication Error (<4 bytes)
              {
                loopCount = INVALID_DATA_THRESHOLD;
                break;
              }
            case 2:  // Bad header
              {
                loopCount = HEADER_ERROR_THRESHOLD;
                break;
              }
            case 3:  // Checksum fail
              {
                loopCount = CHECKSUM_FAIL_THRESHOLD;
                break;
              }
            case 4:  // Out of range
              {
                loopCount = OUT_OF_RANGE_THRESHOLD;
                break;
              }
          }

          // Exit retry loop if successful
          if (loopCount == -1) break;
        }

        // If still failed after all retries, abort dual read
        if (status != 0) {
#ifdef DEBUG_SENSOR
          Serial.print(F("[Main] FAILED after all retries. Final status: "));
          Serial.println(status);
#endif
          break;
        }
      } else {
        break;  // Previous reading failed, abort
      }
    }

    // If either reading failed, return error
    if (status != 0) {
#ifdef DEBUG_SENSOR
      Serial.println(F("[Main] ERROR: First or second reading failed"));
      Serial.println(F("========== ultrasonicSensorCall END (FAILED) ==========\n"));
#endif
      return;
    }

    // Check if both readings are within tolerance
    float read1Cm = readCm[0];
    float read2Cm = readCm[1];
    float difference = abs(read2Cm - read1Cm);

#ifdef DEBUG_SENSOR_DEEP
    Serial.print(F("\n[Main] Comparing dual reads: "));
    Serial.print(readCm[0]);
    Serial.print(F(" cm vs "));
    Serial.print(readCm[1]);
    Serial.print(F(" cm | Diff: "));
    Serial.print(difference);
    Serial.print(F(" cm (tolerance: "));
    Serial.print(TOLERANCE_BAND_CM);
    Serial.println(F(" cm)"));
#endif

    if (difference > TOLERANCE_BAND_CM) {
      status = 5;
#ifdef DEBUG_SENSOR
      Serial.println(F("[Main] ERROR: Readings differ too much (unstable)"));
      if (h >= UNSTABLE_READING_THRESHOLD) Serial.println(F("========== ultrasonicSensorCall END (UNSTABLE) ==========\n"));
#endif
    } else {
      // Calculate average distance
      distanceCm = (read1Cm + read2Cm) / 2.0;

#ifdef DEBUG_SENSOR
      Serial.print(F("[Main] SUCCESS: Average distance = "));
      Serial.print(distanceCm);
      Serial.println(F(" cm"));
      Serial.println(F("========== ultrasonicSensorCall END (SUCCESS) ==========\n"));
#endif
      break;
    }
  }
}

// End of file ultrasonic_Sensor_Call.ino