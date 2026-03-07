# 📖 Full Setup Guide — Water Reminder Alert System

## Table of Contents
1. [Tools & Software You Need](#1-tools--software-you-need)
2. [Understanding Your Components](#2-understanding-your-components)
3. [Version 2: Button Mode (Start Here — Easiest)](#3-version-2-button-mode)
4. [Version 1: Blynk App Mode](#4-version-1-blynk-app-mode)
5. [Serial Monitor Debugging](#5-serial-monitor-debugging)
6. [Troubleshooting](#6-troubleshooting)
7. [Customization Options](#7-customization-options)

---

## 1. Tools & Software You Need

### Required for Both Versions
- **Arduino IDE** — free download at [arduino.cc/en/software](https://www.arduino.cc/en/software)
- **CH340 USB Driver** — needed for Chinese clone UNO boards (your kit says "CH340")
  - Download: [www.wch-ic.com/downloads/CH341SER_EXE.html](https://www.wch-ic.com/downloads/CH341SER_EXE.html)
  - Install it, then plug in your Arduino — Windows should recognize it

### Additional for Blynk Version (Version 1)
- **Python 3.x** — [python.org/downloads](https://www.python.org/downloads/)
- **pip package**: `blynk-library-python` (install via CMD)
- **Blynk app** on phone (Android/iOS)

---

## 2. Understanding Your Components

### Active Buzzer vs Passive Buzzer
Your kit has BOTH. This project uses the **Active Buzzer**.

| | Active Buzzer | Passive Buzzer |
|---|---|---|
| Has built-in oscillator | ✅ Yes | ❌ No |
| Needs `tone()` function | No — just HIGH/LOW | Yes |
| Sound | Fixed pitch beep | Programmable pitch |
| Identifies by | Has (+) marking on top | Looks the same but no (+) mark |

**How to tell them apart**: Apply 5V briefly — the active buzzer makes a continuous tone. The passive buzzer either stays silent or makes only a click.

### The 1-Digit 7-Segment Display
- Has **10 pins** (5 on each side)
- Likely **Common Cathode** (COM pins connect to GND)
- Each segment is an individual LED inside

```
      ┌──────────────────────────────────┐
PIN 1 │ e  d  COM  c  DP                │
      │ ┌─────────────────────────────┐ │
      │ │  ___                        │ │
      │ │ |   |  ← segments a,b,c,d,  │ │
      │ │ |___|     e,f,g visible     │ │
      │ │ |   |                       │ │
      │ │ |___|.                      │ │
      │ └─────────────────────────────┘ │
PIN 6 │ b  a  COM  f  g                 │
      └──────────────────────────────────┘
```

> **Tip**: To test your 7-segment display, connect its COM pin to GND and briefly touch each other pin to 5V (via a 220Ω resistor). Each segment will light up. Make a note of which pin = which segment.

---

## 3. Version 2: Button Mode

### Hardware Build Steps

#### Step 1 — Place components on breadboard

Place these components on your breadboard with space between them:
- 1-digit 7-segment display — straddle the center gap
- Active buzzer — one column away from 7-seg
- Red LED — another section
- Push button — corner of breadboard

#### Step 2 — Connect the Common Ground

Run a jumper wire from **Arduino GND** to the **blue rail** (minus rail) of the breadboard. All GND connections go here.

#### Step 3 — Wire the Active Buzzer

```
Arduino D10 → Buzzer pin marked (+) or longer leg
GND rail    → Buzzer (–) / shorter leg
```

#### Step 4 — Wire the Red LED

```
Arduino D11 → 220Ω Resistor → LED Anode (longer leg)
GND rail    ← LED Cathode (shorter leg)
```

#### Step 5 — Wire the Push Button

```
One leg of button → Arduino D2
Same leg          → 10kΩ Resistor → GND rail
Other leg of button → GND rail

(This uses INPUT_PULLUP — button reads LOW=pressed when you press it)
```

Wait — the code uses `INPUT_PULLUP`. That means:
```
One leg of button → Arduino D2
Other leg of button → GND rail
```
No external resistor needed with `INPUT_PULLUP`! The code already handles this.

#### Step 6 — Wire the 7-Segment Display

First, identify your COM pins (usually 2 of the 10 pins are COM — they connect internally):

```
COM pin → GND rail  (may be 2 COM pins — connect both)
```

Then connect each segment through a 220Ω resistor:

```
7-seg pin [a] → 220Ω → Arduino D3
7-seg pin [b] → 220Ω → Arduino D4
7-seg pin [c] → 220Ω → Arduino D5
7-seg pin [d] → 220Ω → Arduino D6
7-seg pin [e] → 220Ω → Arduino D7
7-seg pin [f] → 220Ω → Arduino D8
7-seg pin [g] → 220Ω → Arduino D9
```

> Use a different row for each 220Ω resistor on the breadboard so they don't touch.

#### Step 7 — Final check
Before connecting to PC:
- [ ] Buzzer polarity correct
- [ ] LED polarity correct (longer leg = anode)
- [ ] 7-seg COM connected to GND
- [ ] All resistors properly in series with LEDs/segments
- [ ] No bare wire legs touching each other

### Software Upload Steps

#### Step 1 — Install CH340 Driver
Install the CH340 driver (see Section 1). Restart PC if needed.

#### Step 2 — Plug in Arduino
Connect via USB. Check Device Manager → Ports → should see **CH340 (COMX)**.

#### Step 3 — Open Arduino IDE
Open `version2_button/water_reminder_button.ino`

#### Step 4 — Configure IDE
- **Tools → Board → Arduino AVR Boards → Arduino Uno**
- **Tools → Port → COM5** (or whichever port your Arduino appears on)

#### Step 5 — Upload
Click the **Upload** button (right arrow →).
Wait for "Done uploading" message.

#### Step 6 — Test it!
1. The display should show **1** (Mode 1 = every 1 minute)
2. After 1 minute, the buzzer beeps and LED flashes — *drink your water!*
3. Press the button — display changes to **2** (every 5 minutes)
4. Press again → **3** (10 min) → **4** (20 min) → back to **1**

---

## 4. Version 1: Blynk App Mode

### Hardware Build Steps

Much simpler than Version 2 — only 2 components + resistor:

#### Step 1 — Wire the Active Buzzer
```
Arduino D8  → Buzzer (+)
Arduino GND → Buzzer (–)
```

#### Step 2 — Wire the Red LED
```
Arduino D9  → 220Ω Resistor → LED Anode (+)
Arduino GND ← LED Cathode (–)
```

### Blynk App Setup

#### Step 1 — Download Blynk Legacy App
- Android: search "Blynk" on Play Store (version 2.27.x or lower — the old/legacy version)
- iOS: same on App Store
- **Important**: Use Blynk Legacy, not Blynk IoT 2.0 (the new cloud version is different)

#### Step 2 — Create Account
- Open app → Create New Account
- Use your real email (your auth token comes here)

#### Step 3 — Create New Project
1. Tap **"New Project"**
2. **Project name**: Water Reminder
3. **Choose Device**: Arduino UNO
4. **Connection Type**: USB
5. Tap **Create**
6. The app sends your **Auth Token** to your email — **copy it**

#### Step 4 — Add Widgets to Dashboard
Tap the **+** icon to add widgets:

**Widget 1 — Interval Control**
- Type: **Slider**
- Output: **V0**
- Label: `Interval (min)`
- Min: `1`, Max: `60`, Decimals: No
- Tap OK

**Widget 2 — Test Button**
- Type: **Button**
- Output: **V1**
- Label: `Test Alert Now`
- Mode: **Push** (not Switch)
- Tap OK

**Widget 3 — Countdown Display**
- Type: **Value Display**
- Input: **V2**
- Label: `Next Alert`
- Reading Rate: **1 sec**
- Tap OK

Arrange them nicely on the dashboard. Tap the top-right **Play** triangle (▶) to activate the project.

#### Step 5 — Paste Auth Token into Code
1. Open `version1_blynk/water_reminder_blynk.ino`
2. Find line 37:
   ```cpp
   #define BLYNK_AUTH_TOKEN   "YourAuthTokenHere"
   ```
3. Replace `YourAuthTokenHere` with your token (keep the quotes):
   ```cpp
   #define BLYNK_AUTH_TOKEN   "abc123def456ghijklmnop"
   ```

#### Step 6 — Install Blynk Library in Arduino IDE
1. Arduino IDE → **Sketch → Include Library → Manage Libraries**
2. Search: `Blynk`
3. Find **Blynk by Volodymyr Shymanskyy**
4. Install version **0.6.7** (Legacy compatible)

#### Step 7 — Upload Sketch to Arduino
1. **Tools → Board → Arduino Uno**
2. **Tools → Port → your COM port**
3. Click Upload (→)
4. Wait for "Done uploading"

#### Step 8 — Install Python Bridge on PC
Open **Command Prompt** (Start → type "CMD"):
```cmd
pip install blynk-library-python
```

#### Step 9 — Run the USB Bridge
In CMD (replace `COM5` and `YourToken`):
```cmd
python -m blynk_serial_usb --port COM5 --token YourAuthTokenHere
```

You should see:
```
Connecting to: blynk.cloud:80
Blynk connected!
Ping: 45ms
```

#### Step 10 — Use the App!
- In Blynk app: the project should show **ONLINE** (green dot)
- Drag the **Interval slider** to e.g. 30 minutes
- The countdown display shows time until next alert
- Tap **Test Alert Now** to immediately test your buzzer + LED

---

## 5. Serial Monitor Debugging

Both versions print helpful debug info to the Serial Monitor:
- Arduino IDE → **Tools → Serial Monitor**
- Set baud rate to **9600**

Example output for Version 2:
```
=== Water Reminder Alert STARTED ===
Mode loaded from EEPROM: 1
[applyMode] Mode 1 — every 1 min
Mode changed to: 2 → Every 5 minute(s)
[EEPROM] Saved mode: 1
>>> DRINK WATER! <<<
```

---

## 6. Troubleshooting

### Arduino not detected by PC
- Install CH340 driver → [wch-ic.com](https://www.wch-ic.com/downloads/CH341SER_EXE.html)
- Try a different USB cable (many USB cables are charge-only, not data)
- Try a different USB port on your PC

### Buzzer doesn't sound
- Check polarity: (+) to D8/D10, (–) to GND
- Test: Open Serial Monitor and watch for ">>> DRINK WATER! <<<"
- Verify it's the **Active** buzzer (should have a + marking on it)

### LED doesn't flash
- Check polarity: longer leg (anode) goes through resistor to D9/D11
- Check resistor: use a 220Ω (red-red-brown color code)
- Test LED directly: briefly connect anode to 5V through resistor

### 7-Segment shows wrong digit or garbled
- Your display may be **Common Anode** instead of Common Cathode
  - Test: connect COM to 5V and probe segments to GND — if they light up, it's Common Anode
  - Fix: in `water_reminder_button.ino`, change `displayDigit()` — swap HIGH↔LOW:
    ```cpp
    // Common Anode version:
    digitalWrite(SEG_PINS[seg], !DIGIT_PATTERNS[digit][seg]);
    ```
- Segments a–g may be wired in different order than your display's pinout
  - Re-check which physical pin = which segment letter using continuity test

### Button doesn't change mode
- Check wiring: one leg to D2, other leg to GND (with `INPUT_PULLUP` no pull-down resistor needed)
- Try pressing firmly — cheap buttons need solid contact
- Check Serial Monitor for "Mode changed to:" messages

### Blynk app shows OFFLINE
- Make sure the Python bridge script is running in CMD
- Make sure the COM port name is correct (check Device Manager)
- Make sure the Auth Token is identical in both the code and the CMD command
- Blynk Legacy servers: connect to `blynk.cloud` or `blynk-cloud.com`

### Blynk slider doesn't seem to work
- Make sure widget is set to **V0** (Virtual Pin 0)
- Sync values: in app, close and reopen the project
- Check Serial Monitor for `[Blynk] Interval changed to:` messages

---

## 7. Customization Options

### Change Reminder Sound Pattern (both versions)
In the `.ino` file, modify:
```cpp
const int BUZZ_BEEP_COUNT = 3;   // How many beeps
const int BUZZ_ON_MS      = 200; // How long each beep lasts
const int BUZZ_OFF_MS     = 150; // Gap between beeps
```

Try a more urgent pattern:
```cpp
const int BUZZ_BEEP_COUNT = 5;
const int BUZZ_ON_MS      = 80;
const int BUZZ_OFF_MS     = 80;
```

### Change LED Flash Pattern
```cpp
const int LED_FLASH_COUNT = 6;   // How many flashes
const int LED_ON_MS       = 100; // How long each flash
const int LED_OFF_MS      = 100; // Gap between flashes
```

### Use RGB LED Instead of Plain Red LED (Version 2)
Your kit has an RGB LED (4 legs: R, G, B, common). Connect:
```
Pin 11 → 220Ω → R (red)     ← for alert
Pin 12 → 220Ω → G (green)   ← for "system OK"
Pin 13 → 220Ω → B (blue)    ← for mode change
Common → GND (if common cathode) or 5V (if common anode)
```

### Add a Second LED for Status (e.g. green "system running" blink)
```cpp
const int PIN_STATUS_LED = 12;

// In loop(), add a heartbeat blink every 5 seconds:
static unsigned long lastBeat = 0;
if (millis() - lastBeat >= 5000) {
  lastBeat = millis();
  digitalWrite(PIN_STATUS_LED, HIGH);
  delay(50);
  digitalWrite(PIN_STATUS_LED, LOW);
}
```

### Change Mode Intervals (Version 2)
```cpp
// In water_reminder_button.ino, modify:
const unsigned long INTERVALS_MIN[4] = {1, 5, 10, 20};
//                                       ↑  ↑   ↑   ↑
//                                   Mode1  2   3   4

// Example — change to 15, 30, 45, 60 minute intervals:
const unsigned long INTERVALS_MIN[4] = {15, 30, 45, 60};
```

### Extend Blynk Interval Range (Version 1)
```cpp
// In water_reminder_blynk.ino, the clamp is at line with:
if (newInterval < 1)  newInterval = 1;
if (newInterval > 60) newInterval = 60;

// Change 60 to 120 (up to 2 hours):
if (newInterval > 120) newInterval = 120;
// And update your Blynk Slider max value to 120 in the app
```
