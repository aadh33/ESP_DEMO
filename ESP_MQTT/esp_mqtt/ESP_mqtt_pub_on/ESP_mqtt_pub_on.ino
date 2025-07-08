#include <WiFi.h>
#include <PubSubClient.h>

// Wi-Fi credentials
const char* ssid = "yasin";
const char* password = "yaaz123456";

// MQTT broker details
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* mqttClientID = "ESP32Client_12345";
const char* mqttTopic = "test/adhmj";

WiFiClient espClient;
PubSubClient client(espClient);

// GPIO pins
const int SWITCH_PIN = 34;      // SW5
const int SW6_PIN = 39;         // SW6
const int RED_LED = 4;
const int BLUE_LED = 13;
const int GREEN_LED = 2;

// SW5 debounce
bool lastSwitchReading = HIGH;
bool switchState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// SW6
bool lastSw6Reading = HIGH;
unsigned long sw6PressedTime = 0;
bool sw6IsPressed = false;

// === MQTT callback ===
void callback(char* topic, byte* message, unsigned int length) {
  String msg = "";
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)message[i];
  }

  Serial.print("📥 MQTT Message: ");
  Serial.println(msg);

  if (msg == "RED ON") {
    digitalWrite(RED_LED, HIGH);
    Serial.println("🟥 RED turned ON via MQTT");
  } else if (msg == "BLUE ON") {
    digitalWrite(BLUE_LED, HIGH);
    Serial.println("🔵 BLUE turned ON via MQTT");
  } else if (msg == "GREEN ON") {
    digitalWrite(GREEN_LED, HIGH);
    Serial.println("🟢 GREEN turned ON via MQTT");
  } else if (msg == "ALL OFF") {
    digitalWrite(RED_LED, LOW);
    digitalWrite(BLUE_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    Serial.println("🛑 All LEDs OFF via MQTT");
  }
}

void setup_wifi() {
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Wi-Fi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect_mqtt() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker... ");
    if (client.connect(mqttClientID)) {
      Serial.println("✅ Connected!");
      client.subscribe(mqttTopic);
    } else {
      Serial.print("❌ Failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 2 seconds...");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(SWITCH_PIN, INPUT);
  pinMode(SW6_PIN, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  // Ensure LEDs are off initially
  digitalWrite(RED_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(GREEN_LED, LOW);

  setup_wifi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect_mqtt();
  }
  client.loop();

  // === SW5: All OFF ===
  int reading = digitalRead(SWITCH_PIN);
  if (reading != lastSwitchReading) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != switchState) {
      switchState = reading;
      if (switchState == LOW) {
        digitalWrite(RED_LED, LOW);
        digitalWrite(BLUE_LED, LOW);
        digitalWrite(GREEN_LED, LOW);
        Serial.println("🔘 SW5 Pressed -> All LEDs OFF");
        client.publish(mqttTopic, "All LEDs OFF (SW5 press)");
      } else {
        Serial.println("🔄 SW5 Released");
        client.publish(mqttTopic, "SW5 released");
      }
    }
  }
  lastSwitchReading = reading;

  // === SW6: RED/BLUE/GREEN ON based on hold ===
  int sw6Reading = digitalRead(SW6_PIN);
  if (sw6Reading == LOW && lastSw6Reading == HIGH) {
    sw6PressedTime = millis();
    sw6IsPressed = true;
  }

  if (sw6Reading == HIGH && lastSw6Reading == LOW) {
    sw6IsPressed = false;
    unsigned long heldTime = millis() - sw6PressedTime;

    if (heldTime < 2000) {
      digitalWrite(RED_LED, HIGH);
      Serial.println("🟥 RED ON (SW6 short press)");
      client.publish(mqttTopic, "RED LED ON (SW6 short press)");
    } else if (heldTime >= 2000 && heldTime < 4000) {
      digitalWrite(BLUE_LED, HIGH);
      Serial.println("🔵 BLUE ON (SW6 long press 2s)");
      client.publish(mqttTopic, "BLUE LED ON (SW6 long press 2s)");
    } else if (heldTime >= 4000) {
      digitalWrite(GREEN_LED, HIGH);
      Serial.println("🟢 GREEN ON (SW6 long press 4s)");
      client.publish(mqttTopic, "GREEN LED ON (SW6 long press 4s)");
    }
  }

  lastSw6Reading = sw6Reading;
}
