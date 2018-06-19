#include "DeviceManager.h"

void DeviceManager::DEBUG_DM(String msg) {
  if (_debug) {
    Serial.print("*DM: ");
    Serial.println(msg);
  }
}

void DeviceManager::begin() {
  EEPROM.begin(SIZE);
  _deviceCount = readROM(DEVICE_COUNT_ADDR);
  if (_deviceCount > 5 ) {
    // reset to 0
    writeROM(DEVICE_COUNT_ADDR, 0);
    _deviceCount = 0;
  }

  // Load devices from EEPROM
  for (uint8_t i = 0; i < _deviceCount; i++) {
    Device d;
    EEPROM.get((i * sizeof(Device)) + 1, d);
    _devices[i] = d;
    // TODO: add devices to Fauxmo
  }
  // fauxmo.addDevice("light one");
  // fauxmo.addDevice("light two");
  // fauxmo.addDevice("light three");
  // fauxmo.addDevice("light four");
  // fauxmo.enable(true);

  // fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state) {
  //     Serial.printf("[MAIN] Device #%d (%s) state: %s\n", device_id, device_name, state ? "ON" : "OFF");
  // });
  // fauxmo.onGetState([](unsigned char device_id, const char * device_name) {
  //     return true; // whatever the state of the device is
  // });
  _apName = WiFi.SSID();
  setupServer();
}

void DeviceManager::handle() {
  fauxmo.handle();
  server.handleClient();
}

uint8_t DeviceManager::readROM(uint8_t addr) {
  return EEPROM.read(addr);
}

void DeviceManager::writeROM(uint8_t addr, uint8_t data) {
  EEPROM.write(addr, data);
  EEPROM.commit();
}

bool DeviceManager::addDevice(uint8_t pin, const char * name) {
  if (_deviceCount < MAX_DEVICES && !deviceExists(name)) {
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
    return true;
  }
  return false;
}

void DeviceManager::printDevices() {
  for (uint8_t i = 0; i < _deviceCount; i++) {
    Device d = _devices[i];
    Serial.print(i);
    Serial.print("-->");
    Serial.print(d.pin);
    Serial.print(" : ");
    Serial.println(d.name);
  }
}

void DeviceManager::clearDevices() {
  writeROM(DEVICE_COUNT_ADDR, 0);
  _deviceCount = 0;
}

bool DeviceManager::deviceExists(const char * name) {
  bool flag = false;
  for (uint8_t i = 0; i < _deviceCount; i++) {
    if (strcmp(_devices[i].name, name) == 0) {
      flag = true;
      break;
    }
  }
  return flag;
}

void DeviceManager::setupServer() {
  server.on("/", HTTP_GET, std::bind(&DeviceManager::rootHandler, this));
  server.on("/clear", HTTP_GET, std::bind(&DeviceManager::clearDevicesHandler, this));
  server.on("/devices", HTTP_GET, std::bind(&DeviceManager::listDevicesHandler, this));
  server.on("/info", HTTP_GET, std::bind(&DeviceManager::infoHandler, this));

  server.on("/add", HTTP_POST, std::bind(&DeviceManager::addDeviceHander, this));
  server.onNotFound([]() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += DM.server.uri();
    message += "\nMethod: ";
    message += (DM.server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += DM.server.args();
    message += "\n";

    for (uint8_t i = 0; i < DM.server.args(); i++) {
      message += " " + DM.server.argName(i) + ": " + DM.server.arg(i) + "\n";
    }

    DM.server.send(404, "text/plain", message);
  });

  server.begin();

  Serial.println("HTTP Server Started");
}

// requestHandlers

/** Handle the info page */
void DeviceManager::infoHandler() {
  DEBUG_DM(F("Info Handler"));

  String page = FPSTR(HTML_HEAD);
  page.replace("{v}", "Info");
  page += FPSTR(HTML_SCRIPT);
  page += FPSTR(HTML_STYLE);
  // page += _customHeadElement;
  page += FPSTR(HTML_HEAD_END);
  page += F("<h3>Device Info</h3>");
  page += F("<ul>");
  page += F("<li>Chip ID: ");
  page += ESP.getChipId();
  page += F("</li>");
  page += F("<li>Flash Chip ID: ");
  page += ESP.getFlashChipId();
  page += F("</li>");
  page += F("<li>IDE Flash Size:  ");
  page += ESP.getFlashChipSize();
  page += F(" bytes</li>");
  page += F("<li>Real Flash Size: ");
  page += ESP.getFlashChipRealSize();
  page += F(" bytes</li>");
  page += F("<li>Device IP: ");
  page += WiFi.localIP().toString();
  page += F("</li>");
  page += F("<li>Soft AP MAC: ");
  page += WiFi.softAPmacAddress();
  page += F("</li>");
  page += F("<li>Station MAC: ");
  page += WiFi.macAddress();
  page += F("</li>");
  page += F("</ul>");
  page += FPSTR(HTML_BACK);
  page += FPSTR(HTML_END);

  server.send(200, "text/html", page);
}

void DeviceManager::addDeviceHander() {
  if (!server.hasArg("pin") || !server.hasArg("name")
    || server.arg("pin") == NULL || server.arg("name") == NULL) {
      server.send(400, "text/plain", "400: Invalid Request");
    }
  else {
    if (addDevice(server.arg("pin").toInt(), server.arg("name").c_str())) {
      server.sendHeader("Location", String("/"), true);
      server.send ( 302, "text/plain", "");
    }
      // server.send(200, "text/plain", "Device Added");
    else
      server.send(500, "text/plain", "500: Can't Add Device");
  }
}

void DeviceManager::clearDevicesHandler() {
  clearDevices();
  // server.send(200, "text/plain", String(_deviceCount));
  server.sendHeader("Location", String("/"), true);
  server.send ( 302, "text/plain", "");
}

void DeviceManager::listDevicesHandler() {
  DEBUG_DM("Devices handler");
  String _customHeadElement = "";
  // char temp[100];
  
  String page = FPSTR(HTML_HEAD);
  page.replace("{v}", "Devices");
  page += FPSTR(HTML_SCRIPT);
  page += FPSTR(HTML_STYLE);
  page += _customHeadElement;
  page += FPSTR(HTML_HEAD_END);
  page += F("<h3>Devices</h3>");
  // page += "<h1>";
  // page += _apName;
  // page += "</h1>";
  // page += FPSTR(HTML_PORTAL_OPTIONS);
  page += "<div> <ul>";
  for (uint8_t i = 0; i < DM._deviceCount; i++) {
    String temp = FPSTR(HTML_DEVICE_LIST);
    temp.replace("{d}", _devices[i].name);
    temp.replace("{p}", String(_devices[i].pin));
    page += temp;
  }
  page += "<br/><div class=\"addDevice\">";
  page += FPSTR(HTML_FORM_ADD_DEV);
  page += FPSTR(HTML_FORM_END);
  page += "</div>";
  page += FPSTR(HTML_BACK);
  page += FPSTR(HTML_END);

  server.send(200, "text/html", page);
}

void DeviceManager::rootHandler() {
  DEBUG_DM("Root handler");
  
  String page = FPSTR(HTML_HEAD);
  page.replace("{v}", "Devices");
  page += FPSTR(HTML_SCRIPT);
  page += FPSTR(HTML_STYLE);
  page += FPSTR(HTML_HEAD_END);
  page += F("<h3>Alexa Node</h3>");
  page += FPSTR(HTML_PORTAL_OPTIONS);
  page += FPSTR(HTML_END);

  server.send(200, "text/html", page);
}

DeviceManager DM;