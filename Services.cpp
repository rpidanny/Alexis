#include "Services.h"

Services::Services() {
  _chipId = String("Alexis-" + ESP.getChipId()).c_str();
  _mqttClient = new PubSubClient(_wifiClient);
}

void Services::DEBUG_SER(String msg) {
  if (_debug) {
    Serial.print("*SE: ");
    Serial.println(msg);
  }
}

void Services::begin() {
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
}

void Services::addDevice(const char * name, CallbackFunction callback) {
  delay(1000);
  callback();
}

void Services::mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void Services::reconnect() {
  if (_mqttClient->connect(_chipId)) {
    DEBUG_SER("MQTT Connected");
    _mqttClient->publish("heartbeat", "hello world");
    _mqttClient->subscribe("Alexis");
  } else {
    DEBUG_SER("Will try to connect to MQTT server again in a while..");
  }
}

void Services::handle() {
  if (!_mqttClient->connected() && (millis() - _lastMillis > MQTT_RECONNECT)) {
    _lastMillis = millis();
    reconnect();
  }
  _mqttClient->loop();
}

Services Ser;