/*
 * ============================================================
 *  💧 Water Reminder Alert — Version 2: Button Mode
 * ============================================================
 *  No Blynk / No WiFi / No internet required.
 *  
 *  HOW IT WORKS:
 *    - Press button to cycle through 4 reminder intervals:
 *        Mode 1 → Every 1  minute
 *        Mode 2 → Every 5  minutes
 *        Mode 3 → Every 10 minutes
 *        Mode 4 → Every 20 minutes
 *    - The 7-segment display shows the current mode (1–4)
 *    - When time is up: buzzer beeps + LED flashes
 *    - Mode is saved to EEPROM (survives power-off)
 *  
 *  WIRING: See version2_button/WIRING.md
 *  
 *  HARDWARE:
 *    - Arduino UNO R3
 *    - Active Buzzer on D10
 *    - Red LED on D11 (via 220Ω resistor)
 *    - Button on D2 (INPUT_PULLUP — button connects D2 to GND)
 *    - 1-Digit 7-Segment (Common Cathode) on D3–D9
 *  
 *  LIBRARIES: None (built-in only)
 * ============================================================
 */

#include <EEPROM.h>

// ─── PIN DEFINITIONS ─────────────────────────────────────────
const int PIN_BUTTON  = 2;   // Push button (INPUT_PULLUP)
const int PIN_BUZZER  = 10;  // Active buzzer
const int PIN_LED     = 11;  // Red LED

// 7-Segment pins: a, b, c, d, e, f, g
const int SEG_PINS[7] = {3, 4, 5, 6, 7, 8, 9};

// ─── CONSTANTS ───────────────────────────────────────────────
// Four reminder modes in minutes
const unsigned long INTERVALS_MIN[4] = {1, 5, 10, 20};

// EEPROM address to store current mode
const int EEPROM_MODE_ADDR = 0;

// Debounce delay for button (ms)
const unsigned long DEBOUNCE_MS = 50;

// Alert configuration
const int BUZZ_BEEP_COUNT  = 3;   // Number of beeps per alert
const int BUZZ_ON_MS       = 200; // Each beep: ON duration
const int BUZZ_OFF_MS      = 150; // Each beep: OFF duration (gap)
const int LED_FLASH_COUNT  = 6;   // Number of LED flashes per alert
const int LED_ON_MS        = 100; // LED flash ON duration
const int LED_OFF_MS       = 100; // LED flash OFF duration

// ─── 7-SEGMENT DIGIT PATTERNS ────────────────────────────────
// Common Cathode: 1 = segment ON, 0 = segment OFF
// Order: a, b, c, d, e, f, g
//         3  4  5  6  7  8  9  (Arduino pins)
const byte DIGIT_PATTERNS[5][7] = {
  // a  b  c  d  e  f  g
  {  0, 0, 0, 0, 0, 0, 0 },  // [0] = blank (all off)
  {  0, 1, 1, 0, 0, 0, 0 },  // [1] = digit "1"
  {  1, 1, 0, 1, 1, 0, 1 },  // [2] = digit "2"
  {  1, 1, 1, 1, 0, 0, 1 },  // [3] = digit "3"
  {  0, 1, 1, 0, 0, 1, 1 },  // [4] = digit "4"
};

// ─── GLOBAL STATE ────────────────────────────────────────────
byte          currentMode       = 0;       // 0–3 (index into INTERVALS_MIN)
unsigned long intervalMs        = 0;       // Current interval in milliseconds
unsigned long lastReminderTime  = 0;       // millis() of last reminder
bool          buttonWasPressed  = false;   // Debounce state

// ─── FUNCTION PROTOTYPES ─────────────────────────────────────
void loadModeFromEEPROM();
void saveModeToEEPROM();
void applyMode();
void displayDigit(byte digit);
void clearDisplay();
void triggerAlert();
void buzzBeeps(int count);
void flashLED(int count);
void handleButton();

// ─────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);

  // Set up 7-segment pins
  for (int i = 0; i < 7; i++) {
    pinMode(SEG_PINS[i], OUTPUT);
    digitalWrite(SEG_PINS[i], LOW);
  }

  // Set up buzzer and LED
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  // Set up button with internal pull-up
  // Button should connect D2 → GND when pressed
  pinMode(PIN_BUTTON, INPUT_PULLUP);

  // Load saved mode from EEPROM
  loadModeFromEEPROM();
  applyMode();

  // Brief startup sequence — show all 4 digits scrolling
  Serial.println("=== Water Reminder Alert STARTED ===");
  Serial.print("Mode loaded from EEPROM: ");
  Serial.println(currentMode + 1);

  // Quick startup animation on 7-seg (1→2→3→4)
  for (byte d = 1; d <= 4; d++) {
    displayDigit(d);
    delay(200);
  }
  // Show current mode
  displayDigit(currentMode + 1);

  // Startup beep (1 short beep)
  digitalWrite(PIN_BUZZER, HIGH);
  delay(80);
  digitalWrite(PIN_BUZZER, LOW);

  lastReminderTime = millis();

  Serial.print("Interval set to: ");
  Serial.print(INTERVALS_MIN[currentMode]);
  Serial.println(" minute(s)");
  Serial.println("Press button to change mode.");
}

