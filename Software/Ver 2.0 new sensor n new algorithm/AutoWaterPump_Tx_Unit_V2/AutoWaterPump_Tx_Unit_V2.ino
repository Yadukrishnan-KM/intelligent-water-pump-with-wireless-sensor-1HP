
/*
 * AutoWaterPump_Tx_Unit.ino
 * Main Program for Smart Auto Water Pump Transmitter (ATmega328PB/Arduino)
 *
 * Author: Yadukrishnan K M, Jahnavi K
 * Last Updated: 16/dec/2025
 * Code version : V2.0
 * Code status : Tested, working
 *
 * Platform: MiniCore ATmega328 (https://github.com/MCUdude/MiniCore), Internal 8 MHz
 * Description: Battery-powered water level monitoring and pump control transmitter.
 *              Reads ultrasonic sensor, transmits water level via RF, implements
 *              adaptive sleep algorithm for battery optimization.
 *
 * Hardware Setup:
 *   - ATmega328P @ 8MHz internal clock
 *   - Ultrasonic sensor via SoftwareSerial
 *   - RF transmitter with HT12E encoder
 *   - Programmed via USB ASP in ISP mode (with or without crystal on the board ISP will work)
 *
 * Dependencies:
 *   EEPROM.h, LowPower.h, SoftwareSerial.h
 */

#include <EEPROM.h>
#include "LowPower.h"  // https://github.com/LowPowerLab/LowPower , version used: 2.2
#include <SoftwareSerial.h>

// ============ DEBUG DEFINITIONS ============

#define DEBUG_SETUP              // Comment this line to disable debug output for main.ino, void setup
//#define DEBUG_LOOP               // Comment this line to disable debug output for main.ino, void loop
//#define DEBUG_LOOP_TIME_ELAPSED  // Comment this line to disable debug output for main.ino, for calculating loop one time execution time
/*
#define DEBUG_SENSOR       // Comment this line to disable debug output for Sensor (simple debug)
#define DEBUG_SENSOR_DEEP  // Comment this line to disable debug output for Sensor (deep debug)
#define DEBUG_BAT_MONITOR  // Comment to disable battery voltage monitor debug prints
#define DEBUG_BAT_SOLAR    // Comment to disable battery charging debug prints
#define DEBUG_RESET        // Comment to disable reset debug prints
#define DEBUG_RF           // Comment to disable RF transmission debug prints
#define DEBUG_RF_DEEP      // Comment for detailed RF transmission debug prints
#define DEBUG_LEVEL_SETUP  // Comment to disable level setup debug prints
#define DEBUG_SLEEP        // Comment to disable dynamic sleep period debug prints
*/
// ============ SENSOR CONFIGURATION ============
#define INVALID_DATA_THRESHOLD 10     // Max retries for communication errors (<4 bytes)
#define HEADER_ERROR_THRESHOLD 20     // Max retries for header errors
#define CHECKSUM_FAIL_THRESHOLD 5     // Max retries for checksum errors
#define OUT_OF_RANGE_THRESHOLD 5      // Max retries for out-of-range readings
#define UNSTABLE_READING_THRESHOLD 6  // Max retries for unstable reading
#define TOLERANCE_BAND_CM 3.0         // Max allowed difference between dual reads (cm)

// ============ DYNAMIC SLEEP CONFIGURATION ============
#define MOV_AVG_BUFFER_TOLERANCE 2.0  // Tolerance for moving average (cm)

// ============ SYSTEM TIMING ============
// 24 hours reset, based on system active time (sleeping time not inculded, millis is off during sleep) need to rest evey 448sec, in ms: 448000 
#define RESET_PERIOD_MS 448000UL    
#define INITIAL_SETUP_TIMEOUT 5000UL  // 5 seconds timeout for initial setup

// ============ PIN DEFINITIONS (Atmega328p) ============
const int ultrasonicTrigPin = 3;
const int ultrasonicechoPin = 4;
const int sensorPwrPin = A2;
const int rfTxPwrPin = A3;
const int txEnablePin = 5;
const int d0Pin = 6;
const int d1Pin = 7;
const int d2Pin = 8;
const int d3Pin = 9;
const int inputSetPin = 2;
const int ledPin = A1;
const int solarChargePin = A0;
const int resetPin = 10;

// ============ GLOBAL VARIABLES ============
bool d[4];                 // RF transmission data bits
float depth;               // Current ultrasonic reading (cm)
int status;                // Sensor read status, 0 - success without any error
int wtrLvlPercent;         // Water level percentage (0-100)
int sleepPeriod = 3;       // Current sleep period in 8-second cycles, default - 37 * 8 ≈ 300 sec (5min)
float minLevel, maxLevel;  // Calibrated tank levels from EEPROM

SoftwareSerial sensorSerial(ultrasonicechoPin, ultrasonicTrigPin);  // RX (from sensor TX), TX (to sensor RX)

