
// Extendet by Kai Weiberg
//	AS3935 MOD-1016 Lightning Sensor Arduino library
//	Written originally by Embedded Adventures


#include "AS3935.h"
#include "Arduino.h"
#include "Wire.h"
#include "SPI.h"

volatile bool displayingFrequency;
volatile sgn32 pulse = 0;
sgn32 cap_frequencies[16];

void pulseDetected() {
	if (displayingFrequency)
		pulse++;
}

void autoTuneCaps(uns8 irq) {
    // OPERATION TYPE   selector
    // PRECONDITIONS    Die Liste darf nicht leer sein.
    // DESCRIPTION      Verwendet einen Iterator aus der classlib,
    //                  um die unsortierte Liste zu durchsuchen.
    // POSTCONDITIONS   Die L�nge der Liste wird durch die
    //                  Funktion nicht ge�ndert.
    // RETURN VALUE     Der gr��te Wert in der Liste
	int fdiv = mod1016.getDivisionRatio();
	Serial.println("Measuring frequency. Please wait...\n");
	freqPerTuneCaps(fdiv, irq);
	recommendTuning();
	mod1016.calibrateRCO();
	delay(1000);
}

sgn32 getFrequency(uns8 irq) {
	mod1016.writeRegister(DISP_LCO, 0x80);
	displayingFrequency = true;
	pulse = 0;
	attachInterrupt(digitalPinToInterrupt(irq), pulseDetected, RISING);
	delay(200);
	displayingFrequency = false;
	detachInterrupt(irq);
	mod1016.writeRegister(DISP_LCO, 0x00);
	return (pulse * 5);
}

void freqPerTuneCaps(uns16 fdiv, uns8 irq) {
	for (int i = 0; i < 16; i++) {
			mod1016.setTuneCaps(i);
			delay(2);
			/*Measure frequency 5x. Get average.
			If you want to use a slower aproach that measures frequency over a longer period of time, uncomment this section,
			and comment out the other sgn32 freq line. */

			/*
			sgn32 freq = 0;
			for (int i = 0; i < 5; i++) {
			  freq += getFrequency(irq);
			}
			freq = ((freq / 5) * fdiv) - 500000;
			*/

			/*Measure frequency in 1/5th of a second, multiply by 500000*/
		   sgn32 freq = (getFrequency(irq) * fdiv) - 500000;  


			cap_frequencies[i] = (freq < 0) ? -freq : freq;
			//Uncomment to print out frequnecy calculations

//			Serial.print("TUNE CAPS = ");
//			Serial.print(i);
//			Serial.print("\tFrequency - 500k = ");
//			Serial.println(cap_frequencies[i]);
//			Serial.println();
		}
}

void recommendTuning() {
	int best = 0, next, current;
	for (int i = 1; i < 16; i++) {
		current = best;
		next = i;
		best = (cap_frequencies[next] < cap_frequencies[current]) ? next : best; 
	}
	Serial.print("Best value for TUNE_CAPS: ");
	Serial.println(best);
	Serial.println("Setting TUNE_CAPS...");
	mod1016.setTuneCaps(best);
}

/*--------------------------------------------------------*/

void AS3935Class::setMinStrikes(uns8 minStrk)
{
    // DESCRIPTION      This function sets the minmum strikes, see pg 22 of the datasheet for more info (#strikes in 17 min)
    // CONDITIONS    	Options are 1, 5, 9 or 16 strikes.
	if(5 > minStrk)
	{
		writeRegister(MIN_LIGH, 0x00);
	}
	else if(9 > minStrk)
	{
		writeRegister(MIN_LIGH, 0x10);
	}
	else if(16 > minStrk)
	{
		writeRegister(MIN_LIGH, 0x20);
	}
	else
	{
		writeRegister(MIN_LIGH, 0x30);
	}
}

uns8 AS3935Class::getMinStrikes()
{
	uns8 minStrk = readRegister(MIN_LIGH);

	if(minStrk == 0x00)
	{
		return 1;
	}
	else if(minStrk == 0x10)
	{
		return 5;
	}
	else if(minStrk == 0x20)
	{
		return 9;
	}
	else
	{
		return 16;
	}

}

