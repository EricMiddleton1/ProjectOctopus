#include "OpticalFlow.hpp"

#include <Arduino.h>

static OpticalFlow* __flow;

OpticalFlow::Flow::Flow()
  : x {0}
  , y {0} {
}

OpticalFlow::Flow::Flow(int _x, int _y)
  : x { _x  }
  , y { _y  } {
  
}

OpticalFlow::Flow& OpticalFlow::Flow::operator+=(const Flow& other) {
  x += other.x;
  y += other.y;

  return *this;
}

OpticalFlow::OpticalFlow(int _pinX0, int _pinX1, int _pinY0, int _pinY1, int _pinEnable)
  : pinX0 {_pinX0}
  , pinX1 {_pinX1}
  , pinY0 {_pinY0}
  , pinY1 {_pinY1}
  , pinEnable {_pinEnable}
  , stateX  {0}
  , stateY  {0}
  , flow  { 0, 0 }  {

  __flow = this;
}

void OpticalFlow::begin() {
  pinMode(pinX0, INPUT);
  pinMode(pinX1, INPUT);
  pinMode(pinY0, INPUT);
  pinMode(pinY1, INPUT);

  digitalWrite(pinEnable, LOW);
  pinMode(pinEnable, OUTPUT);

  delay(1000);
  digitalWrite(pinEnable, HIGH);

  stateX = flowState(digitalRead(pinX0), digitalRead(pinX1));
  stateY = flowState(digitalRead(pinY0), !digitalRead(pinY1));

  auto updateX = []() {
    updateCount(__flow->stateX, __flow->flow.x, __flow->pinX0, __flow->pinX1);
  };

  auto updateY = []() {
    updateCount(__flow->stateY, __flow->flow.y, __flow->pinY0, __flow->pinY1);
  };
  
  attachInterrupt(digitalPinToInterrupt(pinX0), updateX, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pinX1), updateX, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pinY0), updateY, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pinY1), updateY, CHANGE);
}

OpticalFlow::Flow OpticalFlow::get() {
  noInterrupts();
  Flow retval(flow.x, flow.y);

  flow.x = 0;
  flow.y = 0;
  interrupts();

  return retval;
}

int OpticalFlow::flowState(int a, int b) {
  return (a << 1) | b;
}

void OpticalFlow::updateCount(volatile int& state, volatile int& count, int pinA, int pinB) {
  const int STATE_TABLE[4][4] {
    {0, 1, -1, 0},
    {-1, 0, 0, 1},
    {1, 0, 0, -1},
    {0, -1, 1, 0}
  };
  
  int newState = flowState(digitalRead(pinA), digitalRead(pinB));

  count += STATE_TABLE[state][newState];
  state = newState;
}

