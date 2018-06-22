#include "DeviceManager.h"

void DeviceManager::DEBUG_DM(String msg) {
  if (_debug) {
    Serial.print("*DM: ");
    Serial.println(msg);
  }
}

void DeviceManager::begin() {
  DEBUG_DM("Before callback");
  Ser.addDevice("Info Handler",[](){
    DM.DEBUG_DM(F("Prinint from callback: "));
  });
  Ser.begin();

  pinMode(CONFIG_PIN, INPUT);
  EEPROM.begin(SIZE);
  _deviceCount = readROM(DEVICE_COUNT_ADDR);
  if (_deviceCount > 5 ) {
    // reset to 0
    writeROM(DEVICE_COUNT_ADDR, 0);
    _deviceCount = 0;
  }
  if (_deviceCount == 0) {
    // enter configuration mode
    _config = true;
    startConfigServer();
  } else {
    // Load devices from EEPROM
    for (uint8_t i = 0; i < _deviceCount; i++) {
      Device d;
      EEPROM.get((i * sizeof(Device)) + 1, d);
      _devices[i] = d;
      pinMode(d.pin, OUTPUT);
      digitalWrite(d.pin, d.state);
      fauxmo.addDevice(d.name); 
    }
    fauxmo.enable(true);
    // TODO: GPIO controls
    fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state) {
        Serial.printf("[fauxmo] Device #%d (%s) state: %s\n", device_id, device_name, state ? "ON" : "OFF");
    });
    fauxmo.onGetState([](unsigned char device_id, const char * device_name) {
        return true; // whatever the state of the device is
    });
  }
  _apName = WiFi.SSID();
  // Print loaded devices
  if (_debug)
    DM.printDevices();
}

void DeviceManager::handle() {
  fauxmo.handle();
  if (_config)
    server.handleClient();

  // TODO: start config server on long btn press
  if (digitalRead(CONFIG_PIN) == LOW) {
    //Button pressed
    if (millis() - _buttonTimer > BTN_PRESS_TIME && !_config) {
      DEBUG_DM("Entering Congiguration Mode");
      _config = true;
      startConfigServer();
    }
  } else {
    //Button not pressed
    _buttonTimer = millis();
  }
}

void DeviceManager::setDebug(bool flag) {
  _debug = flag;
}

uint8_t DeviceManager::readROM(uint8_t addr) {
  return EEPROM.read(addr);
}

void DeviceManager::writeROM(uint8_t addr, uint8_t data) {
  EEPROM.write(addr, data);
  EEPROM.commit();
}

bool DeviceManager::addDevice(uint8_t pin, const char * name) {
  if (_deviceCount < MAX_DEVICES && getDeviceIndex(name) == -1 && getDeviceIndex(pin) == -1) {
    pinMode(pin, OUTPUT);
    Device d;
    d.pin = pin;
    d.state = LOW;
    strcpy(d.name, name);

    _devices[_deviceCount] = d;

    // +1 because first address is used to save device count
    EEPROM.put(_deviceCount * sizeof(Device) + 1, d);
    EEPROM.commit();
    _deviceCount++;
    writeROM(DEVICE_COUNT_ADDR, _deviceCount);
    DEBUG_DM(String(name) + " Added");
    return true;
  }
  return false;
}

void DeviceManager::printDevices() {
  DEBUG_DM("Saved Devices:");
  for (uint8_t i = 0; i < _deviceCount; i++) {
    Device d = _devices[i];
    DEBUG_DM("[" + String(i) + "] " + String(d.name) + " : " + String(d.pin));
  }
}

void DeviceManager::delDevice(const char * name) {
  if (name == NULL) {
    DEBUG_DM("NULL");
    // remove all devices
    writeROM(DEVICE_COUNT_ADDR, 0);
    _deviceCount = 0;
  } else {
    DEBUG_DM(name);
    int8_t idx = getDeviceIndex(name);
    if (idx > -1) {
      for (uint8_t i = idx; i < _deviceCount; i++) {
        Device d = _devices[i+1];
        _devices[i] = d;
        EEPROM.put(i * sizeof(Device) + 1, d);
        EEPROM.commit();
      }
      _deviceCount--;
      writeROM(DEVICE_COUNT_ADDR, _deviceCount);
      DEBUG_DM(String(name) + " Deleted");
    }
  }
}

int8_t DeviceManager::getDeviceIndex(const char * name) {
  int8_t index = -1;
  for (uint8_t i = 0; i < _deviceCount; i++) {
    if (strcmp(_devices[i].name, name) == 0) {
      index = i;
      break;
    }
  }
  return index;
}

int8_t DeviceManager::getDeviceIndex(uint8_t pin) {
  int8_t index = -1;
  for (uint8_t i = 0; i < _deviceCount; i++) {
    if (_devices[i].pin == pin) {
      index = i;
      break;
    }
  }
  return index;
}

void DeviceManager::startConfigServer() {
  DEBUG_DM("Starting Configuration Server");

  server.on("/", HTTP_GET, std::bind(&DeviceManager::rootHandler, this));
  server.on("/del", HTTP_GET, std::bind(&DeviceManager::delDevicesHandler, this));
  server.on("/devices", HTTP_GET, std::bind(&DeviceManager::listDevicesHandler, this));
  server.on("/info", HTTP_GET, std::bind(&DeviceManager::infoHandler, this));
  server.on("/rs", HTTP_GET, std::bind(&DeviceManager::restartHander, this));

  server.on("/add", HTTP_POST, std::bind(&DeviceManager::addDeviceHander, this));
  server.onNotFound(std::bind(&DeviceManager::notFoundHander, this));

  server.begin();
  DEBUG_DM("HTTP Server Started");
}

