#ifndef DeviceMANAGER_H
#define DeviceMANAGER_H

#include <fauxmoESP.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>

#define DEVICE_COUNT_ADDR 0  // address 0 of EEPROM
#define MAX_DEVICES 5
#define NAME_LENGTH 20
#define SIZE sizeof(Device) * MAX_DEVICES // Maxing at 5 devices for now

typedef struct {
  uint8_t pin;
  bool state;
  char name[NAME_LENGTH];
} Device;

class DeviceManager {
  public:
    void begin();
    void handle();
    bool addDevice(uint8_t pin, const char * name);
    void printDevices();
    void clearDevices();
  private:
    void DEBUG_DM(String msg);
    uint8_t readROM(uint8_t addr);
    void writeROM(uint8_t addr, uint8_t data);
    bool deviceExists(const char * name);

    void setupServer();
    // requestHandlers
    void addDeviceHander();
    void clearDevicesHandler();

    bool _debug = true;
    uint8_t _deviceCount;
    Device _devices[MAX_DEVICES];
    fauxmoESP fauxmo;
    ESP8266WebServer server;
};

extern DeviceManager DM;

#endif