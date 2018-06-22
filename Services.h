#ifndef SERVICES_H
#define SERVICES_H

#ifndef ARDUINO_H
  #include <Arduino.h>
#endif

#include <WiFiClient.h>
#include <PubSubClient.h>

#define MQTT_RECONNECT 5000

// #ifndef WIFICLIENT
//   WiFiClient wifiClient;
// #endif

class Services {
  public:
    Services();
    void begin();
    typedef std::function<void(void)> CallbackFunction;
    void addDevice(const char * name, CallbackFunction callback);
    void handle();

  private:
    void DEBUG_SER(String msg);
    void mqttCallback(char* topic, byte* payload, unsigned int length);
    void reconnect();
    bool _debug = true;
    const char * _chipId;
    WiFiClient _wifiClient;
    PubSubClient *_mqttClient;
    long _lastMillis = 0;
};

extern Services Ser;

#endif