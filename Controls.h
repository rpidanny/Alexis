#ifndef SERVICES_H
#define SERVICES_H

#define MQTT_RECONNECT 10000

#ifndef ARDUINO_H
  #include <Arduino.h>
#endif

#ifndef MAX_DEVICES
  #define MAX_DEVICES 5
#endif

#include <WiFiClient.h>
#include <PubSubClient.h>
#include <fauxmoESP.h>

#include "Device.h"

class Controls {
  public:
    Controls();
    void begin(Device* devices, uint8_t count);
    typedef std::function<void(void)> CallbackFunction;
    void handle();
    void onStateChange(CallbackFunction callback);
  private:
    void DEBUG_SER(String msg);
    void mqttCallback(char* topic, byte* payload, unsigned int length);
    void reconnect();
    void addDevice(Device d);
    void setDevice(const char * name, bool state);

    bool _debug = true;
    const char * _chipId;
    long _lastMillis = 0;
    uint8_t _deviceCount = 0;

    WiFiClient _wifiClient;
    PubSubClient *_mqttClient;
    fauxmoESP fauxmo;
    Device _devices[MAX_DEVICES];
};

extern Controls controls;

#endif