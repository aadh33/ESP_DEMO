#define TOUCH_PIN 15  //
#define THRESHOLD 30  // Adjust based on sensitivity needed

void setup() {
  Serial.begin(115200);
  Serial.println("Touch sensor test starting...");
  delay(3000);
  Serial.println("Touch sensor 1");
  
}

void loop() {
  int touchValue = touchRead(TOUCH_PIN);
 
  // Detect finger press (lower value = touch detected)
  if (touchValue < THRESHOLD) {
    Serial.println("🔵 sensor is touched");
  }

  delay(200);
}