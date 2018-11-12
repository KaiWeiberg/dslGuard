#include "EEPROMex.h"
#include "helper.h"
#include "3935.h"


const int addr_tunecups = 1;
const int addr_afe = 2;
const int addr_noiseFloor = 3;
const int addr_minStrikes = 4;
const int addr_watchdogThreshold = 5;
const int addr_spikeRejection = 6;
const int addr_offTime = 7;
const int addr_minDistance = 8;


void printEprom()
{
	Serial.println("EEPROM SETUP");
	printValue("Tuning:          ",EEPROM.readByte(addr_tunecups));
	printValue("Innen/Aussen:    ",EEPROM.readByte(addr_afe));
	printValue("Noisefloor:      ",EEPROM.readByte(addr_noiseFloor));
	printValue("Minimum Lightings:",EEPROM.readByte(addr_minStrikes));
	printValue("Watchdog Treshh.: ",EEPROM.readByte(addr_watchdogThreshold));
	printValue("Spike Rejection:  ",EEPROM.readByte(addr_spikeRejection));
	printValue("offTime:          ",EEPROM.readByte(addr_offTime));
	printValue("minDistance:      ",EEPROM.readByte(addr_minDistance));
	Serial.println("");	
}


void writeVarToEprom()
{
	Serial.println("write Var to Eprom");
	EEPROM.updateByte(addr_tunecups,mn_tunecups);
	EEPROM.updateByte(addr_afe,mn_afe);	
	EEPROM.updateByte(addr_noiseFloor,mn_noiseFloor);	
	EEPROM.updateByte(addr_minStrikes,mn_minStrikes);
	EEPROM.updateByte(addr_watchdogThreshold,mn_watchdogThreshold);
	EEPROM.updateByte(addr_spikeRejection,mn_spikeRejection);
	EEPROM.updateByte(addr_offTime,mn_offTime);
	EEPROM.updateByte(addr_minDistance,mn_minDistance);
}


void readVarFromEprom()
{
	Serial.println("Read Var from Eprom");
	mn_tunecups = EEPROM.readByte(addr_tunecups);	
	mn_afe = EEPROM.readByte(addr_afe);	
	mn_noiseFloor = EEPROM.readByte(addr_noiseFloor);	
	mn_minStrikes = EEPROM.readByte(addr_minStrikes);
	mn_watchdogThreshold = EEPROM.readByte(addr_watchdogThreshold);
	mn_spikeRejection = EEPROM.readByte(addr_spikeRejection);
	mn_offTime = EEPROM.readByte(addr_offTime);
	mn_minDistance = EEPROM.readByte(addr_minDistance);	
}




