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
 * done - displaying data on glcd
 * done - modify timer setup
 * - assign correct ports,pins to rgbs and mosfet
 */

 
ControlUnit CU;
Timer0 timer;
volatile bool interruptFlag = false;

extern "C" void __cxa_pure_virtual(void){};

char uart1GetChar()
{
	char c = uart1_getc() & 0x00FF;
	return c;
}

char uartGetChar()
{
	char c = uart_getc() & 0x00FF;
	return c;
}

void clearUartBuffers()
{
	while(uart_available()) {
		uart_getc();
	}
	while(uart1_available()) {
		uart1_getc();
	}
}


int main(void)
{
	UartReceiver rx;
	Packet receivedPacket;
	
	sei();
	//serial for communication with DU
	DDRE |= (1<<PE1);
	uart_init(UART_BAUD_SELECT(9600, F_CPU));
	//serial for receiving data from GPS and sending debug data to PC
	DDRD |= (1<<PD3);
	uart1_init(UART_BAUD_SELECT(57600, F_CPU));
	clearUartBuffers();
	Debug::println("UART initialized");
	
	
	CU.init();
	Debug::println("CU init finished");
	
	CU.startupSequence();
	Debug::println("Startup sequence finished");
	timer.init();
	timer.start();
	
	
	while(1)
	{
		while(uart1_available()) {
			CU.newGPSChar(uart1GetChar());
		}
	
		if(interruptFlag) {
			interruptFlag = false;
			CU.update();
		}
		
		if(uart_available())
		{
			char uartChar = uartGetChar();
			rx.loadNewChar(uartChar);
			while(uart_available()) {
				rx.loadNewChar(uartGetChar());
			}
			if(rx.parseBuffer()) {
				receivedPacket = rx.getPacket();
				Debug::println("New packet");
				rx.clearBuffer();
				CU.onNewPacketReceived(&receivedPacket);
			}
		}
	}
	return 0;
}

ISR(TIMER0_COMPA_vect)
{
	//PORTC ^= (1<<testLedPin);
	interruptFlag = true;
}
