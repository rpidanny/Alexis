#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <EEPROM.h>
#include "fauxmoESP.h"
#include "credentials.h"

#define SERIAL_BAUDRATE                 115200

#define LED                             2

#define RELAYPIN1  14
#define RELAYPIN2  13
#define RELAYPIN3  12
#define RELAYPIN4  15


fauxmoESP fauxmo;

IRsend irsend(16);

// -----------------------------------------------------------------------------
// Wifi
// -----------------------------------------------------------------------------

void wifiSetup() {

    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);

    // Connect
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Wait
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    // Connected!
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

}

void gpioSetup(){
  pinMode(RELAYPIN1,OUTPUT);
  pinMode(RELAYPIN2,OUTPUT);
  pinMode(RELAYPIN3,OUTPUT);
  //pinMode(RELAYPIN4,OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  digitalWrite(RELAYPIN1, EEPROM.read(0));
  digitalWrite(RELAYPIN2, EEPROM.read(1));
  digitalWrite(RELAYPIN3, EEPROM.read(2));
  //digitalWrite(RELAYPIN4, HIGH);

  if(EEPROM.read(3)){
    irsend.sendNEC(16203967, 32);
  }else{
    irsend.sendNEC(16236607, 32);
  }
  
  for(int i=0;i<4;i++){
    Serial.println(EEPROM.read(i),DEC);
  }
}

void writeFlash(int addr,byte data){
  EEPROM.write(addr, data);
  EEPROM.commit();
}
void setup() {

    EEPROM.begin(4);
    
    // Init serial port and clean garbage
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();
    Serial.println();

    //ir setup
    irsend.begin();

    //irsend.sendNEC(0xF740BF, 32);
    //delay(1000);
    //irsend.sendNEC(0xF7C03F, 32);

    // Setup GPIO
    gpioSetup();
    
    // Wifi
    wifiSetup();

    // Fauxmo
    fauxmo.addDevice("movie light");
    fauxmo.addDevice("bed light");
    fauxmo.addDevice("tube light");
    fauxmo.addDevice("dome light");

    // fauxmoESP 2.0.0 has changed the callback signature to add the device_id, this WARRANTY
    // it's easier to match devices to action without having to compare strings.
    fauxmo.onMessage([](unsigned char device_id, const char * device_name, bool state) {
        String device = String(device_name);
        Serial.printf("[MAIN] Device #%d (%s) state: %s\n", device_id, device_name, state ? "ON" : "OFF");
        if(device.equals("dome light")){
          writeFlash(0,!state);
          digitalWrite(RELAYPIN1, !state);
        }else if(device.equals("tube light")){
          writeFlash(1,!state);
          digitalWrite(RELAYPIN2, !state);
        }else if(device.equals("bed light")){
          writeFlash(2,!state);
          digitalWrite(RELAYPIN3, !state);
        }else if(device.equals("movie light")){
          writeFlash(3,!state);
          digitalWrite(LED, !state);
          if(state){
            irsend.sendNEC(16236607, 32);
          }else{
            irsend.sendNEC(16203967, 32);
          }
        }
    });

}

void loop() {

    // Since fauxmoESP 2.0 the library uses the "compatibility" mode by
    // default, this means that it uses WiFiUdp class instead of AsyncUDP.
    // The later requires the Arduino Core for ESP8266 staging version
    // whilst the former works fine with current stable 2.3.0 version.
    // But, since it's not "async" anymore we have to manually poll for UDP
    // packets
    fauxmo.handle();


    static unsigned long last = millis();
    if (millis() - last > 5000) {
        last = millis();
        Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
    }

}
