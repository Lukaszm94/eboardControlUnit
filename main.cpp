#include <avr/io.h>
#include <util/delay.h>

/*
 * TODO:
 * - measuring CU's battery voltage
 * - displaying data on glcd
 *
 */

ControlUnit CU;

int main(void)
{

	CU.init();
	
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


