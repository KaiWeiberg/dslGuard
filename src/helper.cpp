#include <Arduino.h>

void printValue(const char* text,uint8_t wert){
	Serial.print("\t");
	Serial.print(text);
	Serial.print("\t");
	Serial.print(wert, DEC);
	Serial.print("\t");
	Serial.println(wert, BIN);
}