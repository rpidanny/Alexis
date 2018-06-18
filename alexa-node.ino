#include "NetworkManager.h"
#include "OTAManager.h"
#include "DeviceManager.h"

void setup() {

    Serial.begin(115200);

    // ... connect to wifi ...
    Network.begin();
    Ota.begin();
    DM.begin();
    
}

void loop() {
    DM.handle();
    Ota.handle();
}
