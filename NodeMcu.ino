#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "YourAuthToken"; // Your Blynk auth token
char ssid[] = "YourNetworkName"; // Your network SSID (name)
char pass[] = "YourPassword"; // Your network password

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
}

void loop() {
  if (Serial.available()) {
    String status = Serial.readStringUntil('\n');
    Blynk.virtualWrite(V1, status); // Send status to Blynk app
  }
  Blynk.run();
}
