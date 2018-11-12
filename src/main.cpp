#include <Arduino.h>
#include <Wire.h>
#include "OneButton.h"
#include "SimpleTimer.h"

const int buttonOnePin = 30;
OneButton buttonOne(buttonOnePin,true,true);

SimpleTimer timer;

void buttonOnePressed()
{
	Serial.println("Button pressed");
}


void setup() {
  Serial.begin(9600);
	while (!Serial) {};  
	Serial.println("");
	Serial.println("initialize dslGuard");

  buttonOne.attachClick(buttonOnePressed);


}

void loop() {
  buttonOne.tick();
  timer.run();
}