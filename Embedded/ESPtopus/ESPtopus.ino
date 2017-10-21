#include <Adafruit_TCS34725.h>

#include <ESP8266WiFi.h>

#define PIN_L0  0
#define PIN_L1  1
#define PIN_R0  2
#define PIN_R1  16

#define PIN_OF_EN 15
#define PIN_OF_X0 12
#define PIN_OF_X1 13
#define PIN_OF_Y0 14
#define PIN_OF_Y1 3

#define PWM_MAX 1023
#define SPEED 256

uint16_t pwm = 0;

volatile int counterX = 0, counterY = 0;
volatile int stateX, stateY;

unsigned long motorTime;
bool dir;

WiFiClient tcpClient;

void setup() {
  WiFi.persistent(false);
  
  digitalWrite(PIN_OF_EN, LOW);
  pinMode(PIN_OF_EN, OUTPUT);

  pinMode(PIN_OF_X0, INPUT);
  pinMode(PIN_OF_X1, INPUT);
  pinMode(PIN_OF_Y0, INPUT);
  pinMode(PIN_OF_Y1, INPUT);

  stateX = flowState(digitalRead(PIN_OF_X0), digitalRead(PIN_OF_X1));
  stateY = flowState(digitalRead(PIN_OF_Y0), digitalRead(PIN_OF_Y1));

  attachInterrupt(digitalPinToInterrupt(PIN_OF_X0), isrFlowX, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_OF_X1), isrFlowX, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_OF_Y0), isrFlowY, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_OF_Y1), isrFlowY, CHANGE);

  pinMode(PIN_L0, OUTPUT);
  pinMode(PIN_L1, OUTPUT);
  pinMode(PIN_R0, OUTPUT);
  pinMode(PIN_R1, OUTPUT);

  delay(1000);
  digitalWrite(PIN_OF_EN, HIGH);

  WiFi.mode(WIFI_STA);
  WiFi.begin("OctoNet", "3ricn3t1");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  tcpClient.connect("192.168.2.3", 8080);
  tcpClient.print("Connected.\r\n");

  motorTime = millis();
  analogWrite(PIN_L0, SPEED);
  analogWrite(PIN_R0, SPEED);
  dir = false;
}

void loop() {
  unsigned long curTime = millis();
  if(curTime > (motorTime + 5000)) {
    motorTime = curTime;

    dir = !dir;
    
    digitalWrite(PIN_L1, dir);
    digitalWrite(PIN_R1, dir);
    
    if(dir) {
      analogWrite(PIN_L0, PWM_MAX - SPEED);
      analogWrite(PIN_R0, PWM_MAX - SPEED);
    }
    else {
      analogWrite(PIN_L0, SPEED);
      analogWrite(PIN_R0, SPEED);
    }
  }

  tcpClient.print(String("Flow X: ") + String(counterX) + String("\tFlow Y: ") + String(counterY) + String("\r\n"));

  delay(10);
}

int flowState(int a, int b) {
  return (a << 1) | b;
}

void isrFlowX() {
  const int STATE_TABLE[4][4] {
    {0, 1, -1, 0},
    {-1, 0, 0, 1},
    {1, 0, 0, -1},
    {0, -1, 1, 0}
  };
  
  int newState = flowState(digitalRead(PIN_OF_X0), digitalRead(PIN_OF_X1));

  counterX += STATE_TABLE[stateX][newState];
  stateX = newState;
}

void isrFlowY() {
  const int STATE_TABLE[4][4] {
    {0, 1, -1, 0},
    {-1, 0, 0, 1},
    {1, 0, 0, -1},
    {0, -1, 1, 0}
  };
  
  int newState = flowState(digitalRead(PIN_OF_Y0), !digitalRead(PIN_OF_Y1));

  counterY += STATE_TABLE[stateY][newState];
  stateY = newState;
}

