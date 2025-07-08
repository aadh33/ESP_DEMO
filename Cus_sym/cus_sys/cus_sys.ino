// GPIO definitions
#define RED_LED     4
#define GREEN_LED   2
#define SW4         35
#define SW5         34
#define SW6         39

// LED states
bool red_on = false;
bool green_on = false;
bool red_was_active = false;  // Tracks if red was active before

// Timing
unsigned long lastRedToggle = 0;
unsigned long lastGreenToggle = 0;
unsigned long sw5PressTime = 0;
bool sw5Pressed = false;

void setup() {
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);

  pinMode(SW4, INPUT);
  pinMode(SW5, INPUT);
  pinMode(SW6, INPUT);
}

void loop() {
  unsigned long now = millis();

  // SW4: Start red LED (10Hz)
  if (digitalRead(SW4) == LOW) {
    red_on = true;
    green_on = false;
    red_was_active = true;
    digitalWrite(GREEN_LED, LOW);
  }

  // SW5: Short press turns off red; long press turns off green
  if (digitalRead(SW5) == LOW) {
    if (!sw5Pressed) {
      sw5Pressed = true;
      sw5PressTime = now;
    } else {
      if (now - sw5PressTime >= 2000) {
        green_on = false;
        digitalWrite(GREEN_LED, LOW);
      }
    }

    // Short press always turns off red
    red_on = false;
    digitalWrite(RED_LED, LOW);
  } else {
    sw5Pressed = false;
  }

  // SW6: On release, green LED (2Hz) starts ONLY IF red was previously active and now off
  static bool lastSW6State = HIGH;
  bool sw6State = digitalRead(SW6);
  if (lastSW6State == LOW && sw6State == HIGH) {
    if (!red_on && red_was_active) {
      green_on = true;
      red_was_active = false; // Reset so green turns on only once after red
    }
  }
  lastSW6State = sw6State;

  // Red LED 10Hz
  if (red_on && now - lastRedToggle >= 100) {
    lastRedToggle = now;
    digitalWrite(RED_LED, !digitalRead(RED_LED));
  }

  // Green LED 2Hz
  if (green_on && now - lastGreenToggle >= 500) {
    lastGreenToggle = now;
    digitalWrite(GREEN_LED, !digitalRead(GREEN_LED));
  }
}
