#include "NetworkManager.h"
#include "DeviceManager.h"

void setup() {
    Serial.begin(115200);
    Network.begin();
    DM.begin();    
}

void loop() {
    DM.handle();
    Network.handle();
}
