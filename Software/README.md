# Intelligent Water Pump (1HP) with Wireless Sensor

This repository contains the Arduino source code for the automatic/intelligent water pump control system consisting of:

- **Main Controller Unit** (receiver with LCD, relay control, user interface, safety monitoring, etc.)
- **Wireless Transmitter/Sensor Unit** (solar + battery-powered tank level sensor using ultrasonic sensor with RF transmission)

The code is organized into separate folders, one for each major version:

- `v2.0.0` (latest)  
- `v1.2`  
- `v1.1`  
- `v1.0`  
- `v0.0` (initial prototype)

Each folder contains the full source code and supporting files for that version.

## Version History

### v2.0.0 – Current Version (Efficient dynamic sleep for Tx & Tx Sensor Upgrade)

The main controller program remains unchanged — no modifications were made. Only the transmitter/sensor unit program was updated.

**Change 1: New sensor**  
Used new sensor named **DYP-A02YYTW-V2.0**. It is a UART-Controlled Ultrasonic Sensor capable of measuring from **3–450 cm**. Here we need to trigger the sensor to get a distance measurement. Normally, the sensor is triggered by sending a signal of *0x55* through UART. We implemented **SoftwareSerial** instead of hardware serial because hardware serial is already being used for the serial monitor.

> **Library details**  
Software serial library used: **SoftwareSerial.h**  
Documentation link: [https://docs.arduino.cc/learn/built-in-libraries/software-serial/](https://docs.arduino.cc/learn/built-in-libraries/software-serial/)

> **Sensor details**  
Sensor name: **DYP-A02YYTW-V2.0** (Datasheet available in `hardware/datasheet` folder)  
Price (INR): **1,665.00**  
Purchase date: **04/12/2025**  
Website: [https://robu.in/product/ultrasonic-sensor-dyp-a02yytw-v2-0/](https://robu.in/product/ultrasonic-sensor-dyp-a02yytw-v2-0/)

**Change 2: Dynamic sleep period**  
Previously, the sleep period of the system was determined by the tank level alone. Then the problem was that once the tank was full, the sensor was continuously taking measurements irrespective of the motor status. Actually, after the pump turns off following a full tank, the farst measurements are not required. So we implemented an algorithm to find the pump status by looking at the water level changes:  
- If the water level is increasing → pump is on  
- If not changing or reducing → pump is off  

Based on that, we can dynamically control the sleep period. Not just tank level alone — pump condition is also considered:  
- Pump off → slow readings  
- Pump on and low level → not full-speed reading  
- If level is going to reach 100% and pump is on → fast readings  

This means dynamically controlling the sleep period based on tank level and pump condition. Pump condition is determined by using a **moving average** algorithm on the last 4 periodic tank level readings.

**Change 3: Level setup bug fix**  
Previously, button press was determined by just detecting a press, without looking for release of the button. So if the user pressed and held, multiple option selections would happen. Solved by first looking for press; once detected, then looking for release. Only then the system determines it as a valid button press and takes action. Implemented using while loops, because it is a setup program that does not happen regularly — no need to do any other process simultaneously, so blocking the code with while loops is fine. Keeping it as a simple implementation.

**Change 4:**  
Small optimizations and small bug fixes in the code. Also added some comments for better readability.

### v1.2 – Sensor Accuracy Improvement  

The main controller program remains unchanged — no modifications were made. Only the transmitter/sensor unit program was updated.

**Change: New sensor**  
Used a new waterproof UART-based ultrasonic sensor named **DYP-A02YYUW** for more accurate readings. The previous sensor had a larger blind zone, so there were problems reading nearby distances. This sensor has only a 3 cm blind zone, so nearby distances are easy to measure. This is an auto UART Ultrasonic Sensor capable of measuring from **3–450 cm**. There is no need to trigger the sensor to get a distance measurement — once powered it automatically and continuously sends data through UART. Here we used **SoftwareSerial** instead of hardware serial because hardware serial is already being used for the serial monitor. Inside the folder there is one subfolder named Sensor trials which contains the sample program for the sensor.

>**Library details**  
Software serial library used: **SoftwareSerial.h**  
Documentation link: [https://docs.arduino.cc/learn/built-in-libraries/software-serial/](https://docs.arduino.cc/learn/built-in-libraries/software-serial/)

>**Sensor details**    
Sensor name: **DYP-A02YYUW** (Datasheet available in `hardware/datasheet` folder)  
Price (INR): **1599.00**  
Purchase date: **29/01/2025**  
Website: [https://robu.in/product/dfrobot-a02yyuw-waterproof-ultrasonic-sensor/](https://robu.in/product/dfrobot-a02yyuw-waterproof-ultrasonic-sensor/)

### v1.1 – Bug fix

The main controller program remains unchanged — no modifications were made. Only the transmitter/sensor unit program was updated.

**Chnage: Replacement of damaged sensor**  
Used the same sensor as earlier, but the triggering pulse is different in this sensor. For the previous sensor a positive pulse was used for triggering, but here it uses a negative pulse. So only the triggering pulse part was changed in the sensor read program — no other changes were made.

>**Sensor details**  
Sesnor name: **JSN-SR04T** (Datasheet available in `hardare/datasheet` folder)  
Price (INR): **499.00**   
Purchase date: **01/07/2024**  
Website: [https://www.amazon.in/dp/B077TPQCXX?ref=ppx_yo2ov_dt_b_fed_asin_title&th=1](https://www.amazon.in/dp/B077TPQCXX?ref=ppx_yo2ov_dt_b_fed_asin_title&th=1)

### v1.0 – First Stable Prototype

The main controller program had only minor bug fixes and the transmitter/sensor unit program was updated.

**Change: New sensor**  
The previous sensor was getting damaged due to humidity and condensation. So a new waterproof sensor was purchased. Only the sensor read program was changed to make it suitable for this new sensor.
Working is almost the same — not much difference.  

>**Sensor details:**  
Sensor name: **JSN-SR04T** (Datasheet available in `hardware/datasheet` folder)  
Price (INR): **219.00**  
Purchase date: **04/06/2024**  
Website: [https://robu.in/product/waterproof-ultrasonic-obstacle-sensor-reversing-radar-sensor/](https://robu.in/product/waterproof-ultrasonic-obstacle-sensor-reversing-radar-sensor/)

### v0.0 – Initial Prototype

First prototype. Explained below in System overview.

- Basic ultrasonic distance measurement  
- Simple 4-bit RF transmission of water level ranges  
- Fixed Tx sleep interval based on water level
- Basic button-based min/max level calibration in Tx 
- Initial main controller with LCD, relay, LEDs, basic alarms  
- Core safety checks: dry-run, low voltage, power failure
- User controlled system settings through LCD and buttons

### System Overview (Prototype v0.0)

#### Transmitter:

This Arduino-based transmitter/sensor unit complements the main controller by measuring water tank depth using an **ultrasonic sensor**, calculating the water level percentage based on pre-set min/max depths stored in **EEPROM**, and wirelessly transmitting 4-bit encoded data (water level ranges, battery status, solar charging) via **RF** to the receiver. It supports an **initial setup** mode for calibrating low and high water levels with button input and **LED feedback**, periodically checks battery voltage and solar charging, adjusts **sleep periods** for power efficiency based on water level, and transmits data in bursts. Overall, it ensures energy-efficient, reliable data collection from the tank sensor, with fault-tolerant measurements and periodic status updates every few seconds, depending on conditions.

The unit begins with an initial setup routine triggered at startup: it blinks the LED to signal readiness, waits for a button press or timeout (5 seconds), and if pressed, guides the user through setting the low water level (press and hold to save current ultrasonic reading to EEPROM address 0), confirms with LED blinks, then repeats for high level (saved to address 4) `[initialLevelSETUP.ino]`. The core sensor measurement powers on the ultrasonic module, takes three rapid readings (trigger pulse, echo duration converted to depth in cm), averages them, and repeats the process in a loop until the average falls within a 10% tolerance of the middle value for accuracy, then powers off the sensor `[sensorMeaurement.ino]`.  

In the main program, pins are initialized for ultrasonic, RF transmitter, data bits, button, LED, and solar input; it runs the initial setup, loads min/max levels from EEPROM, then in the loop: measures depth, maps it to a 1-100% level (constrained), encodes the percentage into one of 10 discrete 4-bit ranges (e.g., 0-13% as 0000, 98-100% as 1001), transmits via RF, increments a counter, and every third cycle sends battery low/good status followed by solar charging detection (based on analog read of solar pin, low if <= 512), then resets the counter; sleep duration is set based on level (longer at mid-levels for efficiency), and it enters low-power idle for 0-3 periods of 8 seconds each using the **LowPower library** `[Transmitter_Sensor_Unit_Prgm.ino]`. RF transmission sets the 4 data pins (complemented due to hardware), powers on the RF module, sends two enable pulses to transmit the data stream, then powers off `[rfTransmission.ino]`. Battery voltage is measured every third loop cycle by reading the internal 1.1V reference via ADC and calculating AVcc in millivolts, flagging low if below 3400mV `[battVolatgeCalculator.ino]`.

>**Sensor details**  
Sesnor name: **HC-SR04T** (Datasheet available in `hardare/datasheet` folder)  
Price (INR): **60.00**   
Purchase date: NA  
Website: Purchnased offline

#### Receiver/ main controller:

This Arduino-based controller manages an automatic water pump system designed to maintain water levels in a tank. It supports auto and manual operation modes, programmable pump run **schedules** (eg: every *6/8/12/24* hours), automatic pump off **timers** after running, and multiple safety protections such as **dry-run cutoff**, **low-voltage** protection, power failure detection, and transmitter (sensor unit) timeout (**Tx fail**) monitoring. The system receives real-time water level percentage and transmitter battery/solar charging status wirelessly via **interrupt**-driven data bits. It continuously monitors mains voltage presence, low voltage, and dry-run conditions from the power board. Based on these inputs, user settings (stored in EEPROM), and current state, it decides whether to turn the pump relay ON or OFF, activates visual/audible **alarms** when needed, and provides a user-friendly LCD interface with button navigation to view status, change settings, check active alarms, or perform factory reset. The result is a reliable, safe, and user-configurable automatic water level control system with good fault detection and feedback.

The system starts with a welcome screen showing the project name, version, and credits. It then flashes the LEDs and buzzer a few times to test them before saying “Done” `[start_UpScreen.ino]`. From the menu, the user can choose factory reset, confirm by holding a button, and the system restores all default settings in EEPROM and shows a success message `[startupFactoryReset.ino]`.
An interrupt reads 4-bit data from the wireless transmitter (water level, battery low, solar charging status) and resets a timer that detects if the transmitter is missing `[interupt.ino]`. Another function continuously checks the power board for dry-run (low current), low voltage, and mains power presence, using short delays to confirm real faults before setting protective flags `[powerBoardReading.ino]`.  

The menu (entered with right button) lets the user change settings like Auto/Manual mode, pump schedule (off or every 6/8/12/24 hours), low-voltage cutoff, auto-off timer, dry-run protection, and LCD brightness. Changes are saved to **EEPROM** `[mainMenu.ino]`. Buttons are read from one analog pin using different voltage levels to detect Up, Down, Left, Right, or Enter `[button_Read.ino]`.
The LCD shows **three main screens**: the **home screen** with water percentage and status symbols (power, pump on, timer, auto mode, etc.), the **settings menu**, and the **alarm list**. Both menu and alarm screens return to home after 6 seconds of no activity `[displayCodesMain.ino]`. The microcontroller’s supply voltage is measured using the internal ADC for possible future use `[batVoltageCalculator.ino]`.

If there are alarms, the left button opens a list showing only active problems like Power Failure, Low Voltage, Dry Run, Transmitter Error, or Sensor Battery Low `[alarmMenu.ino]`. The main file initializes everything, runs the startup, then loops forever: reads buttons and transmitter data, calculates water level, checks power conditions, decides if the pump should run (based on mode, schedule, timers, faults), controls the relay, LEDs, buzzer, and alarm light, and can restart the Arduino after 2 days if the pump stays off `[Main_Controller_MCU_Prgm.ino]`.  

## Thanks To

- Special LCD symbols are made using the Website: [https://maxpromer.github.io/LCD-Character-Creator/](https://maxpromer.github.io/LCD-Character-Creator/)
