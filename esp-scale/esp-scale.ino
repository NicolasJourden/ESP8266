#include <Wire.h>
#include <dummy.h>
#include <HX711.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#include "wifi_code.h"

#ifndef WIFI_SSID
#error "SSID required."
#endif
#ifndef WIFI_PASS
#error "Key required."
#endif
#ifndef DEF_URL
#error "Error API key required."
#endif

// Init clock: DATA. CLK
HX711 scale(D6, D5);

/**
 * Setup the ESP8266.
 */
void setup() {
  // Led off:
  pinMode(D0, OUTPUT); 
  digitalWrite(D0, HIGH);

  // Serial:
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while(!Serial) { }

  // Wi-Fi:
  Serial.printf("Connecting to %s\n", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(650);
  }
  Serial.println("WiFi connected");

  // Scale:
  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());      // print a raw reading from the ADC
  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));   // print the average of 20 readings from the ADC
  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight (not set yet)
  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);  // print the average of 5 readings from the ADC minus tare weight (not set) divided 
  Serial.println("Initializing scale ...");
  scale.set_scale(-3013.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();
}

/**
 * Send to ThingSpeak. 
 * sendThingSpeak.
 * @param pValue The read value.
 */
void sendThingSpeak(float pValue)
{
  digitalWrite(D0, LOW);

  // Convert the value to string:
  String str_one = String( abs( pValue ) );
  Serial.print("Value: " );
  Serial.println(pValue);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(650);
  }
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());    

  // HTTP:
  HTTPClient http;
  char buffer[120];
  sprintf(buffer, DEF_URL, str_one.c_str());
  http.begin(buffer);
  int httpCode = http.GET();
  if(httpCode > 0)
  {
    if(httpCode == HTTP_CODE_OK)
    {
      int code = http.getString().toInt();
      Serial.print("Thingspeak ... (");
      Serial.print(code);
      if (code != 0) {
        Serial.println(") done");        
      } else {
        Serial.println(") failed");
      }
    }
  }

  digitalWrite(D0, HIGH);
}

/**
 * Read in the LOOP...
 * 
 */
void loop() {
  // Read the value:
  float weight = scale.get_units(1);
  if (weight < 0) weight = 0;
  Serial.println(String(weight, 2));

  // Send to the internet:
  //sendThingSpeak(weight);

  // Power saving:
  scale.power_down();
  delay(1000);
  scale.power_up();
}
