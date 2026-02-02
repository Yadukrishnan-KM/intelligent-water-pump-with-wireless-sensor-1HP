/*
 * reset.ino
 *
 * Software Reset Function for ATmega328PB (Arduino Uno/Nano) to reset after timeout to clean the system and ensure reliable operation
 *
 * Author: Yadukrishnan K M
 * Last Updated: December 11, 2025
 * Platform: Arduino IDE 2.3.6, ATmega328PB MCU
 * Description: Monitors MCU uptime and triggers hardware after a configurable period and reset
 *              by pulling a digital pin LOW (connected to RESET pin via 1kΩ series resistor).
 *
 * Key Features:
 * - Debug prints optional via #define DEBUG_RESET.
 * - Safe: Keeps pin HIGH normally; pulls LOW only on timeout.
 *
 * Usage:
 *   Definitions: #define RESET_PERIOD_MS 448000UL (24 hours, active time of 448 sec (sleeping time not inculded, millis is off during sleep)
 *   Declaration: const int resetPin = ; (Digital pin connected to RESET via 1kΩ resistor)
 *   In setup(): pinMode(resetPin, OUTPUT); digitalWrite(resetPin, HIGH);
 *   In loop(): reset();  // Call every cycle
 *   Customize: Edit RESET_PERIOD_MS macro.
 *
 * Dependencies: None (uses millis(), digitalWrite).
 *
 */

// ============ DEBUG CONTROL ============
// #define DEBUG_RESET  // Comment to disable reset debug prints
// =======================================
void resetCheck() {
  unsigned long currentMillis = millis();
  
  // Reset only if water level is less (to avoid reset while tank is filling to full) and reached time out
  if (currentMillis >= RESET_PERIOD_MS && wtrLvlPercent <= 75) {
#ifdef DEBUG_RESET
    Serial.println(F("[resetCheck] Uptime timeout reached - Triggering MCU Reset"));
#endif
    delay(100);
    digitalWrite(resetPin, LOW);  // Pull LOW: Reset triggered
    delay(20);
  }
}
// End of file "reset.ino"