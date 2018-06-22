#ifndef DEVICE_H
#define DEVICE_H

#ifndef NAME_LENGTH
  #define NAME_LENGTH 20
#endif

typedef struct {
  uint8_t pin;
  bool state;
  char name[NAME_LENGTH];
} Device;

#endif