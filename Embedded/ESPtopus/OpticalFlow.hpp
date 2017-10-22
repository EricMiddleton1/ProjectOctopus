#pragma once


class OpticalFlow {
public:
  struct Flow {
    Flow();
    Flow(int x, int y);

    Flow& operator+=(const Flow&);
    
    int x, y;
  };

  OpticalFlow(int pinX0, int pinX1, int pinY0, int pinY2, int pinEnable);

  void begin();

  Flow get();

private:
  static int flowState(int a, int b);
  static void updateCount(volatile int& state, volatile int& count, int pinA, int pinB);

  int pinX0, pinX1, pinY0, pinY1, pinEnable;
  
  volatile int stateX, stateY;
  volatile Flow flow;
};
