#define TINY_GSM_MODEM_SIM800

#include <TinyGPS++.h>
#include <TinyGsmClient.h>
#include "ThingsBoard.h"

const int BAUD_RATE = 115200;
const int GPS_BAUD_RATE = 9600;

HardwareSerial SerialGSM(1);
TinyGsm gsm(SerialGSM);
const int GSM_RX_PIN = 32, GSM_TX_PIN = 33;

HardwareSerial SerialGPS(2);
TinyGPSPlus gps;
const int GPS_RX_PIN = 25, GPS_TX_PIN = 26;

const char *APN = "hologram";
const char *USER = "";
const char *PASSWORD = "";

#define TOKEN               "diLPOAyG9AS7YDvKzdbB"
#define THINGSBOARD_SERVER  "demo.thingsboard.io"
#define THINGSBOARD_PORT    80

TinyGsmClient client(gsm);
ThingsBoardHttp tb(client, TOKEN, THINGSBOARD_SERVER, THINGSBOARD_PORT);

bool modemConnected = false;

bool defaultGPS = true;

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println("Starting...");

  setupGSM();
  setupGPS();
}

void loop() {
  if (!modemConnected && !initGSM()) 
    return;

  initGPS();
  sendTelemetry();
}

/// --- GSM ----------------------------------

void setupGSM() {
  SerialGSM.begin(BAUD_RATE, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN, false);
}

bool initGSM() {
  Serial.println("Init GSM");
  modemConnected = false;

  delay(3000);

  Serial.println(gsm.getModemInfo());

  //gsm.init();

  Serial.print("Restarting GSM... ");
  if (!gsm.restart()) {
    Serial.println("Failed");
    delay(10000);
    return false;
  }
  Serial.println("OK");

  Serial.print("Waiting for network... ");
  if (!gsm.waitForNetwork()) {
    Serial.println("Failed");
    delay(10000);
    return false;
  }
  Serial.println("OK");
 
  Serial.print("Connecting to GPRS... ");
  if(!gsm.gprsConnect(APN,USER,PASSWORD)) {
    Serial.println("Failed");
    delay(10000);
    return false;
  }
  Serial.println("OK");
  
  modemConnected = true;
  return true;
}

void powerDownGSM() {
  gsm.poweroff();
}

/// --- GPS ----------------------------------

void setupGPS() {
  SerialGPS.begin(GPS_BAUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN, false);
}

void initGPS() {
  Serial.print("Init GPS... ");
  unsigned long start = millis();
  do 
  {
    while (SerialGPS.available()) 
      gps.encode(SerialGPS.read());
  } while (millis() - start < 1000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }

  Serial.println("OK");
}

/// --- TELEMETRY -------------------------------------

void sendTelemetry() {
  if (!gps.location.isValid() && !defaultGPS) {
    Serial.print("Latitude : ");
    Serial.println("*****");
    Serial.print("Longitude : ");
    Serial.println("*****");

    return;
  }

  double latitude = gps.location.lat();
  double longitude = gps.location.lng();

  if (defaultGPS) {
    latitude = -26.052450;
    longitude = 28.058380;
  }

  Serial.print("Latitude in Decimal Degrees : ");
  Serial.println(latitude, 6);
  Serial.print("Longitude in Decimal Degrees : ");
  Serial.println(longitude, 6);

  //const int data_items = 2;
  //Telemetry data[data_items] = {
  //  { "latitude", -26.052450 },
  //  { "longitude", longitude },
  //};

  Serial.print("Sending Telemetry Data... ");
  //tb.sendTelemetry(data, data_items);
  tb.sendTelemetryFloat("latitude", latitude);
  tb.sendTelemetryFloat("longitude", longitude);
  Serial.println("OK");
}
