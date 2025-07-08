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

const int SWITCH_PIN = 34;

// Debounce variables
bool lastSwitchReading = HIGH;
bool switchState = HIGH; // current debounced state
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

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
      Serial.println(" trying again in 2 seconds...");
      delay(2000);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("📥 Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  pinMode(SWITCH_PIN, INPUT); // GPIO 34 doesn't support INPUT_PULLUP
  setup_wifi();

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect_mqtt();
  }
  client.loop();

  int reading = digitalRead(SWITCH_PIN);

  // If state changed, reset debounce timer
  if (reading != lastSwitchReading) {
    lastDebounceTime = millis();
  }

  // If stable for debounceDelay, treat as new state
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != switchState) {
      switchState = reading;

      if (switchState == LOW) {
        Serial.println("🔘 Switch Pressed!");
        client.publish(mqttTopic, "Switch pressed");
      }
    }
  }

  lastSwitchReading = reading;
}