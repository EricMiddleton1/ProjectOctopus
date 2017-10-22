#pragma once

#include <Arduino.h>

class Map {
public:
  Map(size_t width, size_t height);
  ~Map();

  operator bool() const;

  const byte* getRaw() const;

  byte get(size_t x, size_t y) const;
private:
  byte* data;
  size_t width, height;
};

