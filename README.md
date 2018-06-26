# Alexis [![Build Status](https://travis-ci.org/rpidanny/Alexis.svg?branch=develop)](https://travis-ci.org/rpidanny/Alexis)
Alexis enables the control of any GPIO pins through Amazon Alexa or MQTT.
The functions of GPIO pins can be configured dynamically through a Web Interface.
Just upload the Alexis firmware and chill, everything can be configured from the UI.

## Wishlist

- [ ] DeviceManager
  - [ ] GPIO Pin pool. (Don't allow specific gpio pins to be used for devices)
  - [x] Save Devices to EEPROM
  - [x] Load Devices from EEPROM
  - [ ] Controls
    - [ ] Enable/Disable Services
    - [ ] Amazon Alexa
      - [x] Add Devices
      - [x] Hook GPIO to State Change
    - [ ] MQTT
      - [ ] MQTT configuration
      - [x] Hook GPIO to topic
    - [ ] Timers ?
  - [x] Configure Mode (Button Press or something else?)
  - [ ] HTTP Server for Configuration
    - [x] List Devices
    - [x] Add Device
    - [x] Remove Device
    - [x] Reset
    - [ ] Edit Device ?
  - [ ] IR

## License

This project is licensed under the MIT License - see the [license file](LICENSE) file for details
