# 💧 Water Reminder Alert System — Arduino UNO

A hydration reminder system using Arduino UNO that buzzes and flashes an LED at configurable intervals to remind you to drink water.

---

## 📦 Components Used (from your kit)

| Component | Quantity | Used For |
|---|---|---|
| Arduino UNO R3 CH340 | 1 | Microcontroller brain |
| Breadboard | 1 | Circuit assembly |
| USB Cable | 1 | Power + programming |
| Active Buzzer | 1 | Alert sound |
| Red/Yellow/Green LED | 1 (Red) | Flashing alert |
| Button Switch | 1 | Mode selection (Version 2) |
| 220Ω Resistors | 8 | LED + 7-segment current limiting |
| 10kΩ Resistor | 1 | Button pull-down |
| 1-Digit 7-Segment Display | 1 | Show current mode (Version 2) |
| M-M Dupont Wires | As needed | Connections |

---

## 📁 Project Structure

```
water-reminder-alert/
├── README.md                        ← You are here
├── SETUP_GUIDE.md                   ← Full detailed project guide (read this!)
├── version1_blynk/
│   ├── water_reminder_blynk.ino    ← Arduino code (Blynk version)
│   └── WIRING.md                   ← Wiring guide for Blynk version
└── version2_button/
    ├── water_reminder_button.ino   ← Arduino code (Button version)
    └── WIRING.md                   ← Wiring guide for Button version
```

---

## 🆚 Which Version Should I Use?

| Feature | Version 1 (Blynk) | Version 2 (Button) |
|---|---|---|
| Interval control | Blynk app slider | Physical button |
| Requires phone | Yes | No |
| Requires PC connection | Yes (USB bridge) | No |
| Modes | Any value 1–60 min | Fixed: 1 / 5 / 10 / 20 min |
| 7-Segment display | Not needed | Shows current mode |
| Extra hardware needed | None beyond kit | None beyond kit |

---

## ⚡ How the Alert Works (Both Versions)

1. Timer counts down to 0
2. **Active Buzzer** beeps 3 times (short beep pattern)
3. **Red LED** flashes rapidly 5 times
4. Timer resets and starts again
5. Drink your water! 💧

---

## 🚀 Quick Start

- **Full step-by-step guide** → [SETUP_GUIDE.md](SETUP_GUIDE.md) ← **Start here!**
- **No phone/app needed?** → Go to [version2_button/WIRING.md](version2_button/WIRING.md)
- **Want full app control?** → Go to [version1_blynk/WIRING.md](version1_blynk/WIRING.md)

---

## 📋 Libraries Required

### Version 1 (Blynk)
- `Blynk` by Volodymyr Shymanskyy — install via Arduino Library Manager

### Version 2 (Button)
- No external libraries needed — uses only Arduino built-ins

---

## 🔌 Power Options

- **USB from PC** — easiest for development
- **9V battery** via DC barrel jack — portable use
- **5V USB power bank** via USB port

---

*Built with ❤️ using Arduino UNO + basic components kit*
