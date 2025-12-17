#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include "OneWire.h"
#include "DallasTemperature.h"
#include <Arduino.h>

class TemperatureSensor {
private: 
  OneWire oneWire;
  DallasTemperature sensor;
  float lastTemperature;

public:
  TemperatureSensor(const unsigned int sensorPin);

  void init();

  bool isConnected();

  float getTemperature();

  float getLastTemperature();
};

#endif /* TEMPERATURE_H */