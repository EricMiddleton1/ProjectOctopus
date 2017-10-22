#include "PathPlanner.hpp"

#include "StaticQueue.hpp"

PathPlanner::PathPlanner(byte _forwardBonus)
  : forwardBonus  {_forwardBonus} {
}

void PathPlanner::execute(int xPos, int yPos, const Map& valueMap) const {
  struct Coordinate {
    uint16_t x, y;
  };

  StaticQueue<Coordinate, 1024> nodeQueue;

  

  nodeQueue.push({xPos, yPos});
}
