#include "NetworkManager.h"
#include "OTAManager.h"
#include "DeviceManager.h"

void setup() {

    Serial.begin(115200);
    Network.begin();
    Ota.begin();
    DM.begin();
    // Devices can be added from config page
//    DM.addDevice(D4, "light");
//    DM.addDevice(D5, "laptop");
    DM.printDevices();
    
}

void loop() {
    DM.handle();
    Ota.handle();
}
