#include <TinyGPS++.h>

const int BAUD_RATE = 9600;//115200; //9600;

//HardwareSerial SerialGPS(1);

#include <SoftwareSerial.h>

SoftwareSerial gpsSerial(4, 3); // GPS Module’s TX to D4 & RX to D3
TinyGPSPlus gps;
//const int GPS_RX_PIN = 25, GPS_TX_PIN = 26;
const int GPS_RX_PIN = 32, GPS_TX_PIN = 33;

int GPSBaud = 9600;

void setup() {
  Serial.begin(BAUD_RATE);
  gpsSerial.begin(GPSBaud);
  Serial.println("Starting...");
}

void loop() {
  delay(5000);
  getGPS();
}

void getGPS() {
  Serial.println("Setup GPS...");

  //while (gpsSerial.available() > 0)
  //  Serial.write(gpsSerial.read());
    
  while (gpsSerial.available() > 0){
    //byte gpsData = gpsSerial.read();
    //Serial.write(gpsData);

    if (gps.encode(gpsSerial.read())); {
      displayInfo();
      //delay(2000);
    }
  }


  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
    

  /*if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }*/

/*

  SerialGPS.begin(BAUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN, false);
  delay(3000);

  while (SerialGPS.available() > 0){
    byte gpsData = SerialGPS.read();
    Serial.write(gpsData);
  }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }*/
}

void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}
