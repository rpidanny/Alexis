#ifndef DeviceMANAGER_H
#define DeviceMANAGER_H

#include <fauxmoESP.h>

class DeviceManager {
  public:
    void begin();
    void handle();
  private:
    void DEBUG_DM(String msg);

    bool _debug = true;
    fauxmoESP fauxmo;
};

extern DeviceManager DM;

#endif