#ifndef AVRIOMANIP_H
#define AVRIOMANIP_H

#define __DDR(pad) DDR ## pad
#define DDR(pad) __DDR(pad)
//#define DDR(name) _DDR(PAD_##name)

#define __PORT(pad) PORT ## pad
#define PORT(pad) __PORT(pad)
//#define PORT(name) _PORT(PAD_##name)

#define __PIN(pad) PIN ## pad
#define _PIN(pad) __PIN(pad)
#define PIN(name) _PIN(PAD_##name)

uint8_t pin(uint8_t pin_)
{
	return (1<<pin_);
}

#endif