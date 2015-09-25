#include <avr/io.h>
#include <util/delay.h>
#include "controlUnit.h"
#include "common/uartReceiver.h"

/*
 * TODO:
 * done- measuring CU's battery voltage
 * - displaying data on glcd
 * - modify timer setup
 * - assign correct ports,pins to rgbs and mosfet
 */

 
ControlUnit CU;
Timer0 timer;
volatile bool interruptFlag = false;

extern "C" void __cxa_pure_virtual(void){};

char uartGetChar()
{
	char c = uart_getc() & 0x00FF;
	return c;
}

int main(void)
{
	uartReceiver rx;
	Packet receivedPacket;
	
	CU.init();
	timer.init();
	timer.start();
	
	while(1)
	{
		if(interruptFlag) {
			interruptFlag = false;
			CU.update();
		}
		
		if(uart_available())
		{
			rx.receiveNewChar(uartGetChar());
			while(uart_available()) {
				rx.receiveNewChar(uartGetChar());
			}
			if(rx.parseBuffer()) {
				receivedPacket = rx.getPacket();
				rx.clear();
				CU.onNewPacketReceived(&receivedPacket);
			}
		}
	}
	return 0;
}

/*ISR(TIMER0_COMPA_vect)
{
	//PORTC ^= (1<<testLedPin);
	interruptFlag = true;
}*/
