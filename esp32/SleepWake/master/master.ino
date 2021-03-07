#include <esp_now.h>
#include <WiFi.h>

#define uS_TO_S_FACTOR 1000000  //Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  15        //Time ESP32 will go to sleep (in seconds)

RTC_DATA_ATTR int bootCount = 0;

const int BAUD_RATE = 115200;
int ledPin = 2;

typedef struct esp_now_message {
  bool masterGoingToSleep;
  int slaveSleepTime;
  uint8_t slaveAddress[];
} esp_now_message;

esp_now_message espNowMessage;

uint8_t broadcastAddress[] = {0xF0,0x08,0xD1,0xC7,0x65,0x78};

RTC_DATA_ATTR bool isFirstBoot = true;

void setup()
{
  Serial.begin(BAUD_RATE);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(onDataSent);
  esp_now_register_recv_cb(onDataRecv);

  delay(1000);

  pinMode(ledPin, OUTPUT);

  //++bootCount;
  //Serial.println("Boot number: " + String(bootCount));

  registerPeer(broadcastAddress);

  /// MASTER - tell peers i'm goin to sleep
  if (isFirstBoot == true) {
    isFirstBoot = false;
    goToSleep();    
    //esp_deep_sleep_start();
  }
  
  
  

  //Go to sleep now
  //esp_deep_sleep_start();
  // start delay
  delayStart = millis();
  delayRunning = true;
}

unsigned long DELAY_TIME = 10000; // 10 sec
unsigned long delayStart = 0; // the time the delay started
bool delayRunning = false; // true if still waiting for delay to finish
int loopCount = 0;

void loop()
{
  /*digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay(100);*/

  delay(1000);

  //if (isFirstBoot == true) {
  //  isFirstBoot = false;
  //  esp_deep_sleep_start();
  //}
  
  loopCount++;
  

  if (loopCount > 20) {
    esp_deep_sleep_start();
  }

  
}

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void onDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&espNowMessage, incomingData, sizeof(espNowMessage));
  // update counter
}

void registerPeer(uint8_t broadcastAddress[]) {
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void goToSleep() {
  espNowMessage.masterGoingToSleep = true;
  espNowMessage.slaveSleepTime = 20;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &espNowMessage, sizeof(espNowMessage));
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }

  //Set timer to 5 seconds
  esp_sleep_enable_timer_wakeup((espNowMessage.slaveSleepTime - 10) * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(espNowMessage.slaveSleepTime - 10) + " Seconds");

  esp_deep_sleep_start();
}
