#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "controlUnit.h"
#include "common/uartReceiver.h"
#include "common/uart.h"
#include "common/glcd.h"
#include "common/label.h"

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
	Label label;
	Debug::println("Initializing label");
	label.init(24, 16);
	Debug::println("Adding label to glcd");
	glcd.addWidget(&label, 0, 0);
	label.setFontMultiplier(2);
	Debug::println("Setting text");
	label.setText("25");
	Debug::println("Redrawing");
	glcd.redraw();
	Debug::println("All done");
	
	/*Widget widget;
	widget.init(2,16);
	widget.outputBuffer[0] = 0xAA;
	widget.outputBuffer[1] = 0xFF;
	widget.outputBuffer[2] = 0x01;
	widget.outputBuffer[3] = 0x80;
	glcd.addWidget(&widget, 0, 0);
	glcd.redraw();*/
	int tens = 0;
	char tab[3];
	tab[2] = '/0';
	while(1)
	{
		for(int i = 0; i < 7; i++) {
			glcd.widgetsData[0].xPos = i*8;
			glcd.widgetsData[0].yPos = i*8;
			GLCD_ClearScreen();
			glcd.redraw();
			_delay_ms(100);
		}
		
		
		
		/*for(int i = 0; i < 10; i++) {
			tab[0] = '0' + tens;
			tab[1] = '0' + i;
			label.setText(tab);
			glcd.redraw();
		}
		tens++;
		if(tens == 10)
			tens = 0;*/
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
