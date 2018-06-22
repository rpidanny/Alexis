#ifndef SERVICES_H
#define SERVICES_H

#ifndef ARDUINO_H
  #include <Arduino.h>
#endif

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

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
    bool _debug = true;

    PubSubClient client;
};

extern Services Ser;

#endif