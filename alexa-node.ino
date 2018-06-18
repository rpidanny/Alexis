#include "NetworkManager.h"
#include <fauxmoESP.h>

fauxmoESP fauxmo;

void setup() {

    Serial.begin(115200);

    // ... connect to wifi ...
    Network.begin();
    
    fauxmo.addDevice("light one");
    fauxmo.addDevice("light two");
    fauxmo.addDevice("light three");
    fauxmo.addDevice("light four");
    fauxmo.enable(true);

    fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state) {
        Serial.printf("[MAIN] Device #%d (%s) state: %s\n", device_id, device_name, state ? "ON" : "OFF");
    });
    fauxmo.onGetState([](unsigned char device_id, const char * device_name) {
        return true; // whatever the state of the device is
    });

}

void loop() {
    fauxmo.handle();
}
