#pragma once

#include <Arduino.h>

class Map {
public:
  Map(size_t width, size_t height);
  ~Map();

  operator bool() const;

  void clear();

  const byte* getRaw() const;
  byte* getRaw();

  byte get(size_t x, size_t y) const;
  byte& get(size_t x, size_t y);
  
private:
  byte* data;
  size_t width, height;
};

