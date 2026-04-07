# 🔌 Version 2 — Wiring Guide (Button Mode, No Blynk)

## Components Needed for This Version

| Component | Quantity |
|---|---|
| Arduino UNO R3 | 1 |
| Breadboard | 1 |
| Active Buzzer | 1 |
| Red LED | 1 |
| Push Button | 1 |
| 1-Digit 7-Segment Display | 1 |
| 220Ω Resistor | 8 (7 for 7-seg + 1 for LED) |
| M-M Dupont Wires | ~20 |
| USB Cable | 1 |

---

## 📌 Pin Assignments

| Arduino Pin | Connected To | Notes |
|---|---|---|
| **D2** | Button (one leg) | Other leg → GND; use `INPUT_PULLUP` in code |
| **D3** | 7-Seg segment **a** | Via 220Ω resistor |
| **D4** | 7-Seg segment **b** | Via 220Ω resistor |
| **D5** | 7-Seg segment **c** | Via 220Ω resistor |
| **D6** | 7-Seg segment **d** | Via 220Ω resistor |
| **D7** | 7-Seg segment **e** | Via 220Ω resistor |
| **D8** | 7-Seg segment **f** | Via 220Ω resistor |
| **D9** | 7-Seg segment **g** | Via 220Ω resistor |
| **D10** | Active Buzzer (+) | Buzzer (–) → GND |
| **D11** | Red LED (Anode +) | Via 220Ω resistor → LED → GND |
| **GND** | Common GND rail | All GND connections |
| **5V** | Breadboard power rail | Optional, for convenience |

---

## 🖥️ 1-Digit 7-Segment Display — Segment Map

```
      ___
     | a |
  f  |   |  b
     |_g_|
  e  |   |  c
     |___|
       d
```

### Typical Common-Cathode 7-Segment Pinout (looking at the FRONT face)

```
  Pin 1 = e     Pin 2 = d     Pin 3 = COM(GND)  Pin 4 = c     Pin 5 = DP
  Pin 6 = b     Pin 7 = a     Pin 8 = COM(GND)  Pin 9 = f     Pin 10 = g
```

> ⚠️ IMPORTANT: Pinout varies by manufacturer. Use a multimeter in continuity/diode mode to confirm.
> Touch the cathode (COM) leg to GND and probe each segment leg — it will light up.
> Label which leg = which segment before wiring.

### How to Identify Common-Cathode vs Common-Anode

- **Common Cathode**: COM pins go to GND; segment pins → HIGH to light up
- **Common Anode**: COM pins go to 5V; segment pins → LOW to light up
- The code in `water_reminder_button.ino` is written for **Common Cathode**
- If yours is Common Anode, change all `HIGH`→`LOW` and `LOW`→`HIGH` in `displayDigit()`

---

## 🔊 Active Buzzer Wiring

```
Active Buzzer
  ( + ) ──────── Arduino D10
  ( – ) ──────── Arduino GND

Note: Active buzzer has a built-in oscillator.
      It makes sound when you apply DC voltage — no tone() needed.
      Usually marked with (+) on the positive side or has a longer leg.
```

---

## 💡 Red LED Wiring

```
Arduino D11 ──── 220Ω Resistor ──── LED Anode (+, longer leg)
                                     LED Cathode (–, shorter leg) ──── GND

The 220Ω resistor limits current to ~14mA — safe for both LED and Arduino.
```

---

## 🔘 Button Wiring

```
Arduino D2 ──────── [Button] ──────── GND

No external 10kΩ resistor is needed.
Enable internal pull-up using `pinMode(D2, INPUT_PULLUP);`

When button is NOT pressed: D2 reads HIGH (internal pull-up)
When button IS pressed:     D2 reads LOW (button shorts D2 to GND)
```

> This is exactly how the provided code works: `INPUT_PULLUP` on D2.

---

## 🗺️ Full Breadboard Wiring Diagram (ASCII)

```
ARDUINO UNO                          BREADBOARD
                                      _______________________
 5V  ──────────────────────────────── + rail (red)
 GND ──────────────────────────────── – rail (black/blue)
                                      
 D2  ──────── [Button leg 1]
              [Button leg 2] ──────── GND rail

 D3  ──── 220Ω ──── 7seg pin [a]
 D4  ──── 220Ω ──── 7seg pin [b]
 D5  ──── 220Ω ──── 7seg pin [c]
 D6  ──── 220Ω ──── 7seg pin [d]
 D7  ──── 220Ω ──── 7seg pin [e]
 D8  ──── 220Ω ──── 7seg pin [f]
 D9  ──── 220Ω ──── 7seg pin [g]
          7seg COM pin ──────────────  GND rail

 D10 ──── Buzzer (+)
          Buzzer (–) ─────────────── GND rail

 D11 ──── 220Ω ──── LED Anode (+)
                    LED Cathode (–) ─ GND rail
```

---

## 🔋 Power

Connect Arduino to PC via USB cable — that's it. No separate power supply needed.

---

## 📋 Mode Summary

| Mode | LED on 7-Seg | Interval |
|---|---|---|
| 1 | `1` | Every **1 minute** |
| 2 | `2` | Every **5 minutes** |
| 3 | `3` | Every **10 minutes** |
| 4 | `4` | Every **20 minutes** |

Press the button to cycle: 1 → 2 → 3 → 4 → back to 1

The current mode is **saved to EEPROM** so it persists after power-off.

---

## ✅ Pre-Upload Checklist

- [ ] All GND connections share the GND rail
- [ ] 7-segment COM pin(s) connected to GND rail
- [ ] Each 7-segment segment has its own 220Ω resistor
- [ ] Buzzer polarity correct (+/– not reversed)
- [ ] LED polarity correct (longer leg = anode = toward resistor/D11)
- [ ] Button connected between D2 and GND (no 10kΩ needed)
- [ ] USB cable connected to Arduino and PC
- [ ] Correct COM port selected in Arduino IDE (Tools → Port)
- [ ] Board set to "Arduino Uno" in Arduino IDE

---

## 🛠️ If Display Is Wrong Or LED Flashes A Segment

If your mode display does not show `1,2,3,4` correctly, or if buzzer/LED flashing appears on a 7-segment segment, your wiring pin order is different from the default map.

Use the built-in software calibration in `water_reminder_button.ino`:

1. Hold the button while powering/resetting Arduino.
2. The sketch runs a segment test: logical `a,b,c,d,e,f,g` one by one.
3. Open Serial Monitor (9600 baud) and note which physical segment lights for each logical segment.
4. Update `SEG_PIN_BY_LOGICAL[7]` in the sketch so each logical segment points to the correct Arduino pin.
5. If your display is common-anode, set `SEG_COMMON_ANODE = true`.

After this, modes should display the real digits `1,2,3,4`, and LED will flash in sync with buzzer from the dedicated LED pin.
