#include "led.h"
#include "SimpleTimer.h"

extern SimpleTimer timer;

const int pinReady = 7;		//Green
const int pinWarning = 8; 	//Red
const int pinLightning = 6;		//White
const int pinDisturber = 5; // blue
const int pinNoise = 4;		// yellow


void initLED() {
	pinMode(pinReady, OUTPUT);
	pinMode(pinWarning, OUTPUT);
	pinMode(pinLightning, OUTPUT);
	pinMode(pinDisturber, OUTPUT);
	pinMode(pinNoise, OUTPUT);
}


void ledOff() {
	digitalWrite(pinReady,LOW);
}


void ledReady() {
	digitalWrite(pinReady,LOW);
	timer.setTimeout(2000, ledOff);
}

void ledLightning() {
	digitalWrite(pinLightning,HIGH);
	timer.setTimeout(2000, ledOff);
}

void ledDisturber() {
	digitalWrite(pinDisturber,HIGH);
	timer.setTimeout(2000, ledOff);
}

void ledNoise() {
	digitalWrite(pinNoise,HIGH);
	timer.setTimeout(1000, ledOff);
}

void ledWarning() {
	digitalWrite(pinWarning,HIGH);
	timer.setTimeout(2000, ledOff);
}