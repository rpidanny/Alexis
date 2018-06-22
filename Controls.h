#ifndef SERVICES_H
#define SERVICES_H

#define MQTT_RECONNECT 5000

#ifndef ARDUINO_H
  #include <Arduino.h>
#endif

#include <WiFiClient.h>
#include <PubSubClient.h>
#include <fauxmoESP.h>

#include "Device.h"

class Controls {
  public:
    Controls();
    void begin();
    typedef std::function<void(void)> CallbackFunction;
    void addDevice(Device d);
    void handle();
    void onStateChange(CallbackFunction callback);
  private:
    void DEBUG_SER(String msg);
    void mqttCallback(char* topic, byte* payload, unsigned int length);
    void reconnect();

    bool _debug = true;
    const char * _chipId;
    long _lastMillis = 0;

    WiFiClient _wifiClient;
    PubSubClient *_mqttClient;
    fauxmoESP fauxmo;
};

extern Controls controls;

#endif