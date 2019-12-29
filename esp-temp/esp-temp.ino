#include <DHT.h>
#include <ESP8266WiFi.h>

#include "wifi_code.h"

String apiKey = API_KEY;
const char *ssid = WIFI_SSID;
const char *pass = WIFI_PASS;
const char* server = "api.thingspeak.com";

#define SLEEP_TIME  (60)  // time to stay in deep sleep
#define DHTPIN      0     // pin of the dht11
#define LED_UPLINK  D4    // led to use

DHT dht(DHTPIN, DHT11);

WiFiClient client;
char buff_t[10];
char buff_h[10];
 
void setup() {
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while(!Serial) { }
  pinMode(LED_UPLINK, OUTPUT);
  digitalWrite(LED_UPLINK, HIGH);
  dht.begin();
 
  Serial.printf("Connecting to %s\n", ssid);
  WiFi.begin(ssid, pass);
 
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(650);
  }
  Serial.println("");
  Serial.println("WiFi connected");

  double h = dht.readHumidity();
  double t = dht.readTemperature();
      
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");

  } else {
    if (client.connect(server,80)) {
      digitalWrite(LED_UPLINK, LOW);
      String postStr = apiKey;
      postStr +="&field1=";
      dtostrf(t, 4, 6, buff_t);
      postStr += buff_t;
      postStr +="&field2=";
      dtostrf(h, 4, 6, buff_h);
      postStr += buff_h;
      postStr += "\r\n\r\n";

      client.print("POST /update HTTP/1.1\n");
      client.print("Host: api.thingspeak.com\n");
      client.print("Connection: close\n");
      client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
      client.print("Content-Type: application/x-www-form-urlencoded\n");
      client.print("Content-Length: ");
      client.print(postStr.length());
      client.print("\n\n");
      client.print(postStr);

      Serial.print("T: ");
      Serial.print(buff_t);      
      Serial.print(" H: ");
      Serial.println(buff_h);
      digitalWrite(LED_UPLINK, HIGH);
    }

    client.stop();
    Serial.println("Waiting...");
  }

  Serial.println("ESP8266 in sleep mode");
  ESP.deepSleep(SLEEP_TIME * 1000000);
}

void loop () {

}
