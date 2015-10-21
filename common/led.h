#ifndef LED_H
#define LED_H
#include <stdint.h>

#define LED_UPDATE_INTERVAL_MS 10

class LED
{
public:
	LED();
	void init(volatile uint8_t* port_, volatile uint8_t* ddr_, uint8_t pin_);
	void setSolid(bool lit);
	void setBlinking(uint16_t newBlinkPeriodMs, uint16_t newBlinkOnTimeMs);
	void update(); //called from ISR
	
private:
	void turnLEDOff();
	void turnLEDOn();
	
	//io variables
	volatile uint8_t* LEDPort;
	volatile uint8_t* LEDDdr;
	uint8_t LEDPin;

	//blinking variables
	bool blinking;
	uint16_t blinkPeriod;
	uint16_t blinkOnTime;
	uint16_t timeCounter; //stores time in ms
};

#endif