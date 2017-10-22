#include <Adafruit_TCS34725.h>

#include <ESP8266WiFi.h>

#include "OpticalFlow.hpp"
#include "Driver.hpp"

#define PIN_L0  0
#define PIN_L1  1
#define PIN_R0  2
#define PIN_R1  16

#define PIN_OF_EN 15
#define PIN_OF_X0 12
#define PIN_OF_X1 13
#define PIN_OF_Y0 14
#define PIN_OF_Y1 3

#define HEADING_KP  0.1f
#define HEADING_KI  0.01f
#define HEADING_KD  0.f

unsigned long motorTime;
bool dir;

OpticalFlow of(PIN_OF_X0, PIN_OF_X1, PIN_OF_Y0, PIN_OF_Y1, PIN_OF_EN);
OpticalFlow::Flow flow;

Driver driver( {PIN_L0, PIN_L1, PIN_R0, PIN_R1}, {HEADING_KP, HEADING_KI, HEADING_KD} );

WiFiClient tcpClient;

void setup() {
  WiFi.persistent(false);

  driver.begin();
  of.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin("OctoNet", "3ricn3t1");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  tcpClient.connect("192.168.2.3", 8080);
  tcpClient.print("Connected.\r\n");

  driver.setSpeed(0.2);
  driver.setHeading(0.f);
}

void loop() {
  auto newFlow = of.get();
  auto movement = driver.update(newFlow);
  
  //tcpClient.print(String("Flow X: ") + String(flow.x) + String("\tFlow Y: ") + String(flow.y) + String("\r\n"));
  tcpClient.print(String("\tHeading: ") + String(driver.getHeading() * 180.f / PI) + String("\tHeading correction: ") + String(driver.getHeadingCorrection()) + String("\r\n"));

  flow += newFlow;

  delay(10);
}
