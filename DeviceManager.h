#ifndef DeviceMANAGER_H
#define DeviceMANAGER_H

#include <fauxmoESP.h>
#include <EEPROM.h>

#define DEVICES 5
#define NAME_LENGTH 20
#define SIZE sizeof(Device) * DEVICES // Maxing at 5 devices for now

struct Device {
  uint8_t pin;
  char name[NAME_LENGTH];
};

class DeviceManager {
  public:
    void begin();
    void handle();
  private:
    void DEBUG_DM(String msg);
    uint8_t readROM(uint8_t addr);
    void writeROM(uint8_t addr, uint8_t data);

    bool _debug = true;
    fauxmoESP fauxmo;
};

extern DeviceManager DM;

#endif