/*
 * ============================================================
 *  Water Reminder Alert - Version 2: Button Mode (LED only)
 * ============================================================
 *  No 7-segment display required.
 *
 *  HOW IT WORKS:
 *    - Default on power-up: Mode 1 (every 1 minute)
 *    - LED flashes once at startup to indicate Mode 1
 *    - Each button press cycles mode: 1 -> 2 -> 3 -> 4 -> 1
 *    - After each mode change, LED flashes mode number once:
 *        Mode 1: 1 flash
 *        Mode 2: 2 flashes
 *        Mode 3: 3 flashes
 *        Mode 4: 4 flashes
 *    - Reminder alert stays the same: 5 sets of ding-ding-ding
 *      with LED flashing in sync with buzzer.
 * ============================================================
 */

// Pin definitions
const int PIN_BUTTON = 2;   // Push button (INPUT_PULLUP, button to GND)
const int PIN_BUZZER = 10;  // Active buzzer
const int PIN_LED    = 8;   // Mode indicator + alert LED

// Four reminder modes in minutes
const unsigned long INTERVALS_MIN[4] = {1, 5, 10, 20};

const unsigned long DEBOUNCE_MS = 50;

// Alert configuration: 5x (ding-ding-ding), 1s pause between sets
const int BUZZ_BEEP_SET   = 3;
const int BUZZ_SET_REPEAT = 5;
const int BUZZ_ON_MS      = 80;
const int BUZZ_OFF_MS     = 70;
const int BUZZ_SET_PAUSE  = 1000;

// Mode indicator flash timing
const int MODE_FLASH_ON_MS  = 130;
const int MODE_FLASH_OFF_MS = 170;

byte currentMode = 0;               // 0..3 -> mode 1..4
unsigned long intervalMs = 0;
unsigned long lastReminderTime = 0;
bool buttonWasPressed = false;

void applyMode();
void setAlertOutputs(bool on);
void flashModeIndicator(byte modeNumber);
void triggerAlert();
void handleButton();

void setup() {
  Serial.begin(9600);

  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  pinMode(PIN_BUTTON, INPUT_PULLUP);

  // Always start from Mode 1 on power-up.
  currentMode = 0;
  applyMode();

  Serial.println("=== Water Reminder Alert STARTED (LED-only) ===");
  Serial.println("Default mode: 1");

  // One-time startup mode indicator.
  flashModeIndicator(currentMode + 1);

  lastReminderTime = millis();

  Serial.print("Interval set to: ");
  Serial.print(INTERVALS_MIN[currentMode]);
  Serial.println(" minute(s)");
  Serial.println("Press button to change mode.");
}

void loop() {
  handleButton();

  unsigned long now = millis();
  if ((now - lastReminderTime) >= intervalMs) {
    lastReminderTime = now;
    triggerAlert();
  }
}

void handleButton() {
  static unsigned long lastDebounceTime = 0;
  static bool lastRawState = HIGH;

  bool rawState = digitalRead(PIN_BUTTON);

  if (rawState != lastRawState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) >= DEBOUNCE_MS) {
    if (rawState == LOW && !buttonWasPressed) {
      buttonWasPressed = true;

      currentMode = (currentMode + 1) % 4;
      applyMode();
      lastReminderTime = millis();

      // One-time indicator after mode change.
      flashModeIndicator(currentMode + 1);

      Serial.print("Mode changed to: ");
      Serial.print(currentMode + 1);
      Serial.print(" -> Every ");
      Serial.print(INTERVALS_MIN[currentMode]);
      Serial.println(" minute(s)");
    } else if (rawState == HIGH) {
      buttonWasPressed = false;
    }
  }

  lastRawState = rawState;
}

void applyMode() {
  intervalMs = INTERVALS_MIN[currentMode] * 60UL * 1000UL;
}

void setAlertOutputs(bool on) {
  digitalWrite(PIN_BUZZER, on ? HIGH : LOW);
  digitalWrite(PIN_LED, on ? HIGH : LOW);
}

void flashModeIndicator(byte modeNumber) {
  if (modeNumber < 1) modeNumber = 1;
  if (modeNumber > 4) modeNumber = 4;

  for (byte i = 0; i < modeNumber; i++) {
    digitalWrite(PIN_LED, HIGH);
    delay(MODE_FLASH_ON_MS);
    digitalWrite(PIN_LED, LOW);

    if (i < modeNumber - 1) {
      delay(MODE_FLASH_OFF_MS);
    }
  }
}

void triggerAlert() {
  Serial.println(">>> DRINK WATER! <<<");

  for (int set = 0; set < BUZZ_SET_REPEAT; set++) {
    for (int i = 0; i < BUZZ_BEEP_SET; i++) {
      setAlertOutputs(true);
      delay(BUZZ_ON_MS);
      setAlertOutputs(false);
      if (i < BUZZ_BEEP_SET - 1) {
        delay(BUZZ_OFF_MS);
      }
    }

    if (set < BUZZ_SET_REPEAT - 1) {
      delay(BUZZ_SET_PAUSE);
    }
  }
}
