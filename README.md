# Intelligent Water Pump Controller (1HP) with Wireless Ultrasonic Sensor

An automatic, intelligent water pump control system for 1HP motors. It monitors overhead tank water level wirelessly, turns the pump ON/OFF based on user settings and tank level, and includes multiple safety protections to prevent damage from dry run, low voltage, power failure, etc.

The system has two units:
- **Main Controller Unit** (indoor) — shows water level on LCD, controls the pump relay, handles user settings via buttons, displays alarms.
- **Sensor Unit** (outdoor, on the tank) — battery + solar-powered, measures water level with ultrasonic sensor, sends data wirelessly via 433 MHz RF every few minutes (adaptive timing to save battery).

***No wiring between tank and controller — fully wireless communication.***

## Key Features

### Main Controller Unit
- 16×2 LCD display showing real-time water level percentage, pump status, power status, battery status, timer, alarms, and symbols.
- 5-button interface (Up, Down, Left, Right, Ok) using a single analog pin.
- Operation modes: **Auto** (level-based on/off) or **Manual** (force on/off).
- **Auto pump cut-in** at user-defined low level (10%, 20%, 30%, etc.).
- **Auto cutoff** when tank is full or on fault alarms.
- **Dry run** protection (cuts off pump if low current detected).
- **Low AC voltage cutoff.**
- Pump **runtime timer** (auto-off after set hours).
- **Scheduling** (pump runs every 6/8/12/24 hours if selected).
- Power failure detection and battery backup (shows level during blackout).
- Alarm list for all faults (power fail, low voltage, dry run, sensor error, sensor battery low, out-of-range, timer activated, etc.).
- Buzzer and alarm LED for alerts.
- Factory reset option via menu.

![Main Controller Unit](/Gallery/Rx_v1_final.jpg)

### Sensor Unit
- Waterproof **ultrasonic sensor** (DYP-A02YYTW-V2.0) for accurate tank level measurement.
- **Solar panel + Li-ion** battery based power source (no external power needed).
- Very low power consumption — sleeps most of the time, wakes only to measure and transmit.
- **Dynamic sleep** interval: faster sampling when tank is filling (to avoid overflow), slower when discharging (to save battery).
- Transmits water level (%), battery status (good/low), charging status, and error flags.
- One-time calibration for empty (low) and full (high) levels using button + LED feedback.

![Sensor Unit](/Gallery/Tx_v2_final.jpg)

## How It Works

1. **Sensor Unit** wakes up periodically (dynamic in behaviour), measures distance to water surface using ultrasonic sensor, calculates percentage, checks battery/charging, then transmits 4-bit data via 433 MHz RF (using **HT12E** encoder).

2. **Main Controller** receives the **RF** signal via **HT12D** decoder. It triggers an **interrupt** and reads the 4 data bits (D0–D3).
   - If it's a **status code** → updates battery low, solar charging, or `sensor out-of-range flags`.
   - If it's water **level data** → stores it and converts to percentage for display.
   - Resets the transmitter timeout timer (5 minutes). If no valid packet for >5 min → `transmitter error flag` is set.

3. In the main loop, the controller continuously reads the power board signals (via optocouplers):
   - **Dry run** (low current) — checked only when pump is running; confirmed after 30 seconds continuous low current.
   - **Low AC voltage** — checked always; when pump is running, confirmed after 30 seconds; when off, immediate.
   - **Power presence** — mains failure detected with short confirmation (1 s when running, longer recovery wait after failure).
   These readings set flags like `dryRunFlag`, `lowVoltFlag`, `pwrFailFlag`.

4. Pump relay control logic:
   - In **Auto mode** — pump turns ON if water level ≤ user-set cut-in % AND no active faults (dry run, low volt, power fail, transmitter error, timer expired, etc.).
   - Turns OFF when tank full, timer expires, or any fault appears.
   - In **Manual mode** — user forces ON/OFF via buttons, but safety cutoffs (dry run, low volt, timer, tank full) still apply.
   - Pump scheduling — allows the pump to run periodically at fixed intervals (every 6, 8, 12, or 24 hours) when selected by the user.
   - Auto-off timer — acts as the final safety layer — the pump automatically stops after the set time expires, even if all other conditions are normal, and triggers a fault alarm to notify the user.

