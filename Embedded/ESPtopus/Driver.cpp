#include "Driver.hpp"

#include <Arduino.h>
#include <math.h>

PID::PID(float _kp, float _ki, float _kd)
  : kp  {_kp}
  , ki  {_ki}
  , kd  {_kd}
  , integral  {0.f}
  , lastError {0.f}
  , setpoint  {0.f}
  , lastTime  {0} {

}

float PID::set(float _setpoint) {
  setpoint = _setpoint;
}

float PID::update(float curpoint) {
  unsigned long curTime = micros();
  
  if(lastTime == 0) {
    lastTime = curTime;

    return 0.f;
  }
  else {
    float dt = (curTime - lastTime) * 0.000001;
  
    float error = setpoint - curpoint;
  
    integral += error*dt;
  
    float actuation = kp*error + ki*integral + kd*(error - lastError)/dt;
  
    lastError = error;
    lastTime = curTime;
  
    return actuation;
  }
}

Driver::Driver(Motors&& _motors, PID&& _headingPID, float _maxHeadingCorrection)
  : motors  {_motors}
  , headingPID  {_headingPID}
  , speed {0.f}
  , heading {0.f}
  , headingCorrection {0.f}
  , maxHeadingCorrection  {_maxHeadingCorrection} {
}

void Driver::begin() {
  motors.begin();
}

void Driver::setSpeed(float _speed) {
  speed = _speed;
}

void Driver::setHeading(float _heading) {
  headingPID.set(_heading);
}

Driver::Movement Driver::update(const OpticalFlow::Flow& flow) {
  Movement m;
  
  float ofX = -flow.x / TICKS_PER_METER,
    ofY = -flow.y / TICKS_PER_METER;

  m.theta = ofX / OF_POS_OFFSET;
  if(m.theta == 0.f) {
    m.x = ofY;
    m.y = 0.f;
  }
  else {
    float r = ofY / m.theta;
    m.x = r * sin(m.theta);
    m.y = r * (1.f - cos(m.theta));
  }

  heading += m.theta;

  headingCorrection = constrain(headingPID.update(heading), -maxHeadingCorrection, maxHeadingCorrection);
  updateMotors();

  return m;
}

float Driver::getSpeed() const {
  return speed;
}

float Driver::getHeading() const {
  return heading;
}

float Driver::getHeadingCorrection() const {
  return headingCorrection;
}

void Driver::updateMotors() {
  motors.setSpeed(speed - headingCorrection, speed + headingCorrection);
}

