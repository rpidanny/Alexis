#include "OTAManager.h"

void OTAManager::DEBUG_OTA(String msg) {
  if (_debug) {
    Serial.print("*OTA: ");
    Serial.println(msg);
  }
}

void OTAManager::begin() {
  ArduinoOTA.setHostname(_host);
  ArduinoOTA.setPasswordHash(_password);

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Ota.DEBUG_OTA("Starting Update: " + type);
    // Serial.println("Start updating " + type);
  });

  ArduinoOTA.onEnd([]() { // do a fancy thing with our board led at end
    Ota.DEBUG_OTA("Update Completed.");
    // Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });

  /* setup the OTA server */
  ArduinoOTA.begin();
  DEBUG_OTA("Ready");
}

void OTAManager::handle() {
  ArduinoOTA.handle();
}
OTAManager Ota;