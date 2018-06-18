#include "DeviceManager.h"

void DeviceManager::begin() {
  EEPROM.begin(SIZE);
  Serial.println(SIZE);
  Device test = {
    0,
    "Wemo Switch"
  };
  EEPROM.put(4 * sizeof(Device), test);
  for (uint8_t i =0; i < DEVICES * sizeof(Device); i+= sizeof(Device)) {
    Device d;
    EEPROM.get(i, d);
    Serial.println(d.name);
  }
  fauxmo.addDevice("light one");
  // fauxmo.addDevice("light two");
  // fauxmo.addDevice("light three");
  // fauxmo.addDevice("light four");
  fauxmo.enable(true);

  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state) {
      Serial.printf("[MAIN] Device #%d (%s) state: %s\n", device_id, device_name, state ? "ON" : "OFF");
  });
  fauxmo.onGetState([](unsigned char device_id, const char * device_name) {
      return true; // whatever the state of the device is
  });
}

void DeviceManager::handle() {
  fauxmo.handle();
}

uint8_t DeviceManager::readROM(uint8_t addr) {
  return EEPROM.read(addr);
}

void DeviceManager::writeROM(uint8_t addr, uint8_t data) {
  EEPROM.write(addr, data);
  EEPROM.commit();
}

DeviceManager DM;