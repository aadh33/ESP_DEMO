#define RED_LED     4
#define GREEN_LED   2
#define SW4         35
#define SW5         34
#define SW6         39

bool red_on = false;
bool green_on = false;

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

  // --- SW4: Turn on RED if GREEN is OFF ---
  if (digitalRead(SW4) == LOW && !green_on) {
    red_on = true;
    digitalWrite(GREEN_LED, LOW);
  }

  // --- SW5: Short press -> RED off | Long press -> GREEN off ---
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

    // Always turn off red on press
    red_on = false;
    digitalWrite(RED_LED, LOW);
  } else {
    sw5Pressed = false;
  }

  // --- SW6: On release, turn on GREEN only if RED is OFF ---
  static bool lastSW6State = HIGH;
  bool sw6State = digitalRead(SW6);
  if (lastSW6State == LOW && sw6State == HIGH) {
    if (!red_on && !green_on) {
      green_on = true;
    }
  }
  lastSW6State = sw6State;

  // --- RED LED Blink 10Hz ---
  if (red_on && now - lastRedToggle >= 50) {
    lastRedToggle = now;
    digitalWrite(RED_LED, !digitalRead(RED_LED));
  }

  // --- GREEN LED Blink 2Hz ---
  if (green_on && now - lastGreenToggle >= 250) {
    lastGreenToggle = now;
    digitalWrite(GREEN_LED, !digitalRead(GREEN_LED));
  }
}
