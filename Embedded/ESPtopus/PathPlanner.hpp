#pragma once

#include <Arduino.h>

#include "Map.hpp"

class PathPlanner {
public:
  using Path = void;
  
  PathPlanner(byte forwardBonus);

  Path execute(int xPos, int yPos, const Map& valueMap) const;
  
private:
  byte forwardBonus;
};
