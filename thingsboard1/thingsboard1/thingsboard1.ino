#define TINY_GSM_MODEM_SIM808

#include <TinyGsmClient.h>
#include <SoftwareSerial.h>
#include "ThingsBoard.h"

const char apn[]  = "internet";
const char user[] = "";
const char pass[] = "";

#define TOKEN               "diLPOAyG9AS7YDvKzdbB"
#define THINGSBOARD_SERVER  "demo.thingsboard.io"
#define THINGSBOARD_PORT    80

#define SERIAL_DEBUG_BAUD   115200

SoftwareSerial serialGsm(11, 10);

//#define DUMP_AT_COMMANDS 1

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(serialGsm, Serial);
  TinyGsm modem(debugger);
#else
  // Initialize GSM modem
  TinyGsm modem(serialGsm);
#endif

TinyGsmClient client(modem);

ThingsBoardHttp tb(client, TOKEN, THINGSBOARD_SERVER, THINGSBOARD_PORT);

bool modemConnected = false;

long randNumber;

void setup() {
  Serial.begin(SERIAL_DEBUG_BAUD);
  Serial.println("-------SETUP------");

  serialGsm.begin(115200);
  delay(3000);

  serialGsm.write("AT+IPR=9600\r\n");
  serialGsm.end();
  serialGsm.begin(9600);

  Serial.println(F("Initializing modem..."));
  modem.restart();

  String modemInfo = modem.getModemInfo();
  Serial.print(F("Modem: "));
  Serial.println(modemInfo);

  randomSeed(analogRead(0));
}

void loop() {
  delay(1000);

  if (!modemConnected) {
    Serial.print(F("Waiting for network..."));
    if (!modem.waitForNetwork()) {
        Serial.println(" fail");
        delay(10000);
        return;
    }
    Serial.println(" OK");

    Serial.print(F("Connecting to "));
    Serial.print(apn);
    if (!modem.gprsConnect(apn, user, pass)) {
        Serial.println(" fail");
        delay(10000);
        return;
    }

    modemConnected = true;
    Serial.println(" OK");
  }

  Serial.println("Enabling GPS/GNSS/GLONASS and waiting 15s for warm-up");
  modem.enableGPS();
  delay(15000L);
  getGPSData();

  

  //Serial.println("Sending temperature data...");
  //tb.sendTelemetryFloat("temperature", random(-20, 20));
}

void getGPSData() {
  Serial.println("Retrieving GPS/GNSS/GLONASS location again as a string");
  
  
  float lat2      = 0;
  float lon2      = 0;
  float speed2    = 0;
  float alt2      = 0;
  int   vsat2     = 0;
  int   usat2     = 0;
  float accuracy2 = 0;
  int   year2     = 0;
  int   month2    = 0;
  int   day2      = 0;
  int   hour2     = 0;
  int   min2      = 0;
  int   sec2      = 0;
  if (modem.getGPS(&lat2, &lon2, &speed2, &alt2, &vsat2, &usat2, &accuracy2,
                    &year2, &month2, &day2, &hour2, &min2, &sec2)) {
      char data[100];

      //DBG("Latitude:", String(lat2, 8), "\tLongitude:", String(lon2, 8));
      sprintf(data, "Latitude: %s \tLongitude: %s", String(lat2).c_str(), String(lon2).c_str());
      Serial.println(data);
      
      //DBG("Speed:", speed2, "\tAltitude:", alt2);
      sprintf(data, "Speed: %s \tAltitude: %s", String(speed2).c_str(), String(alt2).c_str());
      Serial.println(data);
      
      //DBG("Visible Satellites:", vsat2, "\tUsed Satellites:", usat2);
      sprintf(data, "Visible Satellites: %u \tUsed Satellites: %u", vsat2, usat2);
      Serial.println(data);
      
      //DBG("Accuracy:", accuracy2);
      sprintf(data, "Accuracy: %s", String(accuracy2).c_str());
      Serial.println(data);
      
      //DBG("Year:", year2, "\tMonth:", month2, "\tDay:", day2);
      sprintf(data, "Year: %u \tMonth: %u \tDay: ", year2, month2, day2);
      Serial.println(data);
      
      //DBG("Hour:", hour2, "\tMinute:", min2, "\tSecond:", sec2);
      sprintf(data, "Hour: %u \tMinute: %u \tSecond: ", hour2, min2, sec2);
      Serial.println(data);
      
      String gps_raw = modem.getGPSraw();
      Serial.print("GPS/GNSS Based Location String:");
      Serial.println(gps_raw);
      
    } else {
      Serial.println("Couldn't get GPS/GNSS/GLONASS location, retrying in 15s.");
      delay(15000L);
    }
}