uns8 AS3935Class::getWatchdogThreshold(void)
{
	// This function is used to read WDTH. It is used to increase robustness to disturbers,
	// though will make detection less efficient (see page 19, Fig 20 of datasheet)
	// WDTH register: add 0x01, bits 3:0
	// default value of 0001
	// values should only be between 0x00 and 0x0F (0 and 7)
	return readRegister(WDTH);
}

void AS3935Class::setWatchdogThreshold(uns8 wdTh)
{
	// This function is used to modify WDTH. It is used to increase robustness to disturbers,
	// though will make detection less efficient (see page 19, Fig 20 of datasheet)
	// WDTH register: add 0x01, bits 3:0
	// default value of 0001
	// values should only be between 0x00 and 0x0F (0 and 7)
	 writeRegister(WDTH, wdTh);
}

uns8 AS3935Class::getSpikeRejection()
{
	// This function is used to read SREJ (spike rejection). Similar to the Watchdog threshold,
	// it is used to make the system more robust to disturbers, though will make general detection
	// less efficient (see page 20-21, especially Fig 21 of datasheet)
	// SREJ register: add 0x02, bits 3:0
	// default value of 0010
	// values should only be between 0x00 and 0x0F (0 and 7)
	return readRegister(SREJ);
}

void AS3935Class::setSpikeRejection(uns8 spRej)
{
	// This function is used to modify SREJ (spike rejection). Similar to the Watchdog threshold,
	// it is used to make the system more robust to disturbers, though will make general detection
	// less efficient (see page 20-21, especially Fig 21 of datasheet)
	// WDTH register: add 0x02, bits 3:0
	// default value of 0010
	// values should only be between 0x00 and 0x0F (0 and 7)
	 writeRegister(SREJ, spRej);
}


void AS3935Class::PowerUp(void)
{
	// power-up sequence based on datasheet, pg 23/27
	// register 0x00, PWD bit: 0 (clears PWD)
    writeRegister(PWD, 0);
	calibrateRCO();
}

void AS3935Class::PowerDown(void)
{
	// register 0x00, PWD bit: 0 (sets PWD)
	 writeRegister(PWD, 1);
}



/*--------------------------------------------------------*/



void AS3935Class::init(uns8 irqPin) {
	_usingI2C = true;
	calibrateRCO();
	pinMode(irqPin, INPUT);
}

void AS3935Class::init(uns8 irqPin, uns8 csPin) {
	_usingI2C = false;
	_irq = irqPin;
	_cs = csPin;
	pinMode(_cs, OUTPUT);
	digitalWrite(_cs, HIGH);
	calibrateRCO();
	pinMode(_irq, INPUT);
	SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE2));
}

void AS3935Class::init(uns8 irqPin, uns8 clkPin, uns8 mosiPin, uns8 misoPin, uns8 csPin) {
	_usingI2C = false;
	_irq = irqPin;
	_clk = clkPin;
	_mosi = mosiPin;
	_miso = misoPin;
	_cs = csPin;
	
	pinMode(_clk, OUTPUT);
	pinMode(_mosi, OUTPUT);
	pinMode(_miso, INPUT);
	pinMode(_cs, OUTPUT);
	digitalWrite(_cs, HIGH);
	calibrateRCO();
	pinMode(_irq, INPUT);
	SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE2));
	
}

void AS3935Class::calibrateRCO() {
	if (_usingI2C) {
		Wire.beginTransmission(AS3935_ADDR);
		Wire.write(0x3D);
		Wire.write(0x96);
		Wire.endTransmission();
	}
	else {
		digitalWrite(_cs, LOW);
		SPI.transfer(AS3935_SPI_WRITE & 0x3D);
		SPI.transfer(0x96);
		digitalWrite(_cs, HIGH);
	}
	writeRegister(DISP_TRCO, (0x01 << 5));
	delay(2);
	writeRegister(DISP_TRCO, (0x00 << 5));
}

uns8 AS3935Class::readRegisterRaw(uns8 reg) {
	uns8 result;
	if (_usingI2C) {
		Wire.beginTransmission(AS3935_ADDR);
		Wire.write(reg);
		Wire.endTransmission(false);
		Wire.requestFrom(AS3935_ADDR, 1);
		if (Wire.available()) {
			result = Wire.read();
		}
	}
	else {
		reg &= 0x3F;
		reg |= AS3935_SPI_READ;
		digitalWrite(_cs, LOW);
		SPI.transfer(reg);
		result = SPI.transfer(0x00); 
		digitalWrite(_cs, HIGH);
	}
	return result;
}

