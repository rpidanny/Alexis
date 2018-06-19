#ifndef DeviceMANAGER_H
#define DeviceMANAGER_H

#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <fauxmoESP.h>
#include <memory>

#ifndef NAME_LENGTH
  #define NAME_LENGTH 20
#endif
#ifndef MAX_DEVICES
  #define MAX_DEVICES 5
#endif
#define DEVICE_COUNT_ADDR 0  // address 0 of EEPROM
#define SIZE sizeof(Device) * MAX_DEVICES // Maxing at 5 devices for now

const char HTML_HEAD[] PROGMEM            = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>";
const char HTML_STYLE[] PROGMEM           = "<style>body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} * { box-sizing: border-box; } ul { list-style-type: none; padding: 0; margin: 0; } ul li { border: 1px solid #ddd; margin-top: -1px; background-color: #f6f6f6; padding: 12px; text-decoration: none; font-size: 18px; color: black; display: block; position: relative; } ul li:hover { background-color: #eee; } .close { cursor: pointer; position: absolute; top: 50%; right: 0%; padding: 12px 16px; transform: translate(0%, -50%); color: red;} .close:hover {background: #bbb;} .addDevice { border: 1px solid #ddd; margin-top: -1px; }</style>";
const char HTML_SCRIPT[] PROGMEM          = "<script>var closebtns = document.getElementsByClassName(\"close\"); var i; for (i = 0; i < closebtns.length; i++) { closebtns[i].addEventListener(\"click\", function() { this.parentElement.style.display = 'none'; }); }</script>";
const char HTML_HEAD_END[] PROGMEM        = "</head><body><div style='text-align:left;display:inline-block;min-width:260px;'>";
const char HTML_PORTAL_OPTIONS[] PROGMEM  = "<form action=\"/devices\" method=\"get\"><button>List Devices</button></form><br/><form action=\"/info\" method=\"get\"><button>Device Info</button></form><br/><form action=\"/clear\" method=\"get\"><button>Reset Device</button></form>";
const char HTML_FORM_ADD_DEV[] PROGMEM     = "<form action=\"/add\" method='post' ><input name='name' length=20 placeholder='Device Name'><br/><input name='pin' length=4 type='number' placeholder='Pin Number'><br/>";
const char HTML_FORM_END[] PROGMEM        = "<br/><button type='submit'>save</button></form>";
const char HTML_DEVICE_LIST[] PROGMEM      = "<li><span class=\"deviceName\">{d}</span> : {p} <span class=\"close\">x</span></li>";
const char HTML_BACK[] PROGMEM             = "<div><br><br><form action=\"/\" method=\"get\"><button type='submit'>Back</button></form></div>";
const char HTML_END[] PROGMEM             = "</div></body></html>";

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
    void addDevicePageHander();
    void clearDevicesHandler();
    void listDevicesHandler();
    void rootHandler();
    void infoHandler();

    bool _debug = true;
    uint8_t _deviceCount;
    String _apName;
    Device _devices[MAX_DEVICES];
    fauxmoESP fauxmo;
    ESP8266WebServer server;
};

extern DeviceManager DM;

#endif