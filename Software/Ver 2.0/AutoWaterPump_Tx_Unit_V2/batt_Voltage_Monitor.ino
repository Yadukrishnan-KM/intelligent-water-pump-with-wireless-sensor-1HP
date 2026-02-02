/*
 * batt_Voltage_Monitor.ino
 *
 * Battery Voltage Monitoring Function for ATmega328PB (Arduino Uno/Nano)
 *
 * Author: Yadukrishnan K M
 * Last Updated: December 11, 2025
 * Platform: Arduino IDE 2.3.6, ATmega328PB MCU
 * Description: Measures supply voltage (AVCC) using internal 1.1V bandgap reference
 *              via 10-bit ADC. Ideal for battery-powered projects to monitor low voltage.
 *
 * Key Features:
 * - Averages 4 samples for noise reduction.
 * - Returns voltage in mV (e.g., 5000 for 5V).
 * - Debug prints optional via #define DEBUG_BAT_MONITOR .
 * - Assumes ADC enabled/prescaled in setup() (see notes).
 *
 * Usage:
 *   long vcc = readVcc();
 *
 * Status Codes/Returns:
 * - Valid: 3000-5500 mV typical.
 * - -1: ADC error (division by zero or invalid read).
 *
 * Wiring/Setup Notes:
 * - No external pins needed (internal bandgap channel 14).
 * - In setup(): ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // Enable ADC, /128 prescaler (125kHz @ 16MHz clock)
 *             : ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1); // For 8MHz internal clock,  prescaler 64 (125kHz @ 8 MHz clock)
 * - Calibrate constant (1126400L) if bandgap drifts (±10 mV factory trim).
 * - For production: Call periodically (e.g., every 10s) to save power.
 *
 * Dependencies: None (uses AVR registers directly).
 * Limitations: ADC clock <200 kHz for accuracy; noisy environments need more averaging.
 *
 */

// ============ DEBUG CONTROL ============
// #define DEBUG_BAT_MONITOR   // Comment to disable voltage debug prints
// =======================================

long readVcc() {
#ifdef DEBUG_BAT_MONITOR 
  Serial.println(F("Checking battery voltage..."));
#endif
  
  // Assume ADCSRA configured in setup: ADEN=1, prescaler=/128
  
  long result = 0;
  const int samples = 4;  // Average for noise reduction
  
  for (int i = 0; i < samples; i++) {
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);  // Internal 1.1V ref, bandgap input (channel 14)
    delay(2);  // Settle Vref
    
    ADCSRA |= _BV(ADSC);  // Start conversion
    while (bit_is_set(ADCSRA, ADSC));  // Wait complete
    
    result += ADCL | (ADCH << 8);  // 10-bit result
  }
  
  result /= samples;  // Average
  
  if (result == 0) {
#ifdef DEBUG_BAT_MONITOR 
    Serial.println(F("VCC Read Error: Invalid ADC value"));
#endif
    return -1;  // Error
  }
  
  long vcc_mV = 1126400L / result;  // AVCC in mV
  
#ifdef DEBUG_BAT_MONITOR 
  Serial.print(F("Battery Voltage: "));
  Serial.print(vcc_mV);
  Serial.println(F(" mV"));
#endif
  
  return vcc_mV;
}

// End of file "batt_Voltage_Monitor.ino"