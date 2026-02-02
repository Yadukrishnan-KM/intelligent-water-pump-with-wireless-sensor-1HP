/*
 * level_Setup_UI.ino
 * Initial Tank Level Calibration Setup Function (ATmega328PB/Arduino)
 *
 * Author: Yadukrishnan K M
 * Last Updated: December 11, 2025
 * Platform: Arduino IDE 2.3.6, ATmega328PB MCU
 * Description: Startup calibration routine for ultrasonic tank level sensor.
 *              Guides user through low (empty) and high (full) level calibration
 *              using single button and LED interface. Saves values to EEPROM
 *              for percentage calculation.
 *
 * Usage:
 *   - Runs once on startup (call in setup())
 *   - Debug prints optional via #define DEBUG_LEVEL_SETUP
 *   - Required global variables (declare in main.ino):
 *     const int ledPin;           // LED indicator pin
 *     const int inputSetPin;      // Calibration button pin
 *     unsigned long timeOut;      // Timeout for entry (ms)             
 *   - Dependencies: EEPROM.h, sensorRead() function
 *
 * User Flow:
 *   1. LED blinks twice and stays on → Waiting for button press to enter setup mode
 *   2. Press button within timeout (one blink and stay on) → Enter calibration
 *      Not pressed → LED off and exit setup mode
 *   3. Short press → Low tank level menu (LED goes off) → Hold button → 
 *      Saves low value (LED blinks 4x for success)
 *   4. Short press → High tank level menu (LED goes off) → Hold button → 
 *      Saves high value (LED blinks 4x for success)
 *   5. Timeout on initial wait → LED off, skip calibration
 */

// ============ DEBUG CONTROL ============
// #define DEBUG_LEVEL_SETUP  // Comment to disable setup debug prints
// =======================================

// Helper function for software-debounced button reading
bool readDebouncedButton() {
  bool firstRead = digitalRead(inputSetPin);
  delay(50);  // Standard debounce delay
  bool secondRead = digitalRead(inputSetPin);
  // Return button state only if both reads match (stable)
  return (firstRead == secondRead) ? secondRead : false;
}

void initialSetup() {
  bool input;                                   // Button state flag
  float depth;                                  // Ultrasonic reading variable (cm)
  int status;                                   // Sensor read status, 0 - success without any error
  unsigned long previousMillis, currentMillis;  // Timing variables
  // Initialize with LED blink pattern (setup mode entry)
  digitalWrite(ledPin, LOW);
  delay(100);
  digitalWrite(ledPin, HIGH);
  delay(500);
  digitalWrite(ledPin, LOW);
  delay(100);
  digitalWrite(ledPin, HIGH);

  // Start timeout tracking
  previousMillis = millis();
  currentMillis = millis();

  // Wait for button press or timeout (setup mode entry)
  input = false;  // Reset input state
  while ((!input) && ((currentMillis - previousMillis) <= INITIAL_SETUP_TIMEOUT)) {
    currentMillis = millis();
    input = readDebouncedButton();  // Debounced button read

#ifdef DEBUG_LEVEL_SETUP
    Serial.println(F("[initialSetup] Waiting for button press or timeout"));
#endif
  }

  // If button pressed, enter calibration mode
  if (input) {
    // Enter low level setting menu (one blink feedback)
    digitalWrite(ledPin, LOW);
    delay(500);
    digitalWrite(ledPin, HIGH); // low level menu

    // Wait for button release
    while (input) {
      input = readDebouncedButton();  // Debounced button read
#ifdef DEBUG_LEVEL_SETUP
      Serial.println(F("Please release button"));
#endif
    }

    // Wait for button press to enter low level setting
    while (!input) {
      input = readDebouncedButton();  // Debounced button read

#ifdef DEBUG_LEVEL_SETUP
      Serial.println(F("[initialSetup] Waiting for low level menu entry"));
#endif
    }

    // Visual feedback: LED off for low level capture phase
    digitalWrite(ledPin, LOW);
    delay(500);

    // Wait for button release
    while (input) {
      input = readDebouncedButton();  // Debounced button read
#ifdef DEBUG_LEVEL_SETUP
      Serial.println(F("Please release button"));
#endif
    }

    // Wait for button hold to capture low level
    while (!input) {
      input = readDebouncedButton();  // Debounced button read

#ifdef DEBUG_LEVEL_SETUP
      Serial.println(F("[initialSetup] Hold button to set low level"));
#endif

      // If button is pressed, check if it's held (intentional press)
      if (input) {
        delay(300);                     // Hold delay for intentional press
        input = readDebouncedButton();  // Re-check after hold period
      }
    }

    // Capture and save low level to EEPROM

    ultrasonicSensorCall(depth, status);
    EEPROM.put(0, depth);

#ifdef DEBUG_LEVEL_SETUP
    Serial.print(F("[initialSetup] Low level saved: "));
    Serial.print(depth);
    Serial.println(F("cm"));
    Serial.print(F("Status code:"));
    Serial.print(status);
    Serial.println(F(" , Status code 0 means success"));
#endif

    // Success feedback: 4 quick blinks
    for (int i = 0; i <= 3; i++) {
      digitalWrite(ledPin, HIGH);
      delay(100);
      digitalWrite(ledPin, LOW);
      delay(100);
    }
    digitalWrite(ledPin, HIGH);
    delay(500);

    // Wait for button release
    while (input) {
      input = readDebouncedButton();  // Debounced button read
#ifdef DEBUG_LEVEL_SETUP
      Serial.println(F("Please release button"));
#endif
    }

    // Wait for button press to enter high level setting
    while (!input) {
      input = readDebouncedButton();  // Debounced button read

#ifdef DEBUG_LEVEL_SETUP
      Serial.println(F("[initialSetup] Waiting for high level menu entry"));
#endif
    }

    // Visual feedback: LED off for high level capture phase
    digitalWrite(ledPin, LOW);
    delay(500);

    // Wait for button release
    while (input) {
      input = readDebouncedButton();  // Debounced button read
#ifdef DEBUG_LEVEL_SETUP
      Serial.println(F("Please release button"));
#endif
    }

    // Wait for button hold to capture high level
    while (!input) {
      input = readDebouncedButton();  // Debounced button read

#ifdef DEBUG_LEVEL_SETUP
      Serial.println(F("[initialSetup] Hold button to set high level"));
#endif

      // If button is pressed, check if it's held (intentional press)
      if (input) {
        delay(300);                     // Hold delay for intentional press
        input = readDebouncedButton();  // Re-check after hold period
      }
    }

    // Capture and save high level to EEPROM
    ultrasonicSensorCall(depth, status);
    EEPROM.put(4, depth);

#ifdef DEBUG_LEVEL_SETUP
    Serial.print(F("[initialSetup] High level saved: "));
    Serial.print(depth);
    Serial.println(F("cm"));
    Serial.print(F("Status code:"));
    Serial.print(status);
    Serial.println(F(" , Status code 0 means success"));
#endif

    // Success feedback: 4 quick blinks
    for (int i = 0; i <= 3; i++) {
      digitalWrite(ledPin, HIGH);
      delay(100);
      digitalWrite(ledPin, LOW);
      delay(100);
    }

#ifdef DEBUG_LEVEL_SETUP
    Serial.println(F("[initialSetup] Successfully completed"));
#endif
  } else {
    // Timeout occurred, turn LED off and exit
    digitalWrite(ledPin, LOW);

#ifdef DEBUG_LEVEL_SETUP
    Serial.println(F("[initialSetup] Timeout - Calibration skipped"));
#endif
  }
}

// End of file "level_Setup_UI.ino"