#ifndef DeviceMANAGER_H
#define DeviceMANAGER_H

#ifndef NAME_LENGTH
  #define NAME_LENGTH 20
#endif
#ifndef MAX_DEVICES
  #define MAX_DEVICES 5
#endif
#ifndef CONFIG_PIN
  #define CONFIG_PIN D3
#endif
#ifndef BTN_PRESS_TIME
  #define BTN_PRESS_TIME 5000
#endif

#define DEVICE_COUNT_ADDR 0  // address 0 of EEPROM
#define SIZE sizeof(Device) * MAX_DEVICES // Maxing at 5 devices for now

#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <memory>

#include "Device.h"
#include "Controls.h"

const char HTML_HEAD[] PROGMEM            = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>";
const char HTML_STYLE[] PROGMEM           = "<style>body{text-align: center;font-family:verdana; background: #36393e;} button, input[type=\"button\"]{border:0;background-color:#1fa3ec;color:#fff;line-height:1.4rem;font-size:12px;width:100%;} * { box-sizing: border-box; } ul { list-style-type: none; padding: 0; margin: 0; } ul li { border: 1px solid #ddd; margin-top: -1px; background-color: #f6f6f6; padding: 12px; text-decoration: none; font-size: 12px; color: black; display: block; position: relative; } ul li:hover { background-color: #eee; } .close, .add { cursor: pointer; position: absolute; top: 50%; right: 0%; padding: 12px 16px; transform: translate(0%, -50%); color: red;} .close:hover, .add:hover {background: #bbb;} .addDevice { border: 1px solid #ddd; margin-top: -1px; }  .css-input { font-size:12px; border-color:#cccccc; border-style:solid; border-width:1px; box-shadow: 0px 0px 0px 0px rgba(42,42,42,.75); padding:5px;} .name {text-transform: uppercase;font-weight: 550;} h3 {color: #000;font-family: 'Helvetica Neue', sans-serif;font-size: 28px;font-weight: bold;letter-spacing: -1px;line-height: 1; text-transform: uppercase;}</style>";
const char HTML_CONFIRM_SCRIPT[] PROGMEM  = "<script type=\"text/javascript\">function confSubmit(form){if(confirm(\"Are you sure?\")) {form.submit();}else {console.log(\"cancel\");}}</script>";
const char HTML_DEVICES_SCRIPT[] PROGMEM  = "<script>function onLoad(){var closebtns = document.getElementsByClassName(\"close\"); var i; for (i = 0; i < closebtns.length; i++) { closebtns[i].addEventListener(\"click\", function(e) { console.log(e); var r = new XMLHttpRequest();r.open( \"GET\", \"/del?name=\"+e.path[1].childNodes[0].textContent, false );r.send(null);window.location.href = \"/devices\";});}}</script>";
const char HTML_HEAD_END[] PROGMEM        = "</head><body onload=\"onLoad()\"><div style='display: inline-block;min-width: 330px;border: 1px solid gray;padding: 20px;padding-bottom: 35px;padding-top: 10px; box-shadow: 3px 3px 10px 0px rgba(0,0,0,0.46);background-color: #fff;'>";
const char HTML_HEADER[] PROGMEM          = "<h3>{v}</h3>";
const char HTML_PORTAL_OPTIONS[] PROGMEM  = "<form action=\"/devices\" method=\"get\"><button>Devices</button></form><br/><form action=\"/controls\" method=\"get\"><button>Controls</button></form><br/><form action=\"/info\" method=\"get\"><button>System Info</button></form><br/><form action=\"/del\" method=\"get\"><input type=\"button\" onClick=\"confSubmit(this.form);\" value=\"Factory Reset\"></form><br/><form action=\"/rs\" method=\"get\"><input type=\"button\" onClick=\"confSubmit(this.form);\" value=\"Restart\"></form>";
const char HTML_FORM_ADD_DEV[] PROGMEM    = "<form action=\"/add\" method='post' style=\"padding:5px;\"><input name='name' class=\"\css-input\" length=20 placeholder='Device Name' style=\"width:70%;\"><input name='pin' class=\"\css-input\" length=4 type='number' placeholder='PIN'  style=\"width:20%; float:right;\">";
const char HTML_FORM_CONTROLS[] PROGMEM   = "<form action=\"/c\" method='post' style=\"padding:5px;\"><input type=\"checkbox\" name=\"alexa\" {a}>Alexa Control<br><input type=\"checkbox\" name=\"mqtt\" {m}>MQTT<br>";
const char HTML_FORM_END[] PROGMEM        = "<br/><input type=\"button\" onClick=\"confSubmit(this.form);\" value=\"Add\"></form>";
const char HTML_DEVICE_LIST[] PROGMEM     = "<li><span class=\"name\">{d}</span> : {p} <span class=\"close\">x</span></li>";
const char HTML_BACK[] PROGMEM            = "<div><br><br><form action=\"/\" method=\"get\"><button type='submit'>Back</button></form></div>";
const char HTML_END[] PROGMEM             = "</div></body></html>";
const char HTML_REDIRECT[] PROGMEM        = "<html><head><meta http-equiv=\"refresh\" content=\"5;url=/\" /></head><body><h3>{v}</h3></body></html>";

class DeviceManager {
  public:
    void begin();
    void handle();
    bool addDevice(uint8_t pin, const char * name);
    void printDevices();
    void delDevice(const char * name = NULL);
    void setDebug(bool flag);
  private:
    void DEBUG_DM(String msg);
    uint8_t readROM(uint8_t addr);
    void writeROM(uint8_t addr, uint8_t data);
    int8_t getDeviceIndex(const char * name);
    int8_t getDeviceIndex(uint8_t pin);

    void startConfigServer();

    // requestHandlers
    void addDeviceHander();
    void addDevicePageHander();
    void delDevicesHandler();
    void listDevicesHandler();
    void rootHandler();
    void infoHandler();
    void notFoundHander();
    void restartHander();
    void setControlsHandler();
    void controlsPageHandler();

    bool _debug = true;
    bool _config = false;
    bool _alexa = false;
    bool _mqtt = false;
    long _buttonTimer = 0;
    uint8_t _deviceCount;
    String _apName;

    Device _devices[MAX_DEVICES];
    ESP8266WebServer server;
};

extern DeviceManager DM;

#endif