void setup() {
  // Pinmode setup for all I/O pins
  digitalWrite(resetPin, HIGH);  // Reset pin initially HIGH to prevent accidental reset (requires external pull-up resistor)
  pinMode(sensorPwrPin, OUTPUT);
  pinMode(rfTxPwrPin, OUTPUT);
  pinMode(txEnablePin, OUTPUT);
  pinMode(d0Pin, OUTPUT);
  pinMode(d1Pin, OUTPUT);
  pinMode(d2Pin, OUTPUT);
  pinMode(d3Pin, OUTPUT);
  pinMode(inputSetPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(solarChargePin, INPUT_PULLUP);
  pinMode(resetPin, OUTPUT);

  // Initialize all pins to known safe states
  digitalWrite(resetPin, HIGH);     // Maintain HIGH state to prevent MCU reset
  digitalWrite(sensorPwrPin, LOW);  // Sensor power OFF initially
  digitalWrite(rfTxPwrPin, LOW);    // RF transmitter power OFF
  digitalWrite(txEnablePin, LOW);   // Transmit enable OFF
  digitalWrite(d0Pin, LOW);         // Data pins LOW
  digitalWrite(d1Pin, LOW);
  digitalWrite(d2Pin, LOW);
  digitalWrite(d3Pin, LOW);
  digitalWrite(ledPin, LOW);  // LED OFF

  // Configure ADC for battery voltage reading
  // Enable ADC with 128 prescaler (125kHz @ 16MHz clock)
  //ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
  // For 8MHz internal clock,  prescaler 64 (125kHz @ 8 MHz clock)
  ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1);

  // Initialize serial communication
  Serial.begin(9600);        // Hardware serial for debugging
  sensorSerial.begin(9600);  // Software serial for ultrasonic sensor
  delay(500);                // Allow 500ms for serial port stabilization

#ifdef DEBUG_SETUP
  Serial.println(F("[SETUP] System starting initialization..."));
#endif

  // Power up sensor and perform initial calibration setup
  digitalWrite(sensorPwrPin, HIGH);  // Enable sensor power
  delay(500);                        // Allow 500ms for sensor startup stabilization

  initialSetup();  // Run tank level calibration procedure

  digitalWrite(sensorPwrPin, LOW);  // Disable sensor to save power

  // Read calibrated tank levels from EEPROM
  EEPROM.get(0, minLevel);  // Low (empty) level stored at address 0
  EEPROM.get(4, maxLevel);  // High (full) level stored at address 4

#ifdef DEBUG_SETUP
  Serial.print(F("[SETUP] EEPROM - Minimum level: "));
  Serial.print(minLevel);
  Serial.println(F(" cm"));

  Serial.print(F("[SETUP] EEPROM - Maximum level: "));
  Serial.print(maxLevel);
  Serial.println(F(" cm"));

  if (maxLevel >= minLevel) {
    Serial.println(F("[SETUP] Wrong Levels (1st should be Tank Low, 2nd should be Tank full), Repeat setup"));
  }
#endif
}


