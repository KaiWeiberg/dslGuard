
// Extendet by Kai Weiberg

//	AS3935 MOD-1016 Lightning Sensor Arduino library
//	Written originally by Embedded Adventures


#ifndef __AS3935_h
#define __AS3935_h

#include <Arduino.h>
	
#ifndef		uns8	
	#define uns8	uint8_t
#endif
#ifndef		uns16
	#define uns16	uint16_t
#endif
#ifndef		sgn16
	#define sgn16	int16_t
#endif
#ifndef		sgn32	
	#define sgn32	int32_t
#endif
#ifndef		sgn64
	#define sgn64	int64_t
#endif

#define	AS3935_SPI_READ		0x40
#define AS3935_SPI_WRITE	0x3F 

#define AS3935_ADDR 	0x03
#define INDOORS 		0x24
#define OUTDOORS 		0x1C

//			 Reg address	bitmask
#define DISP_LCO	0x08,	0x80
#define TUNE_CAPS	0x08,	0x0F
#define AFE_GB		0x00,	0x3E
#define NOISE_FLOOR	0x01,	0x70
#define MASK_DST	0x03,	0x20
#define IRQ_TBL		0x03,	0x0F
#define LCO_FDIV	0x03,	0xC0
#define LGHT_DIST	0x07,	0x3F
#define DISP_TRCO	0x08,	0x20

#define PWD			0x00,	0x01
#define MIN_LIGH	0x02,	0x30
#define WDTH		0x01, 	0x0F
#define SREJ		0x02, 	0x0F

//Set of functions for auto calibration of TUNE_CAPS

/*Automatically sets TUNE_CAPS to the best possible setting, ie the one that producs a frequency closest to 500kHz*/
void autoTuneCaps(uns8 irq);
/*ISR for measuring the frequency output on the IRQ pin*/
void pulseDetected();
/*Generate the frequency output on IRQ pin and measure number of pulses in 200ms. Returns number of pulses * 5*/
sgn32 getFrequency(uns8 irq);
/*Measure frequency output under setting 0-15 on TUNE_CAPS*/
void freqPerTuneCaps(uns16 fdiv, uns8 irq);
/*Search for the tuning whose frequency is closest to 500kHz, and sets it to that*/
void recommendTuning();

void auto_calibrate(uns8 irq);

//End of auto-calibration functions

class AS3935Class
{
private:
	uns8	_irq;
	uns8	_clk;
	uns8	_mosi;
	uns8	_miso;
	uns8	_cs;
	bool	_usingI2C;
	uns8 readRegisterRaw(uns8 reg);
	
public:
	void init(uns8 irqPin);
	void init(uns8 irqPin, uns8 csPin);
	void init(uns8 irqPin, uns8 clkPin, uns8 mosiPin, uns8 misoPin, uns8 csPin);
	void calibrateRCO();
	void writeRegister(uns8 reg, uns8 mask, uns8 data);
	void setIndoors();
	void setOutdoors();
	void setNoiseFloor(uns8 noise);
	void setTuneCaps(uns8 tune);
	void enableDisturbers();
	void disableDisturbers();
	uns8 readRegister(uns8 reg, uns8 mask);
	uns8 getNoiseFloor();
	uns8 getAFE();
	uns8 getTuneCaps();
	uns8 getIRQ();
	uns8 getLightDistance();
	//Values for KM -> -1 = out of range, 0 = overhead, 1 = not in table
	sgn16  calculateDistance();
	sgn16  getDivisionRatio();
	uns16  getIntensity();

	void setMinStrikes(uns8 minStrk);
	uns8 getMinStrikes();
	void setWatchdogThreshold(uns8 wdTh);  	// between 0 and 7, default 2. It is used to increase robustness to disturbers,
	uns8 getWatchdogThreshold();
	void setSpikeRejection(uns8 spRej); 	// between 0 and 7, default 2. It is used to increase robustness to disturbers,
	uns8 getSpikeRejection();
	void PowerUp(void);
	void PowerDown(void);
};

extern AS3935Class mod1016;

#endif
