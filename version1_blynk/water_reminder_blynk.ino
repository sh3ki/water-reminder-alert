/*
 * ============================================================
 *  💧 Water Reminder Alert — Version 1: Blynk App Controlled
 * ============================================================
 *  Control your reminder interval from the Blynk mobile app.
 *  Works with Arduino UNO via USB serial bridge (no WiFi needed).
 *  
 *  HOW IT WORKS:
 *    - Open Blynk app → adjust Slider (V0) to set interval in minutes
 *    - When time is up: buzzer beeps + LED flashes
 *    - Press "Test Alert" button (V1) in app to fire immediately
 *    - App display (V2) shows countdown seconds until next alert
 *  
 *  WIRING: See version1_blynk/WIRING.md
 *  
 *  HARDWARE:
 *    - Arduino UNO R3
 *    - Active Buzzer on D8
 *    - Red LED on D9 (via 220Ω resistor to GND)
 *  
 *  LIBRARIES REQUIRED:
 *    - Blynk by Volodymyr Shymanskyy (v0.6.x Legacy)
 *      Install: Sketch → Include Library → Manage Libraries → "Blynk"
 *  
 *  BLYNK APP WIDGETS:
 *    V0 — Slider   : Interval in minutes (Min:1, Max:60, Step:1)
 *    V1 — Button   : Test/Manual trigger (Mode: Push)
 *    V2 — Display  : Countdown to next alert (seconds)
 *  
 *  SETUP STEPS:
 *    1. Paste your Blynk Auth Token below (from email)
 *    2. Upload this sketch to Arduino UNO
 *    3. On PC: run  python -m blynk_serial_usb --port COMX --token YourToken
 *    4. Open Blynk app → your project should go ONLINE (green)
 *    5. Set interval using the slider → reminders begin!
 * ============================================================
 */

// ─── IMPORTANT: PASTE YOUR AUTH TOKEN HERE ───────────────────
#define BLYNK_AUTH_TOKEN   "YourAuthTokenHere"   // ← Replace this!

// ─── BLYNK PRINT (enables Serial debug output from Blynk lib) ─
#define BLYNK_PRINT Serial

#include <BlynkSimpleSerial.h>

// ─── PIN DEFINITIONS ─────────────────────────────────────────
const int PIN_BUZZER = 8;   // Active Buzzer
const int PIN_LED    = 9;   // Red LED (via 220Ω resistor)

// ─── BLYNK VIRTUAL PINS ──────────────────────────────────────
#define VP_INTERVAL   V0    // Slider: reminder interval in minutes
#define VP_TEST       V1    // Button: manual test trigger
#define VP_COUNTDOWN  V2    // Display: seconds until next alert

// ─── ALERT CONFIGURATION ─────────────────────────────────────
const int BUZZ_BEEP_COUNT = 3;    // Beeps per alert
const int BUZZ_ON_MS      = 200;  // Each beep ON time
const int BUZZ_OFF_MS     = 150;  // Gap between beeps

const int LED_FLASH_COUNT = 6;    // LED flashes per alert
const int LED_ON_MS       = 100;  // Each flash ON time
const int LED_OFF_MS      = 100;  // Each flash OFF time

// ─── GLOBAL STATE ────────────────────────────────────────────
int           intervalMinutes    = 30;      // Default: 30 minutes
unsigned long intervalMs         = 30UL * 60UL * 1000UL;
unsigned long lastReminderTime   = 0;
bool          alertPending       = false;   // Set by V1 test button

// Blynk timer for sending countdown to app
BlynkTimer timer;

// ─── BLYNK HANDLER: Interval Slider (V0) ─────────────────────
/*
 * Called by Blynk whenever V0 slider value changes in the app.
 * Updates the reminder interval immediately.
 */
BLYNK_WRITE(VP_INTERVAL) {
  int newInterval = param.asInt();

  // Clamp to valid range (1–60 minutes)
  if (newInterval < 1)  newInterval = 1;
  if (newInterval > 60) newInterval = 60;

  if (newInterval != intervalMinutes) {
    intervalMinutes = newInterval;
    intervalMs = (unsigned long)intervalMinutes * 60UL * 1000UL;

    // Reset timer from now
    lastReminderTime = millis();

    Serial.print("[Blynk] Interval changed to: ");
    Serial.print(intervalMinutes);
    Serial.println(" minute(s)");
  }
}

