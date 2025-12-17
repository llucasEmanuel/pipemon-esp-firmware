#include "Temperature.h"

TemperatureSensor::TemperatureSensor(const unsigned int sensorPin) : oneWire(sensorPin), sensor(&oneWire) {
  this->lastTemperature = 25;
}

void TemperatureSensor::init() {
  this->sensor.begin();
}

bool TemperatureSensor::isConnected() {
  this->sensor.requestTemperatures();
  float tempC = this->sensor.getTempCByIndex(0);
  return (tempC != DEVICE_DISCONNECTED_C);
}

float TemperatureSensor::getTemperature() {
  this->sensor.requestTemperatures();
  float tempC = this->sensor.getTempCByIndex(0);
  this->lastTemperature = tempC;
  return tempC;
}

float TemperatureSensor::getLastTemperature() {
  return this->lastTemperature;
}