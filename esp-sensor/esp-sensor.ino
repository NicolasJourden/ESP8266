#include <ESP8266WiFi.h>
#include <ArduinoMqttClient.h>

#include "wifi_code.h"

// Long sleep, LEDs
#define SLEEP_TIME  (60)  // time to stay in deep sleep
#define LED_UPLINK  D4    // led to use
#define BUTTON_GPIO D2

// Clients:
WiFiClient client;
MqttClient mqttClient(client);
  
bool io_last;



void signal() {
  Serial.printf("Connecting to %s\n", SSID_NAME);
  WiFi.begin(SSID_NAME, SSID_PASS);
 
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(650);
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // MQTT
  Serial.printf("Attempting to connect to the MQTT broker: %s:%d\n", MQTT_SERVER, MQTT_SERVERPORT);
  mqttClient.setId(MQTT_ID);
  mqttClient.setUsernamePassword(MQTT_USERNAME, MQTT_PASSWORD);
  do {
    mqttClient.connect(MQTT_SERVER, MQTT_SERVERPORT);
    Serial.print("q");
    delay(650);
  } while (mqttClient.connectError() != 0);
  mqttClient.poll();
 
  // Get I/O status:
  bool io;

  // Different status:
  if (io != io_last)
  {

  }    

  // Publish:
  mqttClient.beginMessage(MQTT_TOPIC);
  mqttClient.print("ON");
  mqttClient.endMessage();

  Serial.print("IO: ");
  Serial.print(io);      
  Serial.print(" Last IO: ");
  Serial.println(io_last);
  digitalWrite(LED_UPLINK, HIGH);

  client.stop();

}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while(!Serial) { }
  pinMode(LED_UPLINK, OUTPUT);
  pinMode(BUTTON_GPIO, INPUT_PULLUP);
  digitalWrite(LED_UPLINK, HIGH);

  puts("Setup ... ");
  Serial.println("ESP8266 in sleep mode");
  ESP.deepSleep(SLEEP_TIME * 1000000);
}

void loop () {
  int val = digitalRead(BUTTON_GPIO);
  printf ("Status: %d\n", val);
  delay(1000);
}
