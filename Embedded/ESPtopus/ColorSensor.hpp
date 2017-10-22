#pragma once

#include <Arduino.h>
#include "Adafruit_TCS34725.h"

class ColorSensor {
public:
  struct Color {
    byte r, g, b;

    String toString() const;
  };
  
  ColorSensor();

  void begin();

  bool update();
  Color get() const;
private:
  Adafruit_TCS34725 sensor;

  Color c;
};
