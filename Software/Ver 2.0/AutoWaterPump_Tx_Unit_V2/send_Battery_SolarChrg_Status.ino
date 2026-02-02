/*
 * send_Battery_SolarChrg_Status.ino
 * Battery Status Transmission Function for ATmega328PB (Arduino)
 *
 * Author: Yadukrishnan K M, Jahnavi K
 * Last Updated: December 11, 2025
 * Platform: Arduino IDE 2.3.6, ATmega328PB MCU
 * Description: Monitors battery voltage and solar charging status, transmits
 *              via RF every 10 minutes. Uses non-blocking millis() timing to
 *              avoid blocking main program execution.
 *
 * Key Features:
 * - Reads battery voltage using internal bandgap reference
 * - Monitors solar charging status via analog pin
 * - Transmits status codes via RF transmission
 * - Executes only once every 10 minutes (600,000 ms)
 * - Non-blocking implementation using millis()
 *
 * Usage:
 *   Call sendBatteryStatusPeriodically() in main loop()
 *
 * Required Global Variables (declare in main.ino):
 *   bool d[4];                    // RF data bits array
 *   const int solarChargePin;     // Analog pin for charging detection
 *   long readVcc();               // Battery voltage reading function
 *   void rfTransmission();        // RF transmission function
 *
 * Transmission Codes:
 *   Battery Low (<3.4V):   1100 (d[3]=1, d[2]=1, d[1]=0, d[0]=0)
 *   Battery Good:          1101 (d[3]=1, d[2]=1, d[1]=0, d[0]=1)
 *   Charging:              1110 (d[3]=1, d[2]=1, d[1]=1, d[0]=0)
 *   Not Charging:          1011 (d[3]=1, d[2]=0, d[1]=1, d[0]=1)
 *
 * Dependencies: readVcc(), rfTransmission()
 * Timing: Transmits every 9 sec of operating time, 10ms delay between packets
 */

// ============ DEBUG CONTROL ============
// #define DEBUG_BAT_SOLAR   // Comment to disable battery debug prints
// =======================================

/**
 * Sends battery and charging status via RF every 10 minutes
 * Uses non-blocking millis() timing, preserves state between calls
 */
void sendBatterySolarChrgStatusPeriodically() {
  // Static variables preserve state between function calls
  static unsigned long previousMillis = 0;
  static bool firstRun = true;
  
  // 9 seconds in milliseconds, normal sensor read time: 1022 ms, each sleep 10 ms, battery and solar read time: 370 ms
  // if 8 sec sleep going on then it will update aprox. every 1 min, if 5 min sleep going on it will update aprox. every 30 min.wtrLvlPercent
  const unsigned long transmitInterval = 9000UL; // The millis() only will count the active time not sleep time, so need ot keep small time 9sec
  
  // Get current time
  unsigned long currentMillis = millis();
  
  // Check if 10 minutes have passed or this is first run
  if (firstRun || (currentMillis - previousMillis >= transmitInterval)) {
    // Update timing for next transmission
    previousMillis = currentMillis;
    firstRun = false;
    
    // Battery voltage reading and transmission
    int batV = readVcc();  // Reading battery voltage in mV
    
#ifdef DEBUG_BAT_SOLAR 
    Serial.print(F("[Battery] Voltage: "));
    Serial.print(batV);
    Serial.println(F(" mV"));
#endif
    
    // Set RF data bits for battery status
    if (batV <= 3400) {  // Low battery if less than 3.4V, it may work upto 2 hr after warning
      d[3] = 1; d[2] = 1; d[1] = 0; d[0] = 0;  // Low battery code
    } else {
      d[3] = 1; d[2] = 1; d[1] = 0; d[0] = 1;  // Battery good code
    }
    
    // Transmit battery status
    rfTransmission();
    delay(10);  // Delay between transmissions
    
    // Solar charging status reading and transmission
    int chargingVD = analogRead(solarChargePin);  // Reading 0-1023
    
#ifdef DEBUG_BAT_SOLAR 
    Serial.print(F("[Battery] Charging pin: "));
    Serial.println(chargingVD);
#endif
    
    // Set RF data bits for charging status
    if (chargingVD <= 512) {  // Less than half, charging
      d[3] = 1; d[2] = 1; d[1] = 1; d[0] = 0;  // Charging code
    } else {
      d[3] = 1; d[2] = 0; d[1] = 1; d[0] = 1;  // Not charging code
    }
    
    // Transmit charging status
    rfTransmission();
    delay(10);  // Delay between transmissions
    
#ifdef DEBUG_BAT_SOLAR 
    Serial.println(F("[Battery] Status transmitted"));
#endif
  }
}

// End of file "send_Battery_SolarChrg_Status.ino"