// ─────────────────────────────────────────────────────────────
void loop() {
  handleButton();

  unsigned long now = millis();

  // Check if it's time for a reminder
  if ((now - lastReminderTime) >= intervalMs) {
    lastReminderTime = now;
    triggerAlert();
  }

  // Optional: blink the display once per second as a "heartbeat"
  // to show the system is running (uncomment to enable)
  // static unsigned long lastBlink = 0;
  // static bool dispOn = true;
  // if (now - lastBlink >= 1000) {
  //   lastBlink = now;
  //   dispOn = !dispOn;
  //   if (dispOn) displayDigit(currentMode + 1);
  //   else clearDisplay();
  // }
}

// ─── BUTTON HANDLER ──────────────────────────────────────────
/*
 * Detects a debounced button press (INPUT_PULLUP: pressed = LOW).
 * Each press advances the mode by 1 and wraps at 4.
 * Resets the timer so the new interval starts fresh.
 */
void handleButton() {
  static unsigned long lastDebounceTime = 0;
  static bool lastRawState = HIGH;  // INPUT_PULLUP: unpressed = HIGH

  bool rawState = digitalRead(PIN_BUTTON);

  // Detect falling edge (button pressed)
  if (rawState != lastRawState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) >= DEBOUNCE_MS) {
    // Stable LOW = button is pressed
    if (rawState == LOW && !buttonWasPressed) {
      buttonWasPressed = true;

      // Advance mode
      currentMode = (currentMode + 1) % 4;
      saveModeToEEPROM();
      applyMode();

      // Reset timer from now
      lastReminderTime = millis();

      // Confirm beep (2 very short beeps)
      for (int i = 0; i < 2; i++) {
        digitalWrite(PIN_BUZZER, HIGH);
        delay(50);
        digitalWrite(PIN_BUZZER, LOW);
        delay(60);
      }

      Serial.print("Mode changed to: ");
      Serial.print(currentMode + 1);
      Serial.print(" → Every ");
      Serial.print(INTERVALS_MIN[currentMode]);
      Serial.println(" minute(s)");
    } else if (rawState == HIGH) {
      buttonWasPressed = false;
    }
  }

  lastRawState = rawState;
}

// ─── APPLY MODE ──────────────────────────────────────────────
/*
 * Updates intervalMs and the 7-segment display
 * to reflect the currentMode.
 */
void applyMode() {
  intervalMs = INTERVALS_MIN[currentMode] * 60UL * 1000UL;
  displayDigit(currentMode + 1);

  Serial.print("[applyMode] Mode ");
  Serial.print(currentMode + 1);
  Serial.print(" — every ");
  Serial.print(INTERVALS_MIN[currentMode]);
  Serial.println(" min");
}

// ─── TRIGGER ALERT ───────────────────────────────────────────
/*
 * Fires the water reminder: beep the buzzer, flash the LED.
 * Non-blocking approach: buzzer and LED run sequentially but
 * are fast enough (< 2 seconds total) not to interfere with
 * button detection in normal use.
 */
void triggerAlert() {
  Serial.println(">>> DRINK WATER! <<<");

  // Flash LED and buzz simultaneously in alternating pattern
  for (int i = 0; i < max(BUZZ_BEEP_COUNT, LED_FLASH_COUNT); i++) {
    // LED flash
    if (i < LED_FLASH_COUNT) {
      digitalWrite(PIN_LED, HIGH);
    }
    // Buzz
    if (i < BUZZ_BEEP_COUNT) {
      digitalWrite(PIN_BUZZER, HIGH);
    }
    delay(BUZZ_ON_MS);

    digitalWrite(PIN_BUZZER, LOW);
    digitalWrite(PIN_LED, LOW);
    delay(BUZZ_OFF_MS);
  }

  // Extra LED flashes if more than beep count
  for (int i = BUZZ_BEEP_COUNT; i < LED_FLASH_COUNT; i++) {
    digitalWrite(PIN_LED, HIGH);
    delay(LED_ON_MS);
    digitalWrite(PIN_LED, LOW);
    delay(LED_OFF_MS);
  }
}

// ─── DISPLAY DIGIT ───────────────────────────────────────────
/*
 * Shows a digit (1–4) on the 7-segment display.
 * Pass 0 to blank the display.
 * Uses DIGIT_PATTERNS array (Common Cathode — HIGH = segment on).
 */
void displayDigit(byte digit) {
  if (digit > 4) digit = 0;  // bounds check
  for (int seg = 0; seg < 7; seg++) {
    digitalWrite(SEG_PINS[seg], DIGIT_PATTERNS[digit][seg]);
  }
}

// ─── CLEAR DISPLAY ───────────────────────────────────────────
void clearDisplay() {
  displayDigit(0);
}

// ─── EEPROM HELPERS ──────────────────────────────────────────
/*
 * Loads mode from EEPROM. Defaults to mode 0 (1-minute)
 * if the stored value is invalid (e.g., first run).
 */
void loadModeFromEEPROM() {
  byte saved = EEPROM.read(EEPROM_MODE_ADDR);
  if (saved >= 4) {
    currentMode = 0;  // Default: mode 1 (every 1 minute)
  } else {
    currentMode = saved;
  }
  Serial.print("[EEPROM] Loaded mode: ");
  Serial.println(currentMode);
}

/*
 * Saves the current mode index to EEPROM.
 * Only writes if the value changed (preserves EEPROM write cycles).
 */
void saveModeToEEPROM() {
  EEPROM.update(EEPROM_MODE_ADDR, currentMode);
  Serial.print("[EEPROM] Saved mode: ");
  Serial.println(currentMode);
}
