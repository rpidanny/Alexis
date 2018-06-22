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

void Controls::begin() {
  _mqttClient->setServer("192.168.2.12", 1883);
  _mqttClient->setCallback([](char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();
  });

  fauxmo.enable(true);
  // TODO: GPIO controls
  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state) {
      Serial.printf("[fauxmo] Device #%d (%s) state: %s\n", device_id, device_name, state ? "ON" : "OFF");
  });
  fauxmo.onGetState([](unsigned char device_id, const char * device_name) {
      return true; // whatever the state of the device is
  });
}

void Controls::addDevice(Device d) {
  pinMode(d.pin, OUTPUT);
  digitalWrite(d.pin, d.state);
  fauxmo.addDevice(d.name);
  _mqttClient->subscribe(d.name);
}

void Controls::mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void Controls::reconnect() {
  if (_mqttClient->connect(_chipId)) {
    DEBUG_SER("MQTT Connected");
    _mqttClient->publish("heartbeat", "hello world");
    _mqttClient->subscribe("Alexis");
  } else {
    DEBUG_SER("Will try to connect to MQTT server again in a while..");
  }
}

void Controls::handle() {
  if (!_mqttClient->connected() && (millis() - _lastMillis > MQTT_RECONNECT)) {
    _lastMillis = millis();
    reconnect();
  }
  _mqttClient->loop();
  fauxmo.handle();
}

Controls controls;