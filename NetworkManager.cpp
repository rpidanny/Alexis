#include "NetworkManager.h"

void NetworkManager::DEBUG_NM(String msg) {
  if (_debug) {
    Serial.print("*NM: ");
    Serial.println(msg);
  }
}

void NetworkManager::begin() {
  // Local initialization
  WiFiManager wifiManager;
  if (!wifiManager.autoConnect()) {
    DEBUG_NM("Failed to connect..");
    ESP.reset();
    delay(_delay);
  }
  DEBUG_NM("Connected to " + WiFi.SSID());
  Ota.begin();
}

void NetworkManager::handle() {
  Ota.handle();
}

NetworkManager Network;