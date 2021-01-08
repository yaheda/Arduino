#include <TinyGPS++.h>

#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>
#include "ThingsBoard.h"

const int BAUD_RATE = 115200; //9600;

HardwareSerial SerialGSM(1);
TinyGsm modemGSM(SerialGSM);
const int RX_PIN = 32, TX_PIN = 33;

HardwareSerial SerialGPS(1);
TinyGPSPlus gps;
const int GPS_RX_PIN = 25, GPS_TX_PIN = 26;

const char apn[]  = "hologram";
const char gprsUser[] = "";
const char gprsPass[] = "";

const char *APN = "hologram";
const char *USER = "";
const char *PASSWORD = "";

bool modemConnected = false;

#define TOKEN               "diLPOAyG9AS7YDvKzdbB"
#define THINGSBOARD_SERVER  "demo.thingsboard.io"
#define THINGSBOARD_PORT    80

TinyGsmClient client(modemGSM);
ThingsBoardHttp tb(client, TOKEN, THINGSBOARD_SERVER, THINGSBOARD_PORT);

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println("Starting...");

  //setupGPS();
  //setupGSM();
}

void loop() {
  delay(5000);
  /*if (!modemConnected) {
    Serial.print(F("Waiting for network..."));
      if (!modemGSM.waitForNetwork()) {
          Serial.println(" fail");
          delay(10000);
          return;
      }
      Serial.println(" OK");
  
      Serial.print(F("Connecting to "));
      Serial.print(apn);
      if (!modemGSM.gprsConnect(APN, USER, PASSWORD)) {
          Serial.println(" fail");
          delay(10000);
          return;
      }
  
      modemConnected = true;
      Serial.println(" OK");
  }*/
  getGPS();
  //sendTelemetry();
}

void getGPS() {
  Serial.println("Setup GPS...");

  SerialGPS.begin(BAUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN, false);
  delay(3000);

  while (SerialGPS.available() > 0){

    byte gpsData = SerialGPS.read();
    Serial.write(gpsData);
  
    /*if (gps.encode(SerialGPS.read())); {
      displayInfo();
      delay(2000);
    }*/
    
    /*if (gps.location.isUpdated()){
      Serial.print("Latitude= "); 
      Serial.print(gps.location.lat(), 6);
      Serial.print(" Longitude= "); 
      Serial.println(gps.location.lng(), 6);
    }*/
  }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
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

void setupGSM() {
  Serial.println("Setup GSM...");

  SerialGSM.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN, false);
  delay(3000);

  Serial.println(modemGSM.getModemInfo());

  //modemGSM.init();

  if (!modemGSM.restart())
  {
    Serial.println("Restarting GSM\nModem failed");
    delay(10000);
    ESP.restart();
    return;
  }
  Serial.println("Modem restart OK");
  
/*
  

  if (!modemGSM.waitForNetwork()) 
  {
    Serial.println("Failed to connect\nto network");
    delay(10000);
    ESP.restart();
    return;
  }
  Serial.println("Modem network OK");
 
  // conecta na rede (tecnologia GPRS)
  if(!modemGSM.gprsConnect(APN,USER,PASSWORD))
  {
    Serial.println("GPRS Connection\nFailed");
    delay(10000);
    ESP.restart();
    return;
  }
  Serial.println("GPRS Connect OK");*/
}

void sendTelemetry() {
  float latitude;
  float longitude;
  float batteryLevel;

  batteryLevel = random(1, 100);
  
  Serial.println("Sending telemetry data...");
  //tb.sendTelemetry(data, data_items);
  tb.sendTelemetryFloat("sim800l", 1);
  tb.sendTelemetryFloat("latitude", latitude);
  tb.sendTelemetryFloat("longitude", longitude);
  tb.sendTelemetryFloat("batteryLevel", batteryLevel);
  Serial.println("Telemetry data sent...");
}
