#include <Arduino.h>
#include "3935.h"
#include <AS3935.h>
#include <EEPROMex.h>
#include "helper.h"

#include "eprom.h"
#include "led.h"
//#include "rtc.h"


const uint8_t AS3935_IRQ_pin = 2;
volatile bool detected = false;

uint8_t mn_tunecups = 1;
uint8_t mn_afe = 18;
uint8_t mn_noiseFloor = 5;
uint8_t mn_minStrikes = 1;
uint8_t mn_watchdogThreshold =2;  		// between 0 and 7, default 2. It is used to increase robustness to disturbers,
uint8_t mn_spikeRejection = 2; 	// between 0 and 7, default 2. It is used to increase robustness to disturbers,
uint8_t mn_offTime=20;
uint8_t mn_minDistance = 16;



void printDistance()
{
	int distance = mod1016.calculateDistance();
	if (distance == -1)
	Serial.println("Lightning out of range");
	else if (distance == 1)
	Serial.println("Distance not in table");
	else if (distance == 0)
	Serial.println("Lightning overhead");
	else {
		Serial.print("Lightning ~");
		Serial.print(distance);
		Serial.println("km away");
		Serial.print("Intensity: ");
		Serial.println(mod1016.getIntensity());
	}
	Serial.println("");
}


void AS_IRQ_activated ()
{
	detected = true;
}

void impulsAnalyse()
{
	int irq = mod1016.getIRQ();
	Serial.print(irq,DEC);
	switch(irq) {
		case 1:
		Serial.println("  Noise level too high");
		Serial.println("");
		ledNoise();
		break;
		case 4:
		Serial.println("  Disturber detected");
		Serial.println("");
		ledDisturber();
		break;
		case 8:
		Serial.println("  LIGHTNING DETECTED");
		printDistance();
		ledLightning();
		break;
		default:
		printf("   IRQ: ");
		Serial.println(irq);
		break;
	}
}

void ASLoop()
{
	if (detected) { // Gewitter wurde gemeldet
		Serial.println("Event");
//		printDate();
		impulsAnalyse();
		//  stepper.moveDegrees(moveClockwise, 180);
		//  stepper.stop();
		//  stepper.powerOff();
		detected = false;
	}
}


void autoTuneAS3935()
{
	autoTuneCaps(AS3935_IRQ_pin);
	mn_tunecups = mod1016.getTuneCaps();
//	EEPROM.updateByte(addr_tunecups,mn_tunecups);
}


void print3935Register()
{
	Serial.println("AS3935 SETUP:");
	printValue("Tuning:          ", mod1016.getTuneCaps());
	printValue("Innen/Aussen:    ", mod1016.getAFE());
	printValue("Noisefloor:      ", mod1016.getNoiseFloor());
	printValue("Minimum Lightings:",mod1016.getMinStrikes());
	printValue("Watchdog Treshh.: ",mod1016.getWatchdogThreshold());
	printValue("Spike Rejection:  ",mod1016.getSpikeRejection());   //wenn dieses Register gelesen wird! funktiniert der chip nicht
	Serial.println("");
}



void readFromAS3935() {
	Serial.println("Read AS3935 to Var");
	mn_tunecups = mod1016.getTuneCaps();
	mn_afe = mod1016.getAFE();
	mn_noiseFloor = mod1016.getNoiseFloor();
	//mn_minStrikes = mod1016.getMinStrikes(); 	//dieser Registerwert wird nicht verwendet. 
	mn_watchdogThreshold = mod1016.getWatchdogThreshold();
	mn_spikeRejection = mod1016.getSpikeRejection();     //wenn dieses Register gelesen wird! funktiniert der chip nicht
}


void writeVarToAS3935() {
	Serial.println("write to AS3935");
	//mod1016.setTuneCaps(mn_tunecups);
	//mod1016.setIndoors();
	mod1016.setOutdoors();
	// mod1016.setMinStrikes(0); 						    // default 1
	mod1016.setNoiseFloor(mn_noiseFloor); 					// between 0 and 7, default 2.
	mod1016.setWatchdogThreshold(mn_watchdogThreshold); 	// between 0 and 7, default 2. It is used to increase robustness to disturbers,
	mod1016.setSpikeRejection(mn_spikeRejection); 		    // between 0 and 7, default 2. It is used to increase robustness to disturbers, 
	print3935Register();
}


void initAS3935(){
	//	Wire.begin();

	mod1016.init(AS3935_IRQ_pin);   // Board schickt bei Gewittern Interrupts an Pin 2
	autoTuneCaps(AS3935_IRQ_pin);	

	if (EEPROM.readByte(addr_cleanInstall)!=170){
		Serial.println("First Run: initialize eeprom");
		writeVarToEprom();
		EEPROM.writeByte(addr_cleanInstall,170);
	}else{
		readVarFromEprom();
	}

	writeVarToAS3935();

	pinMode(AS3935_IRQ_pin, INPUT); 
	attachInterrupt(digitalPinToInterrupt(AS3935_IRQ_pin), AS_IRQ_activated, RISING);
	mod1016.getIRQ();
}

