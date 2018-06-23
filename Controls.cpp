#include "Controls.h"

Controls::Controls() {
  _chipId = String("Alexis-" + ESP.getChipId()).c_str();
  _mqttClient = new PubSubClient(_wifiClient);
}

void Controls::DEBUG_SER(String msg) {
  if (_debug) {
    Serial.print("*SE: ");
    Serial.println(msg);
  }
}

void Controls::enableAlexa() {
  _alexa = true;
}

void Controls::enableMQTT(const char * host, int port) {
  _mqtt = true;
  _mqttHost = host;
  _mqttPort = port;
}

void Controls::begin(Device* devices, uint8_t count) {
  _deviceCount = count;
  for (uint8_t i = 0; i < _deviceCount; i++) {
    _devices[i] = devices[i];
    pinMode(_devices[i].pin, OUTPUT);
    digitalWrite(_devices[i].pin, _devices[i].state);
    if (_alexa) {
      fauxmo.addDevice(_devices[i].name);
      _mqttClient->subscribe(_devices[i].name);
    }
  } 

  _mqttClient->setServer(_mqttHost, _mqttPort);
  _mqttClient->setCallback([](char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    controls.setDevice(topic, payload[0] == 1);
    for (int i = 0; i < length; i++) {
      Serial.print(payload[i]);
    }
    Serial.println();
  });

  if (_alexa) {
    fauxmo.enable(true);
    // TODO: GPIO controls
    fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state) {
      Serial.printf("[fauxmo] Device #%d (%s) state: %s\n", device_id, device_name, state ? "ON" : "OFF");
      controls.setDevice(device_name, state);
    });
    fauxmo.onGetState([](unsigned char device_id, const char * device_name) {
        return true; // whatever the state of the device is
    });
  }
}

void Controls::reconnectMqtt() {
  if (_mqttClient->connect(_chipId)) {
    DEBUG_SER("MQTT Connected");
    _mqttClient->publish("heartbeat", "Client Connected.");
    for (uint8_t i = 0; i < _deviceCount; i++) {
      _mqttClient->subscribe(_devices[i].name);
    }
  } else {
    DEBUG_SER("Will try to connect to MQTT server again in a while..");
  }
}

void Controls::handle() {
  if (_mqtt) {
    if (!_mqttClient->connected() && (millis() - _lastMillis > MQTT_RECONNECT)) {
      _lastMillis = millis();
      reconnectMqtt();
    }
    _mqttClient->loop();
  }
  if (_alexa) {
    fauxmo.handle();
  }
}

void Controls::setDevice(const char * name, bool state) {
  for (uint8_t i = 0; i < _deviceCount; i++) {
    if (strcmp(_devices[i].name, name) == 0) {
      digitalWrite(_devices[i].pin, state);
      break;
    }
  }
}

Controls controls;