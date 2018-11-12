#include <Arduino.h>

#ifndef _39535_H
#define _39535_H

    const int addr_cleanInstall = 0;

   extern uint8_t mn_minStrikes;
   extern uint8_t mn_watchdogThreshold;
   extern uint8_t mn_noiseFloor;
   extern uint8_t mn_tunecups;
   extern uint8_t mn_spikeRejection;
   extern uint8_t mn_afe;
   extern uint8_t mn_offTime;
   extern uint8_t mn_minDistance;

    extern volatile bool detected;

    extern void initAS3935(void);
    extern void ASLoop(void);

    extern void print3935Register(void);
    extern void writeVarToAS3935(void);
    extern void readFromAS3935(void);
    extern void autoTuneAS3935(void);
#endif


