/*
 * rf_Transmission.ino
 * RF Transmission Function for HT12E Encoder (ATmega328PB/Arduino)
 *
 * Author: Yadukrishnan K M
 * Last Updated: December 11, 2025
 * Platform: Arduino IDE 2.3.6, ATmega328PB MCU
 * Description: Transmits 4-bit data via HT12E RF encoder IC. Inverts bits for NPN transistor
 *              at output stage. Powers RF module, latches data, pulses transmit enable (TE)
 *              twice for redundancy (2 identical streams). Disables after.
 *
 * Usage:
 *   Debug prints optional via #define DEBUG_RF (basic), DEBUG_RF_DEEP (detailed).
 *   In main.ino: Declare pins and set data
 *     // Configuration (user-editable)
 *     const int rfTxPwrPin = A3; // RF module power enable
 *     const int txEnablePin = 5; // HT12E Transmit Enable (TE) pin
 *     const int d0Pin = 6;       // Data bit 0 (extend for your pins)
 *     const int d1Pin = 7;
 *     const int d2Pin = 8;
 *     const int d3Pin = 9;
 *     bool d[4];                 // Global data array (set before call)
 *   Call: rfTransmission();     // In loop() or on event
 *
 * Dependencies: None (uses digitalWrite, delay, Serial).
 */

// ============ DEBUG CONTROL ============
// #define DEBUG_RF       // Comment to disable RF debug prints
// #define DEBUG_RF_DEEP  // Uncomment for detailed debug prints
// =======================================

void rfTransmission() {
#ifdef DEBUG_RF
  Serial.println(F("[rfTransmission] Starting RF transmit..."));
#endif


  // Complement due to NPN transistor at output stage; inverts back for correct logic
  digitalWrite(d0Pin, !d[0]);
  digitalWrite(d1Pin, !d[1]);
  digitalWrite(d2Pin, !d[2]);
  digitalWrite(d3Pin, !d[3]);

#ifdef DEBUG_RF_DEEP
  Serial.print(F("[rfTransmission] Data latched: "));
  for (int bit = 0; bit < 4; bit++) {
    Serial.print(d[bit] ? "1" : "0");
  }
  Serial.println(F(" (inverted on pins)"));
#endif

  // Enable RF power 
  digitalWrite(rfTxPwrPin, HIGH);
#ifdef DEBUG_RF
  Serial.println(F("[rfTransmission] RF power enabled"));
#endif
  delay(50);  // Stabilize power/encoder

  // Transmit loop: Pulse TE for each stream (2x redundancy)
  for (int i = 0; i < 2; i++) {
#ifdef DEBUG_RF_DEEP
    Serial.print(F("[rfTransmission] Sending stream #"));
    Serial.print(i + 1);
    Serial.print(F("/"));
    Serial.println(2);
#endif

    digitalWrite(txEnablePin, HIGH);  // Latch & transmit
    delay(50);

    digitalWrite(txEnablePin, LOW);   // End transmit
    delay(100);
  }

  // Final disable 
  digitalWrite(txEnablePin, LOW);
#ifdef DEBUG_RF_DEEP
  Serial.println(F("[rfTransmission] RF transmission disabled"));
#endif

  digitalWrite(rfTxPwrPin, LOW);  // Power off to save energy
#ifdef DEBUG_RF
  Serial.println(F("[rfTransmission] RF power disabled - Transmission completed"));
#endif
}

// End of file "rfTransmission.ino"