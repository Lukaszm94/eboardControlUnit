#include "led.h"

LED::LED()
{
	blinking = false;
	timeCounter = blinkOnTime = blinkPeriod = 0;
	LEDPort = LEDDdr = 0;
}

void LED::init(volatile uint8_t* port_, volatile uint8_t* ddr_, uint8_t pin_)
{
	LEDPort = port_;
	LEDDdr = ddr_;
	LEDPin = pin_;
	(*LEDDdr) |= (1<<LEDPin);
	turnLEDOff();
}

void LED::setSolid(bool lit)
{
	blinking = false;
	if(lit) {
		turnLEDOn();
	} else {
		turnLEDOff();
	}
}

void LED::setBlinking(uint16_t newBlinkPeriodMs, uint16_t newBlinkOnTimeMs) volatile
{
	timeCounter = 0;
	blinkPeriod = newBlinkPeriodMs;
	blinkOnTime = newBlinkOnTimeMs;
	turnLEDOn();
	blinking = true;
}

void LED::update() volatile
{
	if(!blinking) {
		return;
	}
	timeCounter += LED_UPDATE_INTERVAL_MS;
	if(timeCounter >= blinkPeriod) {
		turnLEDOn();
		timeCounter = 0;
	}
	if(timeCounter >= blinkOnTime) {
		turnLEDOff();
	}
}

void LED::turnLEDOff()
{
	(*LEDPort) &= ~(1<<LEDPin);
}

void LED::turnLEDOn()
{
	(*LEDPort) |= (1<<LEDPin);
}

