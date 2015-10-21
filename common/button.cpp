#include "button.h"

Button::Button()
{
	ddr = 0;
	port = 0;
	pin = 0;
	pinReg = 0;
}

void Button::init(volatile uint8_t* port_, volatile uint8_t* ddr_, volatile uint8_t* pinReg_, uint8_t pin_)
{
	ddr = ddr_;
	port = port_;
	pin = pin_;
	pinReg = pinReg_;
	(*ddr) &= ~(1<<pin); //set pin as input
	(*port) &= ~(1<<pin); //turn off internal pull-up resistor
}

bool Button::isPressed()
{
	bool pressed = ((*pinReg) | (1<< pin));
	return pressed;
}