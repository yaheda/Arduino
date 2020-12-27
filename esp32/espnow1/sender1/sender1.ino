#include <esp_now.h>
#include <WiFi.h>
//https://randomnerdtutorials.com/esp-now-esp32-arduino-ide/
uint8_t broadcastAddress[] = {0xF0,0x08,0xD1,0xC7,0x65,0x78};

typedef struct struct_message {
  char a[32];
  int b;
  float c;
  String d;
  bool e;
} struct_message;

struct_message myData;

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(onDataSent);

  // Register peer
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

void loop() {
  strcpy(myData.a, "THIS IS A CHAIR");
  myData.b = random(1, 20);
  myData.c = 1.2;
  myData.d = "Hello";
  myData.e = false;

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }
  delay(2000);
}
