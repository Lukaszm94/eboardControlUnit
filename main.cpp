#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "controlUnit.h"
#include "common/uartReceiver.h"
#include "common/uart.h"
#include "common/glcd.h"
#include "common/label.h"
#include "common/eboardgui.h"

#include "common/Timer1.h"
#include "common/debug.h"

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

/*char uart1GetChar()
{
	char c = uart1_getc() & 0x00FF;
	return c;
}

char uartGetChar()
{
	char c = uart_getc() & 0x00FF;
	return c;
}*/

//#define F_CPU 16000000L
int main(void)
{
	/*uartReceiver rx;
	Packet receivedPacket;
	
	//serial for communication with DU
	//DDRE |= (1<<PE1);
	uart_init(UART_BAUD_SELECT(9600, F_CPU));
	//serial for receiving data from GPS and sending debug data to PC
	//DDRD |= (1<<PD3);
	uart1_init(UART_BAUD_SELECT(9600, F_CPU));
	
	CU.init();
	
	timer.init();
	timer.start();
	*/
	Debug::init();
	sei();
	GLCD glcd;
	glcd.init();
	EboardGUI gui;
	gui.init();
	gui.addGUIToDisplay(&glcd);
	glcd.redraw();
	
	while(1)
	{
		
		
		
		
		/*while(uart1_available()) {
			CU.newGPSChar(uart1GetChar());
		}
	
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
		}*/
	}
	return 0;
}

ISR(TIMER0_COMPA_vect)
{
	//PORTC ^= (1<<testLedPin);
	interruptFlag = true;
}
