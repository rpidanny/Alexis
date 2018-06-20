#include "Services.h"

Services::Services() {
  
}

void Services::begin() {
  WiFiClient espClient;
  PubSubClient tmp(espClient);
  client = tmp;
  client.setServer("192.168.2.12", 1883);
  client.setCallback([](char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();
  });

  client.connect("Alexis");
  client.publish("heartbeat", "hello world");
  client.subscribe("Alexis");
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

Services Ser;