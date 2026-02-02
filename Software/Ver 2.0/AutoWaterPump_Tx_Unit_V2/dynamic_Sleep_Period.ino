/*
 * dynamicSleepPeriod.ino
 * Dynamic Sleep Period Calculator for Tank Monitoring (ATmega328PB/Arduino)
 *
 * Author: Yadukrishnan K M, Jahnavi K
 * Last Updated: December 11, 2025
 * Platform: Arduino IDE 2.3.6, ATmega328PB MCU
 * Description: Calculates adaptive sleep period based on water level trend and tank percentage.
 *              Uses 4-sample moving average (5-minute spaced) to detect filling events.
 *              Implements exponential sampling during filling to prevent overflow while
 *              maximizing battery life during discharge periods.
 *
 * Usage:
 *   - Call after ultrasonic sensor reading in main loop
 *   - Requires #define MOV_AVG_BUFFER_TOLERANCE 2.0 in main.ino
 *   - Sleep period units: multiples of 8 seconds (for LowPower.powerDown(SLEEP_8S))
 *
 * Algorithm:
 *   1. Track elapsed time, update buffer every ~5 min (296 seconds)
 *   2. Compare current depth with 4-sample moving average
 *   3. If (avg - depth) ≥ tolerance → Filling trend detected
 *   4. Adaptive sleep based on trend + water level percentage
 *      - Discharge: 37 sleeps (296 sec ≈ 5 min)
 *      - Filling: 1-15 sleeps (8-120 sec) based on tank level
 */

// ============ DEBUG CONTROL ============
// #define DEBUG_SLEEP  // Comment to disable debug prints
// =======================================

/**
 * Calculates adaptive sleep period based on water level trend and tank fill percentage
 * 
 * @param depth Current ultrasonic reading in cm (distance to the water from sensor)
 * @param wtrLvlPercent Current water level percentage (0-100%)
 * @return Sleep per, Avg: 57.40 cmiod count (each unit = 8 seconds sleep)
 */
int dynamicSleepPeriod(float depth, int wtrLvlPercent) {
  // Persistent variables (retain values between function calls)
  static float MovAvgBuffer[4] = { 0.0, 0.0, 0.0, 0.0 };
  static unsigned int totalElapsedTime = 0;
  static float avgBuffer = 0.0;    // Store moving average
  static int prevSleepPeriod = 1;  // Start with 8 sec (safety first)
  static bool initialFlag = true;  // To find the first time execution of the function

  // To initiate system with tank filling trend, the moving average buffer will take time to fill up
  // Updating the moving average buffer and avg value to ensure it will start with filling trend
  // Fail proof to ensure initial tank filling event detection
  if (initialFlag) {
    initialFlag = false;
    MovAvgBuffer[0] = depth + MOV_AVG_BUFFER_TOLERANCE + 2;
    MovAvgBuffer[1] = depth + MOV_AVG_BUFFER_TOLERANCE + 2;
    MovAvgBuffer[2] = depth + MOV_AVG_BUFFER_TOLERANCE + 2;
    MovAvgBuffer[3] = depth + MOV_AVG_BUFFER_TOLERANCE + 2;
    avgBuffer = depth + MOV_AVG_BUFFER_TOLERANCE + 2;
  }

  // Detect filling trend (tolerance: MOV_AVG_BUFFER_TOLERANCE - user defined macro)
  bool levelTrend = false;
  if ((avgBuffer - depth) >= MOV_AVG_BUFFER_TOLERANCE) {
    levelTrend = true;  // Filling detected
#ifdef DEBUG_SLEEP
    Serial.println(F("[dynamicSleepPeriod] Filling trend detected"));
#endif
  } else {
    levelTrend = false;  // Discharging or stable
  }

  // Calculate elapsed time based on previous sleep period
  int elapsedSeconds;
  switch (prevSleepPeriod) {
    case 37: elapsedSeconds = 296; break;  // 37 * 8 ≈ 300 sec
    case 15: elapsedSeconds = 120; break;  // 15 * 8 = 120 sec
    case 8: elapsedSeconds = 64; break;    // 8 * 8 = 64 sec
    case 4: elapsedSeconds = 32; break;    // 4 * 8 = 32 sec
    case 2: elapsedSeconds = 16; break;    // 2 * 8 = 16 sec
    default: elapsedSeconds = 8; break;    // 1 * 8 = 8 sec
  }

  // Add to total elapsed time tracker
  totalElapsedTime += elapsedSeconds;

  // Update moving average buffer every ~5 minutes (296 seconds)
  if (totalElapsedTime >= 296) {
    // Shift buffer: [0]←[1]←[2]←[3]←new reading
    MovAvgBuffer[0] = MovAvgBuffer[1];
    MovAvgBuffer[1] = MovAvgBuffer[2];
    MovAvgBuffer[2] = MovAvgBuffer[3];
    MovAvgBuffer[3] = depth;

    // Calculate new moving average
    avgBuffer = (MovAvgBuffer[0] + MovAvgBuffer[1] + MovAvgBuffer[2] + MovAvgBuffer[3]) / 4.0;

    totalElapsedTime = 0;  // Reset timer

    // Special case: When tank is nearly full (>97%), reset buffer to current depth
    // This prevents continuous fast sampling AFTER tank fills and stabilizes
    // The moving average will gradually converge to current level over 5 minutes
    // allowing safe return to normal sampling rate
    if (wtrLvlPercent > 97) {
      MovAvgBuffer[0] = depth - MOV_AVG_BUFFER_TOLERANCE - 3;
      MovAvgBuffer[1] = depth - MOV_AVG_BUFFER_TOLERANCE - 3;
      MovAvgBuffer[2] = depth - MOV_AVG_BUFFER_TOLERANCE - 3;
      MovAvgBuffer[3] = depth - MOV_AVG_BUFFER_TOLERANCE - 3;
#ifdef DEBUG_SLEEP
      Serial.println(F("[dynamicSleepPeriod] Buffer synchronized for tank full condition"));
#endif
    }

#ifdef DEBUG_SLEEP
    Serial.print(F("[dynamicSleepPeriod] Buffer updated, Avg: "));
    Serial.print(avgBuffer);
    Serial.println(F(" cm"));
#endif
  }

  // Determine next sleep period based on trend and water level
  int sleepPeriod;
  if (!levelTrend) {
    sleepPeriod = 37;  // Discharge: 37 sleeps ≈ 5 min
#ifdef DEBUG_SLEEP
    Serial.println(F("[dynamicSleepPeriod] Discharge mode: 5 min sleep"));
#endif
  } else {
    // Filling mode: Adaptive sampling based on tank level
    if (wtrLvlPercent <= 50) {
      sleepPeriod = 15;  // 120 sec
    } else if (wtrLvlPercent <= 70) {
      sleepPeriod = 8;  // 64 sec
    } else if (wtrLvlPercent <= 80) {
      sleepPeriod = 4;  // 32 sec
    } else if (wtrLvlPercent <= 90) {
      sleepPeriod = 2;  // 16 sec
    } else {
      sleepPeriod = 1;  // 8 sec (Emergency near full)
    }

#ifdef DEBUG_SLEEP
    Serial.print(F("[dynamicSleepPeriod] Filling mode: Level="));
    Serial.print(wtrLvlPercent);
    Serial.print(F("%, Sleep="));
    Serial.print(sleepPeriod * 8);
    Serial.println(F(" sec"));
#endif
  }

  // Save current sleep period for next elapsed time calculation
  prevSleepPeriod = sleepPeriod;

  return sleepPeriod;
}

// End of file "dynamicSleepPeriod.ino"