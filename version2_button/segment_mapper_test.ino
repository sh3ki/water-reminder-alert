/*
 * ============================================================
 *  Raw Pin Mapper Test (7-seg + external LED)
 * ============================================================
 *  Purpose:
 *    - Press button once to advance one test step.
 *    - Each step drives exactly ONE output pin.
 *    - You report what lights: top, upperleft, bottomleft,
 *      center, upperright, bottomright, bottom, period,
 *      external LED, or none.
 *
 *  This test is intentionally raw-pin based so mapping is exact.
 *
 *  Serial Monitor: 9600 baud
 * ============================================================
 */

const int PIN_BUTTON = 2;

// Candidate outputs that may be connected to 7-seg segments or external LED.
const int TEST_PINS[] = {3, 4, 5, 6, 7, 8, 9, 11};
const int TEST_PIN_COUNT = sizeof(TEST_PINS) / sizeof(TEST_PINS[0]);

// true = write HIGH to turn active output on (common cathode typical)
// false = write LOW to turn active output on (common anode typical)
const bool ACTIVE_HIGH = true;

const unsigned long DEBOUNCE_MS = 50;

// Steps: D3,D4,D5,D6,D7,D8,D9,D11,ALL_OFF
const int STEP_COUNT = TEST_PIN_COUNT + 1;

int currentStep = 0;
bool buttonWasPressed = false;

void drivePin(int pin, bool on) {
  int onLevel = ACTIVE_HIGH ? HIGH : LOW;
  int offLevel = ACTIVE_HIGH ? LOW : HIGH;
  digitalWrite(pin, on ? onLevel : offLevel);
}

void clearAll() {
  for (int i = 0; i < TEST_PIN_COUNT; i++) {
    drivePin(TEST_PINS[i], false);
  }
}

void showStep() {
  clearAll();

  Serial.print("Step ");
  Serial.print(currentStep);
  Serial.print(" -> ");

  if (currentStep < TEST_PIN_COUNT) {
    int pin = TEST_PINS[currentStep];
    drivePin(pin, true);
    Serial.print("D");
    Serial.println(pin);
  } else {
    Serial.println("ALL_OFF");
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
      currentStep = (currentStep + 1) % STEP_COUNT;
      showStep();
    } else if (rawState == HIGH) {
      buttonWasPressed = false;
    }
  }

  lastRawState = rawState;
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_BUTTON, INPUT_PULLUP);

  for (int i = 0; i < TEST_PIN_COUNT; i++) {
    pinMode(TEST_PINS[i], OUTPUT);
  }

  clearAll();

  Serial.println("=== Raw Pin Mapper Test ===");
  Serial.println("Press button to advance.");
  Serial.println("Steps: D3,D4,D5,D6,D7,D8,D9,D11,ALL_OFF");
  Serial.println("Report which physical position lights for each pin.");

  // Start at step 0 so first visible test is D3.
  currentStep = 0;
  showStep();
}

void loop() {
  handleButton();
}
