#include <WiFi.h>
#include <WebServer.h>
#include "ThingsBoard.h"

#include <LinkedList.h>

#include "MotionHelper.h"

/// https://lastminuteengineers.com/creating-esp32-web-server-arduino-ide/

#define TOKEN               "AwziTZpTlU4fpgpd5kg4"
#define THINGSBOARD_SERVER  "demo.thingsboard.io"
#define THINGSBOARD_PORT    80

WiFiClient client;
ThingsBoardHttp tb(client, TOKEN, THINGSBOARD_SERVER, THINGSBOARD_PORT);

/* Put your SSID & Password */
const char* ssid = "ESP32";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

String wifiSsid = "SouleCottage";
String wifiPassword = "0790527264";

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);

uint8_t LED1pin = LED_BUILTIN;
bool LED1status = LOW;


const int timeSeconds = 1;
const int motionLedPin = LED_BUILTIN;
const int motionSensorPin = 27;

unsigned long now = millis();
unsigned long lastMotionTrigger = 0;
boolean startMotionTimer = false;

LinkedList<int> motionList = LinkedList<int>();
int currentMotions = 0;
unsigned long lastMotionListAddition = 0;

void IRAM_ATTR detectsMovement() {
  Serial.println("Motion Detected!!");
  digitalWrite(motionLedPin, HIGH);
  startMotionTimer = true;
  lastMotionTrigger = millis();

  currentMotions++;
  //WifiHelper::init();
}

void setupMotion() {
  pinMode(motionSensorPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(motionSensorPin), detectsMovement, RISING);

  pinMode(motionLedPin, OUTPUT);
  digitalWrite(motionLedPin, LOW);
}

void pingMotion() {
  now = millis();

  if (lastMotionTrigger && (now - lastMotionTrigger > (timeSeconds * 1000))) {
    Serial.println("Motion Stopped");
    digitalWrite(motionLedPin, LOW);
    lastMotionTrigger = false;

    
  }

  if (lastMotionListAddition && (now - lastMotionTrigger > (timeSeconds * 1000 * 15))) {
    Serial.println("New List Item");

    if (tb.connected()) {
      tb.sendTelemetryFloat("motions", currentMotions);
      tb.sendTelemetryFloat("motionsStep", currentMotions);
      currentMotions = 0;
    }

    //myList.add(currentMotions);
  }

  delay(1000);
}


void setup() {
  Serial.begin(115200);

  //EEPROM.begin(512);

  pinMode(LED1pin, OUTPUT);

  setupWifi();
  
  randomSeed(analogRead(0));

  setupMotion();
}

void loop() {
  //server.handleClient();
  //testTelemetry();
  //delay(1000);

  pingMotion();
}

void testTelemetry() {
  Serial.println("Sending telemetry data...");
  tb.sendTelemetryFloat("holgram", random(0, 15));
  delay(1000);
}

void setupWifi() {
  Serial.println("Disconnecting current wifi connection");
  WiFi.disconnect();

  Serial.print("SSID: ");
  Serial.println(wifiSsid);
  Serial.print("PASS: ");
  Serial.println(wifiPassword);

  Serial.println("WiFi.begin");
  WiFi.begin(wifiSsid.c_str(), wifiPassword.c_str());

  handleWifi();
}

bool testWifi() {
  int c = 0;
  Serial.println("Waiting for Wifi to connect");
  while ( c < 20 ) {
    if (WiFi.status() == WL_CONNECTED) {
      return true;
    }
    delay(500);
    Serial.print("*");
    c++;
  }
  Serial.println("");
  Serial.println("Connect timed out, opening AP");
  return false;
}

void handleWifi() {
  if (testWifi()) {
    Serial.print("Connected to ");
    Serial.print(wifiSsid);
    Serial.println(" Successfully");
  } else {
    setupAccessPoint();

    Serial.println("Waiting.");
    while ((WiFi.status() != WL_CONNECTED)) {
      Serial.print(".");
      delay(100);
      server.handleClient();
    }
  }
}

void setupAccessPoint() {
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  server.on("/", handle_OnConnect);
  server.on("/led1on", handle_led1on);
  server.on("/led1off", handle_led1off);
  server.on("/settings", handleSettings);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void handleSettings() {
  String qsid = server.arg("ssid");
  String qpass = server.arg("pass");

  Serial.print("qsid: ");
  Serial.println(qsid);

  Serial.print("pass: ");
  Serial.println(qpass);

  String message = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";

  server.send(200, "application/json", message); 
}

void handle_OnConnect() {
  LED1status = LOW;
  Serial.println("GPIO4 Status: OFF | GPIO5 Status: OFF");
  server.send(200, "text/html", SendHTML(LED1status)); 
}

void handle_led1on() {
  LED1status = HIGH;
  Serial.println("GPIO4 Status: ON");
  server.send(200, "text/html", SendHTML(true)); 
}

void handle_led1off() {
  LED1status = LOW;
  Serial.println("GPIO4 Status: OFF");
  server.send(200, "text/html", SendHTML(false)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(uint8_t led1stat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>LED Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #3498db;}\n";
  ptr +=".button-on:active {background-color: #2980b9;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>ESP32 Web Server</h1>\n";
  ptr +="<h3>Using Access Point(AP) Mode</h3>\n";
  
   if(led1stat)
  {ptr +="<p>LED1 Status: ON</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";}
  else
  {ptr +="<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";}

  ptr += "<form method='get' action='settings'><label>SSID: </label><input name='ssid' length=32><input name='pass' length=64><input type='submit'></form>";
 

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}