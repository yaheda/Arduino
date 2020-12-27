#include "WiFi.h"

void setup() {
  
  Serial.begin(115200);
  Serial.println("Setup zabi");
  WiFi.mode(WIFI_MODE_STA);
  Serial.println(WiFi.macAddress());
}

void loop() {
  // put your main code here, to run repeatedly:

}
