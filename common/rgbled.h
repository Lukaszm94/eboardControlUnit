#ifndef RGBLED_H
#define RGBLED_H

#include "avriomanip.h"

#define COLORS_COUNT 3
#define RGBLED_UPDATE_INTERVAL_MS 200
#define PIN_UNASSIGNED 255

enum {COMMON_ANODE = 0, COMMON_CATHODE};
enum {RED = 1, GREEN = 2, BLUE = 4, YELLOW};

class RGBLed
{
public:
	RGBLed()
	{
		blinkCounter = 0;
		blinking = false;
		currentColor = 0;
		blinkPeriodMs = 0;
	}
	
	void init(volatile uint8_t* port_, volatile uint8_t* ddr_, uint8_t* pins_,  uint8_t ledType = COMMON_ANODE)
	{
		port = port_;
		for(int i = 0; i < COLORS_COUNT; i++) {
			pins[i] = pins_[i];
			if(pins[i] != PIN_UNASSIGNED) {
				(*ddr_) |= (1<<pins[i]);
				turnLedOff(i);
			}
		};
	}
	
	void update()
	{
		if(!blinking) {
			return;
		}
		blinkCounter += RGBLED_UPDATE_INTERVAL_MS;
		
		if(blinkCounter >= blinkOnTime) {
			setColor(0);
		}
		if(blinkCounter >= blinkPeriodMs) {
			blinkCounter = 0;
			setColor(currentColor);
		}
		
	}
	
	void setBlinking(uint8_t color, uint16_t periodMs, uint8_t dutyCyclePercent)
	{
		if(color == YELLOW) {
			//turn on multiple LEDs
			color = RED | GREEN;
		}
		
		if(currentColor == color && blinkPeriodMs == periodMs) {
			return;
		}
		
		setColor(color); //leave only desired colors on
		blinking = true;
		currentColor = color;
		blinkPeriodMs = periodMs;
		blinkOnTime = (((uint32_t)blinkPeriodMs)*dutyCyclePercent)/100;
		blinkCounter = 0;
	}
	
	void setSolid(uint8_t color)
	{
		if(color == YELLOW) {
			//TODO turn on multiple LEDs
			//temp workaround:
			color = RED | GREEN;
		}
		setColor(color);
		blinking = false;
		currentColor = color;
	}
private:
	bool isCurrentColorOn()
	{
		return ((*port) & pin(pins[currentColor]));
	}
	
	void setColor(uint8_t color)
	{
		for(int i = 0; i < COLORS_COUNT; i++) {
			if(color & (1<<i)) {
				turnLedOn(i);
			} else {
				turnLedOff(i);
			}
		}
	}
	
	void turnLedOn(uint8_t i)
	{
		if(i == PIN_UNASSIGNED) {
			return;
		}
		*port &= ~pin(pins[i]);
	}
	
	void turnLedOff(uint8_t i)
	{
		if(i == PIN_UNASSIGNED) {
			return;
		}
		*port |= pin(pins[i]);
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