#ifndef UARTRECEIVER_H_INCLUDED
#define UARTRECEIVER_H_INCLUDED

#include "packet.h"
#include "packetParser.h"
#include "debug.h"


#define inputBufferSize 100

class UartReceiver
{
public:
    UartReceiver() : buff(inputBuffer, inputBufferSize)
    {

    }

    void loadNewChar(char newChar)
    {
        buff.append(newChar);
    }

    bool parseBuffer()
    {
        return parser.parse(&buff);
    }

    Packet getPacket()
    {
        return parser.getPacket();
    }
	
	void clearBuffer()
	{
		buff.clear();
	}

private:
    char inputBuffer[inputBufferSize];
    MyString buff;
    PacketParser parser;
    Packet parsedPacket;

};
#endif // UARTRECEIVER_H_INCLUDED
