#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "Driver.hpp"
#include "ColorSensor.hpp"


class Communicator {
public:
  Communicator(const String& ip, int port, int botID);

  bool begin();

  void sendUpdate(const Driver::Movement&, const ColorSensor::Color&, unsigned long timestamp);
private:
  static void swap(char&, char&);

  WiFiClient client;

  String ip;
  int port, botID;
};
