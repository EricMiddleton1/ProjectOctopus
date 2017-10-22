#pragma once


class Motors {
public:
  Motors(int pinL0, int pinL1, int pinR0, int pinR1);

  void begin();

  void setSpeed(float left, float right);

private:
  static const int PWM_MAX = 1023;

  static void setMotor(int pinA, int pinB, float value);

  int pinL0, pinL1, pinR0, pinR1;
};

