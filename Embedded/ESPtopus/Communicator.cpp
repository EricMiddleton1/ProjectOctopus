#include "Communicator.hpp"

#include <ArduinoJson.h>

Communicator::Communicator(const String& _ip, int _port, int _botID)
  : ip  {_ip}
  , port  {_port}
  , botID {_botID} {
}

bool Communicator::begin() {
  return client.connect(ip.c_str(), port);
}

void Communicator::sendUpdate(const Driver::Movement& movement, const ColorSensor::Color& color, unsigned long timestamp) {
  StaticJsonBuffer<256> buffer;

  auto& root = buffer.createObject();
  root["dx"] = movement.x;
  root["dy"] = movement.y;
  root["phi"] = movement.theta;
  root["time"] = timestamp;

  auto& rgb = root.createNestedArray("color");
  rgb.add(color.r);
  rgb.add(color.g);
  rgb.add(color.b);

  int bufferSize = root.measureLength();

  char jsonOut[256];
  memcpy(jsonOut, &bufferSize, sizeof(bufferSize));
  root.printTo(jsonOut + sizeof(bufferSize), bufferSize + 1);

  client.write((const uint8_t*)jsonOut, bufferSize + sizeof(bufferSize));
}

void Communicator::swap(char& a, char& b) {
  byte temp = a;

  a = b;
  b = temp;
}

