#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#include "OpticalFlow.hpp"
#include "Driver.hpp"
#include "ColorSensor.hpp"
#include "Communicator.hpp"

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
#define HEADING_KI  0.f //0.01f
#define HEADING_KD  0.f

#define MAX_HEADING_CORRECTION  0.1f

#define IP_ADDR   "192.168.2.4"
#define PORT  8080
#define BOT_ID  0

OpticalFlow of(PIN_OF_X0, PIN_OF_X1, PIN_OF_Y0, PIN_OF_Y1, PIN_OF_EN);
OpticalFlow::Flow flow;

Driver driver( {PIN_L0, PIN_L1, PIN_R0, PIN_R1}, {HEADING_KP, HEADING_KI, HEADING_KD}, MAX_HEADING_CORRECTION);

ColorSensor colorSensor;

Communicator communicator(IP_ADDR, PORT, BOT_ID);

float heading, headingDir;
const float HEADING_MAX = PI/4, HEADING_STEP = PI/128;

void setup() {
  WiFi.persistent(false);

  driver.begin();
  of.begin();
  colorSensor.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin("OctoNet", "3ricn3t1");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  communicator.begin();

  heading = 0.f;
  headingDir = HEADING_STEP;

  driver.setSpeed(0.2);
}

void loop() {
  if(colorSensor.update()) {
    auto newFlow = of.get();
    auto timestamp = micros();
    auto movement = driver.update(newFlow);
    auto color = colorSensor.get();
    
    communicator.sendUpdate(movement, color, timestamp);

    heading += headingDir;
    if( (heading > HEADING_MAX) || (heading < -HEADING_MAX) ) {
      heading -= headingDir;

      headingDir *= -1.f;
    }
    driver.setHeading(heading);
  }
}
