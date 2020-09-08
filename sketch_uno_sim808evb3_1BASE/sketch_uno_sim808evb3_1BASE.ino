#include <SoftwareSerial.h>

/// RX,TX of arduino pin not sim module
/// i.e TX -> RX and RX -> TX
SoftwareSerial sim808Module(11,10); 

void setup() {
  sim808Module.begin(9600);
  Serial.begin(9600);
  Serial.print("GPS Tracker\n");
  delay(100);
}

void loop() {
 if (Serial.available() > 0) { /// serial input from COM
  sim808Module.write(Serial.read()); /// read COM and write to sim module
 }

 if (sim808Module.available() > 0) { /// if sim module responds
    Serial.write(sim808Module.read());
  }

}
