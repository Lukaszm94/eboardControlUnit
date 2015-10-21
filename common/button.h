#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

class Button
{
public:
	Button();
	void init(volatile uint8_t* port_, volatile uint8_t* ddr_, volatile uint8_t* pinReg_, uint8_t pin_);
	bool isPressed();
private:
	volatile uint8_t* port;
	volatile uint8_t* ddr;
	volatile uint8_t* pinReg;
	uint8_t pin;

};

#endif