// requestHandlers

/** Handle the info page */
void DeviceManager::infoHandler() {
  DEBUG_DM("[Handler] Info");

  String page = FPSTR(HTML_HEAD);
  page.replace("{v}", "Info");
  page += FPSTR(HTML_STYLE);
  page += FPSTR(HTML_HEAD_END);
  page += FPSTR(HTML_HEADER);
  page.replace("{v}", "System Info");
  page += F("<ul>");
  page += F("<li><span class=\"name\">Chip ID</span>: ");
  page += ESP.getChipId();
  page += F("</li>");
  page += F("<li><span class=\"name\">Flash Chip ID</span>: ");
  page += ESP.getFlashChipId();
  page += F("</li>");
  page += F("<li><span class=\"name\">IDE Flash Size</span>:  ");
  page += ESP.getFlashChipSize();
  page += F(" bytes</li>");
  page += F("<li><span class=\"name\">Real Flash Size</span>: ");
  page += ESP.getFlashChipRealSize();
  page += F(" bytes</li>");
  page += F("<li><span class=\"name\">Device IP</span>: ");
  page += WiFi.localIP().toString();
  page += F("</li>");
  page += F("<li><span class=\"name\">Soft AP MAC</span>: ");
  page += WiFi.softAPmacAddress();
  page += F("</li>");
  page += F("<li><span class=\"name\">Station MAC</span>: ");
  page += WiFi.macAddress();
  page += F("</li>");
  page += F("</ul>");
  page += FPSTR(HTML_BACK);
  page += FPSTR(HTML_END);

  server.send(200, "text/html", page);
}

void DeviceManager::addDeviceHander() {
  DEBUG_DM("[Handler] Add Device");

  if (!server.hasArg("pin") || !server.hasArg("name")
    || server.arg("pin") == NULL || server.arg("name") == NULL) {
      server.send(400, "text/plain", "400: Invalid Request");
    }
  else {
    if (addDevice(server.arg("pin").toInt(), server.arg("name").c_str())) {
      server.sendHeader("Location", String("/devices"), true);
      server.send ( 302, "text/plain", "");
    }
    else
      server.send(500, "text/plain", "500: Can't Add Device");
  }
}

void DeviceManager::delDevicesHandler() {
  DEBUG_DM("[Handler] Del Device");

  if (server.hasArg("name")) {
    delDevice(server.arg("name").c_str());
    server.sendHeader("Location", String("/devices"), true);
  } else {
    delDevice();
    server.sendHeader("Location", String("/"), true);
  }
  server.send ( 302, "text/plain", "");
}

void DeviceManager::listDevicesHandler() {
  DEBUG_DM("[Handler] List Devices");

  String _customHeadElement = "";
  
  String page = FPSTR(HTML_HEAD);
  page.replace("{v}", "Devices");
  page += FPSTR(HTML_CONFIRM_SCRIPT);;
  page += FPSTR(HTML_DEVICES_SCRIPT);
  page += FPSTR(HTML_STYLE);
  page += _customHeadElement;
  page += FPSTR(HTML_HEAD_END);
  page += FPSTR(HTML_HEADER);
  page.replace("{v}", "Devices");
  page += "<div> <ul>";
  for (uint8_t i = 0; i < _deviceCount; i++) {
    String temp = FPSTR(HTML_DEVICE_LIST);
    temp.replace("{d}", _devices[i].name);
    temp.replace("{p}", String(_devices[i].pin));
    page += temp;
  }
  // Dont allow device additon if max devices reached
  if (_deviceCount < MAX_DEVICES) {
    page += "<br/><div>";
    page += FPSTR(HTML_FORM_ADD_DEV);
    page += "</br></br><input type=\"button\" class=\"addDevice\" onClick=\"confSubmit(this.form);\" value=\"Add\" ></form>";
    page += "</div>";
  }
  page += FPSTR(HTML_BACK);
  page += FPSTR(HTML_END);

  server.send(200, "text/html", page);
}

void DeviceManager::rootHandler() {
  DEBUG_DM("[Handler] Root");
  
  String page = FPSTR(HTML_HEAD);
  page.replace("{v}", "Configuration");
  page += FPSTR(HTML_CONFIRM_SCRIPT);;
  page += FPSTR(HTML_STYLE);
  page += FPSTR(HTML_HEAD_END);
  page += FPSTR(HTML_HEADER);
  page.replace("{v}", "Configuration");
  page += FPSTR(HTML_PORTAL_OPTIONS);
  page += FPSTR(HTML_END);

  server.send(200, "text/html", page);
}

void DeviceManager::notFoundHander() {
  DEBUG_DM("[Handler] Not Found");

  server.send(404, "text/plain", "404: Hmm, looks like that page doesn't exist");
}

void DeviceManager::restartHander() {
  String page = FPSTR(HTML_REDIRECT);
  page.replace("{v}", "Restarting....");
  DM.server.send ( 200, "text/html", page);
  // wait for server to send HTTP respose before restarting
  delay(500);
  ESP.restart();
}

DeviceManager DM;