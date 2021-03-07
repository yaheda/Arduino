#include <WiFi.h>
#include <esp_now.h>

#define uS_TO_S_FACTOR 1000000

typedef struct esp_now_message {
  bool masterGoingToSleep;
  int slaveSleepTime;
  uint8_t slaveAddress[];
} esp_now_message;

esp_now_message espNowMessage;

uint8_t masterAddress[] = {0xF0,0x08,0xD1,0xC7,0x65,0x78};

RTC_DATA_ATTR bool isFirstBoot = true;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  registerPeer(masterAddress);

  esp_now_register_recv_cb(onDataRecv);

  if (isFirstBoot == true) {
    isFirstBoot = false;
  } else {
    updateMaster();
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:

}

void onDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&espNowMessage, incomingData, sizeof(espNowMessage));
  if (espNowMessage.masterGoingToSleep == true) {
    goToSleep(espNowMessage.slaveSleepTime)
  }
}

void goToSleep(int timerToSleep) {
  esp_sleep_enable_timer_wakeup(timerToSleep * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
  esp_deep_sleep_start();
}

void updateMaster() {
  espNowMessage.slaveAddress = {0xF0,0x08,0xD1,0xC7,0x65,0x78};
  esp_err_t result = esp_now_send(masterAddress, (uint8_t *) &espNowMessage, sizeof(espNowMessage));
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  } 
}

//--------------------------------------------

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
