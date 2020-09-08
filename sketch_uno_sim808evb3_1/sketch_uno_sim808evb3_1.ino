


/*
#include <SoftwareSerial.h>

/// RX,TX of arduino pin not sim module
/// i.e TX -> RX and RX -> TX
SoftwareSerial sim808Module(11,10); 
*/
//-----------------------------

#define TINY_GSM_MODEM_SIM808

#define SerialMon Serial
//#define SerialAT Serial1
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(11, 10); // RX, TX

#if !defined(TINY_GSM_RX_BUFFER)
#define TINY_GSM_RX_BUFFER 650
#endif

#define DUMP_AT_COMMANDS

#define TINY_GSM_DEBUG SerialMon

#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 9600//115200

#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>

const char apn[]  = "internet";
const char gprsUser[] = "";
const char gprsPass[] = "";

// Server details
const char server[] = "vsh.pp.ua";
const char resource[] = "/TinyGSM/logo.txt";
const int  port = 80;

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

TinyGsmClient client(modem);
HttpClient http(client, server, port);

void setup() {
//serialGsm.write("AT+IPR=9600\r\n");
  //serialGsm.end();
  SerialMon.begin(9600);
  delay(10);

  SerialMon.println("Wait...");
  //TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
  SerialAT.begin(4800);
  delay(6000);

  

  SerialMon.println("Initializing modem...");
  //modem.restart();
  modem.init();

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);
/*
  #if TINY_GSM_USE_GPRS
    // Unlock your SIM card with a PIN if needed
    if ( GSM_PIN && modem.getSimStatus() != 3 ) {
      modem.simUnlock(GSM_PIN);
    }
  #endif*/
}

void loop() {
  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork()) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isNetworkConnected()) {
    SerialMon.println("Network connected");
  }

  // GPRS connection parameters are usually set after network registration
  SerialMon.print(F("Connecting to "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isGprsConnected()) {
    SerialMon.println("GPRS connected");
  }

  useHttpClient();
  //useWebClient();

  modem.gprsDisconnect();
  SerialMon.println(F("GPRS disconnected"));



  // Do nothing forevermore
  while (true) {
    delay(1000);
  }

}

void useHttpClient() {
  SerialMon.print(F("Performing HTTP GET request... "));
  int err = http.get(resource);
  if (err != 0) {
    SerialMon.println(F("failed to connect nn"));
    delay(10000);
    return;
  }

  int status = http.responseStatusCode();
  SerialMon.print(F("Response status code: "));
  SerialMon.println(status);
  if (!status) {
    delay(10000);
    return;
  }

  SerialMon.println(F("Response Headers:"));
  while (http.headerAvailable()) {
    String headerName = http.readHeaderName();
    String headerValue = http.readHeaderValue();
    SerialMon.println("    " + headerName + " : " + headerValue);
  }

  int length = http.contentLength();
  if (length >= 0) {
    SerialMon.print(F("Content length is: "));
    SerialMon.println(length);
  }
  if (http.isResponseChunked()) {
    SerialMon.println(F("The response is chunked"));
  }

  String body = http.responseBody();
  SerialMon.println(F("Response:"));
  SerialMon.println(body);

  SerialMon.print(F("Body length is: "));
  SerialMon.println(body.length());

  // Shutdown

  http.stop();
  SerialMon.println(F("Server disconnected"));
}

void useWebClient() {
  SerialMon.print("Connecting to ");
  SerialMon.println(server);
  if (!client.connect(server, port)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  // Make a HTTP GET request:
  SerialMon.println("Performing HTTP GET request...");
  client.print(String("GET ") + resource + " HTTP/1.1\r\n");
  client.print(String("Host: ") + server + "\r\n");
  client.print("Connection: close\r\n\r\n");
  client.println();

  uint32_t timeout = millis();
  while (client.connected() && millis() - timeout < 10000L) {
    // Print available data
    while (client.available()) {
      char c = client.read();
      SerialMon.print(c);
      timeout = millis();
    }
  }
  SerialMon.println();

  // Shutdown

  client.stop();
  SerialMon.println(F("Server disconnected"));
}