uns8 AS3935Class::readRegister(uns8 reg, uns8 mask) {
	uns8 data = readRegisterRaw(reg);
	return (data & mask);
}

void AS3935Class::writeRegister(uns8 reg, uns8 mask, uns8 data) {
	uns8 currentReg = readRegisterRaw(reg);

//	Serial.print("Register:     ");
//	Serial.println(reg,HEX);
//	Serial.print("Data:         ");
//	Serial.println(data,HEX);
//	Serial.print("RegVal:       ");
//	Serial.println(currentReg,HEX);
//	Serial.print("Mask:       ");
//	Serial.println(mask,HEX);
	currentReg = currentReg & (~mask);
	data |= currentReg;
//	Serial.print("NewData:      ");
//	Serial.println(data,HEX);
	if (_usingI2C) {
		Wire.beginTransmission(AS3935_ADDR);
		Wire.write(reg);
		Wire.write(data);
//		Serial.print("Success:      ");
//		Serial.println(Wire.endTransmission(),HEX);
//		Serial.println("");
		Wire.endTransmission();
	}
	else {
		reg &= AS3935_SPI_WRITE;
		digitalWrite(_cs, LOW);
		SPI.transfer(reg);
		SPI.transfer(data);
		digitalWrite(_cs, HIGH);
	}
}

void AS3935Class::setIndoors() {
	writeRegister(AFE_GB, INDOORS);
}

void AS3935Class::setOutdoors() {
	writeRegister(AFE_GB, OUTDOORS);
}

void AS3935Class::setNoiseFloor(uns8 noise) {
	writeRegister(NOISE_FLOOR, (noise << 4));
}

void AS3935Class::setTuneCaps(uns8 tune) {
	writeRegister(TUNE_CAPS, tune);
	delay(2);
}

void AS3935Class::enableDisturbers() {
	writeRegister(MASK_DST, (0 << 5));
	delay(2);
}

void AS3935Class::disableDisturbers() {
	writeRegister(MASK_DST, (1 << 5));
	delay(2);	
}

uns8 AS3935Class::getNoiseFloor() {
	return (readRegister(NOISE_FLOOR) >> 4);
}

uns8 AS3935Class::getAFE() {
	return (readRegister(AFE_GB) >> 1);
}

uns8 AS3935Class::getTuneCaps() {
	return readRegister(TUNE_CAPS);
}

uns8 AS3935Class::getIRQ() {
	delay(2);
	return readRegister(IRQ_TBL);
}

uns8 AS3935Class::getLightDistance() {
	return readRegister(LGHT_DIST);
}

sgn16 AS3935Class::calculateDistance() {
	uns8 dist = getLightDistance();
	sgn16 km;		
	switch (dist) {
		case 0x3F:
			km = -1;
			break;
		case 0x28:
			km = 40;
			break;
		case 0x25:
			km = 37;
			break;
		case 0x22:
			km = 34;
			break;
		case 0x1F:
			km = 31;
			break;
		case 0x1B:
			km = 27;
			break;
		case 0x18:
			km = 24;
			break;
		case 0x14:
			km = 20;
			break;
		case 0x11:
			km = 17;
			break;
		case 0x0E:
			km = 14;
			break;
		case 0x0C:
			km = 12;
			break;
		case 0x0A:
			km = 10;
			break;
		case 0x08:
			km = 8;
			break;
		case 0x06:
			km = 6;
			break;
		case 0x05:
			km = 5;
			break;
		case 0x01:
			km = 0;
			break;
		default:
			km = 1;
	}
	return km;
}

sgn16 AS3935Class::getDivisionRatio() {
	uns8 fdiv = (readRegister(LCO_FDIV) >> 6);
	if (fdiv == 0)
		return 16;
	else if (fdiv == 1)
		return 32;
	else if (fdiv == 2) 
		return 64;
	else	
		return 128;
}

uns16 AS3935Class::getIntensity() {
	uns8 lsb, msb, mmsb;
	lsb = readRegisterRaw(0x04);
	msb = readRegisterRaw(0x05);
	mmsb = readRegisterRaw(0x06) & 0x1F;
	uns16 result = (uns16)lsb | ((uns16)msb << 8) | ((uns16)mmsb << 16);
	return result;
}

AS3935Class mod1016;
