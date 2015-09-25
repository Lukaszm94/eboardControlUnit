#ifndef RGBLED_H
#define RGBLED_H

#include "avriomanip.h"

#define COLORS_COUNT 3
#define RGBLED_UPDATE_INTERVAL_MS 200

enum {COMMON_ANODE = 0, COMMON_CATHODE};
enum {RED = 0, GREEN, BLUE};

class RGBLed
{
public:
	RGBLed()
	{
		blinkCounter = 0;
		blinking = false;
	}
	
	void init(volatile uint8_t* port_, volatile uint8_t* ddr_, uint8_t* pins_,  uint8_t ledType = COMMON_ANODE)
	{
		port = port_;
		for(int i = 0; i < COLORS_COUNT; i++) {
			pins[i] = pins_[i];
			(*ddr_) |= (1<<pins[i]);
		};
	}
	
	void update()
	{
		if(!blinking) {
			return;
		}
		blinkCounter += RGBLED_UPDATE_INTERVAL_MS;
		
		if(blinkCounter >= blinkOnTime) {
			//turn led off
		}
		if(blinkCounter >= blinkPeriodMs) {
			blinkCounter = 0;
			//turn led on
		}
		
	}
	
	void setBlinking(uint8_t color, uint16_t periodMs, uint8_t dutyCyclePercent)
	{
		singleLedOn(color); //leave only desired color on
		blinking = true;
		currentColor = color;
		blinkPeriodMs = periodMs;
		blinkOnTime = (blinkPeriodMs*dutyCyclePercent)/100;
		blinkCounter = 0;
	}
	
	void setSolid(uint8_t color)
	{
		singleLedOn(color);
		blinking = false;
		currentColor = color;
	}
private:
	bool isCurrentColorOn()
	{
		return ((*port) & pin(pins[currentColor]));
	}
	
	void singleLedOn(uint8_t color)
	{
		for(int i = 0; i < COLORS_COUNT; i++) {
			if(i == color) {
				turnLedOn(i);
			} else {
				turnLedOff(i);
			}
		}
	}
	
	void turnLedOn(uint8_t i)
	{
		*port |= pin(pins[i]);
	}
	
	void turnLedOff(uint8_t i)
	{
		*port &= ~pin(pins[i]);
	}

	volatile uint8_t* port;
	uint8_t pins[COLORS_COUNT];
	uint8_t currentColor;
	bool blinking;
	uint16_t blinkPeriodMs;
	uint16_t blinkCounter;
	uint16_t blinkOnTime;
};

#endif