void loop() {
  // For calculating loop one time execution time
#ifdef DEBUG_LOOP_TIME_ELAPSED
  unsigned long startTime = millis();  // Or micros() for µs
#endif

#ifdef DEBUG_LOOP
  Serial.println(F("\n\n[LOOP] Waking up after sleep"));
#endif

  // Confirm sensor reading validity: check if reading is within calibrated tank range
  // 3-attempt loop handles temporary obstacles or measurement errors
  bool calLvlOutofRange = true;
  for (int i = 0; i <= 2; i++) {
    // Power up sensor and perform measurement
    digitalWrite(sensorPwrPin, HIGH);     // Enable sensor power
    ultrasonicSensorCall(depth, status);  // Get depth and status
    digitalWrite(sensorPwrPin, LOW);      // Disable sensor to save power

    // Check if reading is within reasonable tank bounds
    // Allow 10cm below maxLevel and 15cm above minLevel for tolerance
    if ((depth >= (maxLevel - 10.0)) && (depth <= (minLevel + 15.0))) {
      calLvlOutofRange = false;
      break;  // Valid reading found, exit loop
    }
  }

#ifdef DEBUG_LOOP
  Serial.print(F("[LOOP] Depth: "));
  Serial.print(depth);
  Serial.print(F(" cm, Status: "));
  Serial.print(status);
  Serial.print(F(", Out of range: "));
  Serial.println(calLvlOutofRange ? "YES" : "NO");
#endif

  // Process water level if the obtained reading is valid
  if (!calLvlOutofRange && status == 0) {
    // Convert depth to percentage (1-100%)
    // Using map with rounded values to avoid floating-point issues
    wtrLvlPercent = constrain(map(round(depth), round(minLevel), round(maxLevel), 1, 100), 1, 100);

#ifdef DEBUG_LOOP
    Serial.print(F("[LOOP] Percentage: "));
    Serial.print(wtrLvlPercent);
    Serial.println(F("%"));
#endif

    // Encode water level percentage into 4-bit RF data
    // Each range corresponds to a specific 4-bit pattern
    if (wtrLvlPercent <= 14) {
      d[3] = 0;
      d[2] = 0;
      d[1] = 0;
      d[0] = 1;  // 1: 0-14%
    } else if (wtrLvlPercent <= 25) {
      d[3] = 0;
      d[2] = 0;
      d[1] = 1;
      d[0] = 0;  // 2: 15-25%
    } else if (wtrLvlPercent <= 35) {
      d[3] = 0;
      d[2] = 0;
      d[1] = 1;
      d[0] = 1;  // 3: 26-35%
    } else if (wtrLvlPercent <= 45) {
      d[3] = 0;
      d[2] = 1;
      d[1] = 0;
      d[0] = 0;  // 4: 36-45%
    } else if (wtrLvlPercent <= 55) {
      d[3] = 0;
      d[2] = 1;
      d[1] = 0;
      d[0] = 1;  // 5: 46-55%
    } else if (wtrLvlPercent <= 65) {
      d[3] = 0;
      d[2] = 1;
      d[1] = 1;
      d[0] = 0;  // 6: 56-65%
    } else if (wtrLvlPercent <= 75) {
      d[3] = 0;
      d[2] = 1;
      d[1] = 1;
      d[0] = 1;  // 7: 66-75%
    } else if (wtrLvlPercent <= 85) {
      d[3] = 1;
      d[2] = 0;
      d[1] = 0;
      d[0] = 0;  // 8: 76-85%
    } else if (wtrLvlPercent <= 97) {
      d[3] = 1;
      d[2] = 0;
      d[1] = 0;
      d[0] = 1;  // 9: 86-97%
    } else if (wtrLvlPercent > 97) {
      d[3] = 1;
      d[2] = 0;
      d[1] = 1;
      d[0] = 0;  // 10: 98-100%
    }

    // Transmit water level data
    rfTransmission();
    delay(10);

    // Send zero data (clear transmission)
    d[3] = 0;
    d[2] = 0;
    d[1] = 0;
    d[0] = 0;
    rfTransmission();
    delay(10);

    // Send battery status periodically
    sendBatterySolarChrgStatusPeriodically();

    // Calculate adaptive sleep period based on depth and water level
    sleepPeriod = dynamicSleepPeriod(depth, wtrLvlPercent);

#ifdef DEBUG_LOOP
    Serial.print(F("[LOOP] Success, Sleep period: "));
    Serial.print(sleepPeriod);
    Serial.println(F(" cycles (8 sec each)"));
#endif
  } else {
    sleepPeriod = 3;  // Because of error state, increasing sleep period - 37 * 8 ≈ 300 sec (5min) (8-second cycles)
  }

  // Handle error conditions: sensor out of range or measurement errors
  if (calLvlOutofRange || status == 4 || status == 5) {
    // Send error code (1111 binary = 15 decimal)
    d[3] = 1;
    d[2] = 1;
    d[1] = 1;
    d[0] = 1;
    rfTransmission();
    delay(10);
    sendBatterySolarChrgStatusPeriodically();

#ifdef DEBUG_LOOP
    Serial.println(F("[ERROR] Invalid data, send error message to RX"));
#endif
  }

#ifdef DEBUG_LOOP
  // Log sensor failures (status 1, 2, or 3) but don't transmit
  if (status == 1 || status == 2 || status == 3) {
    Serial.print(F("[SENSOR FAILURE] Status: "));
    Serial.println(status);
    Serial.println(F("No message sent to RX"));
  }
#endif

  // Sleep for calculated period (each cycle = 8 seconds)
#ifdef DEBUG_LOOP
  Serial.print(F("[LOOP] Entering sleep for "));
  Serial.print(sleepPeriod);
  Serial.println(F(" cycles"));
#endif

// For calculating loop one time execution time
#ifdef DEBUG_LOOP_TIME_ELAPSED
unsigned long endTimeBeforeSleep = millis();  // Or micros()
#endif

  for (int i = 0; i < sleepPeriod; i++) {
#ifdef DEBUG_LOOP
    Serial.println(F("[LOOP] Going for sleep (8 sec)"));
    delay(100);  // Brief delay for UART communication finish
#endif
    delay(10);  // Brief delay before sleep
    //LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_ON);  // 8-second sleep
  }

  // Check if reset timeout has been reached
  resetCheck();

  // For calculating loop one time execution time
#ifdef DEBUG_LOOP_TIME_ELAPSED
  unsigned long endTimeAfterSleep = millis();  // Or micros()
  unsigned long elapsedBeforeSleep = endTimeBeforeSleep - startTime;
  unsigned long elapsedAfterSleep = endTimeAfterSleep - startTime;

  Serial.println(F("-----------------------------------\n"));
  Serial.print(" | Execution Time Before Sleep: ");
  Serial.print(elapsedBeforeSleep);
  Serial.print(" ms");  // Change to " µs" if using micros()
  Serial.print(", Execution Time After Sleep: ");
  Serial.print(elapsedAfterSleep);
  Serial.println(" ms");  // Change to " µs" if using micros()
  Serial.println(F("-----------------------------------\n"));
#endif
}

// End of file "AutoWaterPump_Tx_Unit.ino"