5. **User Menu Screen** (for editing settings):
   - Press the **Right button** from home screen to enter the main menu.
   - Use **Up/Down** buttons to scroll through options (Operation Mode, Pump CutIN Level, Pump Schedule, Low Voltage Cutoff, Pump Auto-Off Timer, Dry Run Protection, LCD Brightness, Factory Reset, Back).
   - Press **Enter** to select an item for editing → it shows current value and side arrows for editing.
   - Use **Left/Right** buttons to change the value (toggle On/Off, select %, time, etc.).
   - Press **Enter** to save the change to EEPROM and exit edit mode.
   - Press **Back** (or wait 6 seconds inactivity) to return to home screen without saving.
   - Factory Reset option: Hold button to confirm → resets all settings to defaults and restarts the system.

6. Alarm handling:
   - If any fault is active (`dryRunFlag`, `lowVoltFlag`, `pwrFailFlag`, `transmitterError`, `batLow`, `sensorOutofRange`, timer alarm, etc.) → alarm flag = true.
   - Alarm LED blinks (500 ms).
   - Press **Left button** when alarm is active → opens alarm menu showing only current faults (e.g., "Power Failure", "Pump Dry Run", "Sensor Battery Low", "Sensor Out of Range", "Pump CutOFF Timer Activated").
   - Scroll with Up/Down; press Enter to exit.

7. During power failure (`!voltPresent`):
   - System continues on battery backup.
   - LCD backlight brightness reduced (analogWrite to low value, e.g., 25) to save power.
   - Water level and basic status still displayed; after power recovery pump may stay on/off depending on pump status before power failure and faults.

8. Other periodic tasks:
   - Home screen refreshes with large water % and small icons (power, pump running, timer, auto mode, battery good, transmitter good, alarm).
   - Menu/alarm screens timeout after 6 seconds inactivity and return to home.
   - Long-term auto system reset after ~10 days (to refresh everything).
   - If pump relay is on then the **buzzer** make short buzzing sound in every 30 sec to alert the user that the pump turned on and now running.

## User Interface

- **Home Screen** (default): Shows water level % in big digits, plus small icons for power, pump running, timer active, auto mode, battery good, transmitter good, alarms, etc.
- **Buttons**:
  - Left/Right: Navigate menu or change values.
  - Up/Down: Scroll menu items.
  - Enter: Select/edit/confirm.
- **Main Menu** (press Right button): Change operation mode, cut-in level, schedule, low voltage cutoff, auto-off timer, dry run protection, LCD brightness, factory reset.
- **Alarm Menu** (press Left button when alarm active): Lists active faults (e.g., "Power Failure", "Pump Dry Run", "Sensor Battery Low"). Scroll with Up/Down, exit with Enter.
- **Startup**: Shows welcome message, version, credits, tests LEDs/buzzer, then goes to home screen.
- **Factory Reset**: In menu → hold button to confirm → resets all settings to defaults and restarts.

## Hardware Overview

Detailed hardware design (PCBs, power circuits, sensor wiring, enclosures) is in the `/hardware/` folder.

- Main Controller PCB → ATmega328P, LCD via PCF8574 I2C, RF receiver + HT12D, button ladder, relay driver.
- Power PCB → VIPer22A SMPS (5V + 12V), battery charging (Li-ion TP4056 + lead-acid discrete), current sensing, dry run/low volt comparators.
- Sensor Unit PCB → ATmega328P (8 MHz internal), ultrasonic (UART), RF transmitter + HT12E, solar + Li-ion charging, power switching MOSFET.

Enclosures: IP65/IP67 waterproof plastic boxes (WJB17 for sensor, ID-9105Z for main unit). See `/hardware/README.md` for models and links.

PCB images: `./Gallery/RxTx_v1_combined.jpg`, `./Gallery/Tx_v1_pcb_combined.jpg`

## Software Overview

Source code is in the `/software/` folder, organized by unit and version.

- Transmitter (Sensor Unit): `/software/v2.0.0/` — latest with dynamic sleep, UART ultrasonic, battery/solar status transmission.
- Receiver (Main Controller): `/software/v1.0/` — full menu system, LCD symbols, safety logic, EEPROM settings.

See each folder's README for version history, key files/functions, libraries (LiquidCrystal_I2C, LowPower, SoftwareSerial, EEPROM, etc.), and changes.

## Where to Find More Details

- Hardware schematics, PCB layouts, bill of materials → `/hardware/`
- Full Arduino code, version notes, sensor datasheets → `/hardware/datasheet/` and `/software/`
- Mechanical enclosure photos and purchase info → `/hardware/README.md`

This project is designed for reliable, low-maintenance automatic water pumping with strong safety features — suitable for homes, farms, or overhead tanks.

Questions or improvements? Feel free to open an issue.
