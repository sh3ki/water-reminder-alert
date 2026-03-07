# 🔌 Version 1 — Wiring Guide (Blynk App Controlled)

## ⚠️ Important: How Blynk Works with Arduino UNO

Arduino UNO **does not have built-in WiFi**. To use Blynk, this version uses the
**Blynk USB Serial Bridge** method — your Arduino communicates to Blynk cloud
through your PC's USB connection. Your phone runs the Blynk app and controls
the Arduino via the cloud.

```
  [Blynk App] ←──WiFi──→ [Blynk Cloud] ←──Internet──→ [PC with blynk-ser script] ←──USB──→ [Arduino UNO]
```

> **Alternative**: If you own an **ESP8266 NodeMCU** (not in the kit), you can use
> that as a WiFi bridge. See the note at the bottom of this guide.

---

## Components Needed for This Version

| Component | Quantity |
|---|---|
| Arduino UNO R3 | 1 |
| Breadboard | 1 |
| Active Buzzer | 1 |
| Red LED | 1 |
| 220Ω Resistor | 1 (for LED) |
| M-M Dupont Wires | ~6 |
| USB Cable | 1 |

> Much simpler wiring than Version 2 — no button or 7-segment needed!

---

## 📌 Pin Assignments

| Arduino Pin | Connected To | Notes |
|---|---|---|
| **D8** | Active Buzzer (+) | Buzzer (–) → GND |
| **D9** | Red LED (Anode +) | Via 220Ω resistor → LED (–) → GND |
| **GND** | Common GND | Buzzer (–), LED cathode |

---

## 🗺️ Wiring Diagram

### Active Buzzer
```
Arduino D8 ──────────────── Buzzer (+) [marked or longer leg]
Arduino GND ─────────────── Buzzer (–)

Active buzzers have a built-in oscillator — just apply voltage and they beep.
```

### Red LED
```
Arduino D9 ──── 220Ω Resistor ──── LED Anode (+ / longer leg)
                                    LED Cathode (– / shorter leg) ──── Arduino GND
```

### Full Breadboard Layout
```
ARDUINO UNO                          BREADBOARD
 GND ──────────────────────────────── – rail (blue)

 D8  ──── Buzzer (+)
          Buzzer (–) ─────────────── – rail (GND)

 D9  ──── 220Ω ──── LED Anode (+)
                    LED Cathode (–) ─ – rail (GND)
```

---

## 📱 Blynk App Setup (Legacy Blynk — blynk.cc)

> This guide uses **Blynk Legacy** (the free version that works with Arduino serial bridge).

### Step 1 — Install Blynk App
- Android: [Blynk on Google Play](https://play.google.com/store/apps/details?id=cc.blynk)
- iOS: [Blynk on App Store](https://apps.apple.com/app/blynk-iot/id808760481)
- Use **Blynk Legacy** (not Blynk IoT 2.0)

### Step 2 — Create a New Project
1. Open Blynk app → **New Project**
2. Device: `Arduino UNO`
3. Connection: `USB`
4. Theme: Light or Dark
5. Tap **Create** — you'll receive an **Auth Token** by email

### Step 3 — Add Widgets

| Widget | Virtual Pin | Settings |
|---|---|---|
| **Slider** | V0 | Label: "Interval (min)", Min: 1, Max: 60, Step: 1 |
| **Button** | V1 | Label: "Test Alert Now", Mode: Push |
| **Value Display** | V2 | Label: "Next alert in:", Pin: V2, Refresh: 1 sec |

### Step 4 — Set Slider Default
- Set the Slider (V0) to a value (e.g., 30 minutes) before starting

---

## 💻 PC Setup — Blynk USB Serial Bridge

### Step 1 — Install Python (if not already installed)
- Download from [python.org](https://www.python.org/downloads/)
- During install: ✅ check "Add Python to PATH"

### Step 2 — Install Blynk serial bridge script
Open Command Prompt (CMD) and run:
```cmd
pip install blynk-library-python
```

### Step 3 — Find your Arduino COM port
- In Arduino IDE: Tools → Port → note the COM port (e.g., `COM5`)
- Or: Device Manager → Ports (COM & LPT)

### Step 4 — Run the bridge script
Open CMD and run (replace `COMX` and `YourAuthToken`):
```cmd
python -m blynk_serial_usb --port COMX --token YourAuthToken
```

Example:
```cmd
python -m blynk_serial_usb --port COM5 --token abc123def456xyz
```

> Keep this CMD window open while using the system.
> The script must be running for Blynk app to communicate with Arduino.

---

## 💻 Arduino IDE Setup

### Step 1 — Install Blynk Library
- Arduino IDE → Sketch → Include Library → Manage Libraries
- Search: `Blynk`
- Install: **Blynk** by Volodymyr Shymanskyy (version 0.6.x for Legacy)

### Step 2 — Update Auth Token in code
- Open `water_reminder_blynk.ino`
- Find this line near the top:
  ```cpp
  char auth[] = "YourAuthTokenHere";
  ```
- Replace `YourAuthTokenHere` with the token from your email

### Step 3 — Upload the sketch
- Set Board: Tools → Board → Arduino UNO
- Set Port: Tools → Port → your COM port
- Click Upload (→)

---

## 📱 Using the Blynk Dashboard

| Control | Function |
|---|---|
| **Slider (V0)** | Drag to set reminder interval (1–60 minutes) |
| **Test Alert button (V1)** | Press to fire an immediate test reminder |
| **Next Alert display (V2)** | Shows countdown in seconds until next reminder |

Changes to the slider take effect immediately — no restart needed.

---

## ✅ Pre-Upload Checklist

- [ ] Buzzer (+) on D8, (–) on GND
- [ ] LED anode → 220Ω → D9, cathode → GND
- [ ] Auth token copied correctly into the .ino file
- [ ] Blynk library installed (Sketch → Include Library)
- [ ] Board set to "Arduino Uno" in Arduino IDE
- [ ] Correct COM port selected in Arduino IDE
- [ ] USB cable connected to Arduino and PC
- [ ] Python installed and `blynk-library-python` pip package installed
- [ ] Blynk app project set to "USB connection" / "Arduino UNO"

---

## 🔁 Alternative: Using ESP8266 for WiFi (Advanced)

If you have an ESP-01 or NodeMCU ESP8266:

```
Arduino TX (D1) ──── ESP8266 RX
Arduino RX (D0) ──── ESP8266 TX  (use SoftwareSerial on other pins to avoid conflicts)
Arduino 3.3V ──────── ESP8266 VCC  (⚠️ ESP8266 runs at 3.3V, NOT 5V)
Arduino GND ────────── ESP8266 GND
```

Replace `#include <BlynkSimpleSerial.h>` with `#include <BlynkSimpleEsp8266.h>`
and add your WiFi SSID and password. No PC bridge script needed with this method.
