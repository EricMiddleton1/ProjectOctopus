#include "ColorSensor.hpp"


String ColorSensor::Color::toString() const {
  return String("(") + String(r) + String(", ") + String(g) + String(", ") + String(b) + String(")");
}

ColorSensor::ColorSensor()
  : sensor{TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X} {
}

void ColorSensor::begin() {
  sensor.begin();
}

bool ColorSensor::update() {
  uint16_t raw_r, raw_g, raw_b, raw_c, largest;
  
  if(!sensor.getRawData(&raw_r, &raw_g, &raw_b, &raw_c)) {
    return false;
  }

  largest = max(raw_r, max(raw_g, raw_b));

  c = {
    raw_r * 255.f / largest,
    raw_g * 255.f / largest,
    raw_b * 255.f / largest
  };

  return true;
}

ColorSensor::Color ColorSensor::get() const {
  return c;
}