// ─── BLYNK HANDLER: Test/Manual Trigger Button (V1) ──────────
/*
 * Called when user taps the "Test Alert Now" button in app.
 * Fires an immediate one-shot alert and resets the timer.
 */
BLYNK_WRITE(VP_TEST) {
  int buttonState = param.asInt();
  if (buttonState == 1) {
    Serial.println("[Blynk] Manual test triggered via app!");
    alertPending = true;  // Handled in main loop
  }
}

// ─── BLYNK CONNECTED HANDLER ─────────────────────────────────
/*
 * Called each time Arduino successfully connects to Blynk.
 * Syncs all virtual pins so app shows current values.
 */
BLYNK_CONNECTED() {
  Blynk.syncAll();
  Serial.println("[Blynk] Connected! Syncing app values...");
}

// ─── COUNTDOWN UPDATE (runs every 1 second via BlynkTimer) ───
void sendCountdown() {
  if (!Blynk.connected()) return;

  unsigned long now          = millis();
  unsigned long elapsed      = now - lastReminderTime;
  unsigned long remaining    = 0;

  if (elapsed < intervalMs) {
    remaining = (intervalMs - elapsed) / 1000UL;
  }

  // Format as MM:SS
  unsigned long mm = remaining / 60;
  unsigned long ss = remaining % 60;

  char buf[10];
  sprintf(buf, "%02lu:%02lu", mm, ss);

  Blynk.virtualWrite(VP_COUNTDOWN, buf);
}

// ─── TRIGGER ALERT ───────────────────────────────────────────
void triggerAlert() {
  Serial.println(">>> DRINK WATER! <<<");

  // Buzz + LED simultaneously
  for (int i = 0; i < max(BUZZ_BEEP_COUNT, LED_FLASH_COUNT); i++) {
    if (i < LED_FLASH_COUNT) digitalWrite(PIN_LED, HIGH);
    if (i < BUZZ_BEEP_COUNT) digitalWrite(PIN_BUZZER, HIGH);
    delay(BUZZ_ON_MS);

    digitalWrite(PIN_BUZZER, LOW);
    digitalWrite(PIN_LED, LOW);
    delay(BUZZ_OFF_MS);
  }

  // Extra LED flashes
  for (int i = BUZZ_BEEP_COUNT; i < LED_FLASH_COUNT; i++) {
    digitalWrite(PIN_LED, HIGH);
    delay(LED_ON_MS);
    digitalWrite(PIN_LED, LOW);
    delay(LED_OFF_MS);
  }
}

// ─────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);

  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  // Initialize Blynk via USB serial
  Blynk.begin(BLYNK_AUTH_TOKEN);

  // Timer: send countdown to app every 1 second
  timer.setInterval(1000L, sendCountdown);

  lastReminderTime = millis();

  Serial.println("=== Water Reminder Alert (Blynk) STARTED ===");
  Serial.print("Default interval: ");
  Serial.print(intervalMinutes);
  Serial.println(" minute(s) — change via app slider (V0)");

  // Startup beep
  digitalWrite(PIN_BUZZER, HIGH);
  delay(100);
  digitalWrite(PIN_BUZZER, LOW);
  delay(100);
  digitalWrite(PIN_BUZZER, HIGH);
  delay(100);
  digitalWrite(PIN_BUZZER, LOW);
}

// ─────────────────────────────────────────────────────────────
void loop() {
  Blynk.run();   // Handle Blynk communication
  timer.run();   // Handle countdown updates to app

  // Check for scheduled reminder
  unsigned long now = millis();
  if ((now - lastReminderTime) >= intervalMs) {
    lastReminderTime = now;
    triggerAlert();
  }

  // Handle manual test alert from app button (V1)
  if (alertPending) {
    alertPending = false;
    lastReminderTime = millis();  // Reset timer after manual trigger
    triggerAlert();
  }
}
