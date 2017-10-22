#include "Motors.hpp"

#include <Arduino.h>
#include <math.h>

Motors::Motors(int _pinL0, int _pinL1, int _pinR0, int _pinR1)
  : pinL0 {_pinL0}
  , pinL1 {_pinL1}
  , pinR0 {_pinR0}
  , pinR1 {_pinR1} {
}

void Motors::begin() {
  pinMode(pinL0, OUTPUT);
  pinMode(pinL1, OUTPUT);
  pinMode(pinR0, OUTPUT);
  pinMode(pinR1, OUTPUT);

  digitalWrite(pinL0, LOW);
  digitalWrite(pinL1, LOW);
  digitalWrite(pinR0, LOW);
  digitalWrite(pinR1, LOW);
}

void Motors::setSpeed(float left, float right) {
  setMotor(pinL0, pinL1, left);
  setMotor(pinR0, pinR1, right);
}

void Motors::setMotor(int pinA, int pinB, float value) {
  value = min(1.f, max(-1.f, value));
  
  bool dir = value < 0.f;
  int pwm = PWM_MAX * fabs(value);

  digitalWrite(pinA, dir);
  analogWrite(pinB, dir ? (PWM_MAX - pwm) : pwm);
}

