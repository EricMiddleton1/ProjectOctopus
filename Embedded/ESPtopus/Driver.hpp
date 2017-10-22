#pragma once

#include "Motors.hpp"
#include "OpticalFlow.hpp"

class PID {
public:
  PID(float kp, float ki, float kd);

  float set(float setpoint);
  float update(float curPoint);
  
private:
  float kp, ki, kd;
  float integral, lastError;
  float setpoint;

  unsigned long lastTime;
};

class Driver {
public:
  struct Movement {
    float x, y, theta;
  };

  Driver(Motors&& motors, PID&& headingPID, float maxHeadingCorrection);

  void begin();

  void setSpeed(float speed);
  void setHeading(float heading);

  Movement update(const OpticalFlow::Flow& flow);

  float getSpeed() const;
  float getHeading() const;
  float getHeadingCorrection() const;

private:
  static constexpr float TICKS_PER_METER = 11526.517;
  static constexpr float OF_POS_OFFSET = 0.07779; //Meters

  void updateMotors();
  
  Motors motors;
  PID headingPID;

  float speed, heading, headingCorrection, maxHeadingCorrection;
};

