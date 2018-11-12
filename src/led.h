// led.h

#ifndef _LED_h
#define _LED_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
     void initLED(void);
     void ledReady(void);
     void ledWarning(void);
     void ledLightning(void);
     void ledDisturber(void);
     void ledNoise(void);
#endif

