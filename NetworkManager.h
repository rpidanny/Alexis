#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#ifdef DEBUG_ESP_PORT
  #define DEBUG_MSG(...) DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#else
  #define DEBUG_MSG(...)
#endif

#ifndef OTAMANAGER_H
  #include "OTAManager.h"
#endif

#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

class NetworkManager {
  public:
    void begin();
    void handle();
  private:
    void DEBUG_NM(String msg);

    bool _debug = true;
    uint16_t _delay = 3000;
};

extern NetworkManager Network;

#endif