#ifndef UARTSENDER_H_INCLUDED
#define UARTSENDER_H_INCLUDED

#include "packet.h"
#include "sFloat.h"
#include "myString.h"

const int maxNumberLength = 5;
const int outputBufferSize = 60;

class uartSender
{
public:
    uartSender() : buff(outputBuffer, outputBufferSize)
    {

    }

    char* getPacketCharString(Packet pack)
    {
		buff.clear();
        buff.append('s');
        buff.append("Ia");
        buff.append(pack.Ia);
        buff.append("Ib");
        buff.append(pack.Ib);
        buff.append("Ta");
        buff.append(pack.Ta);
        buff.append("Tb");
        buff.append(pack.Tb);
        buff.append("Tc");
        buff.append(pack.Tc);
        buff.append("Td");
        buff.append(pack.Td);
        buff.append("Te");
        buff.append(pack.Te);
        buff.append("Tf");
        buff.append(pack.Tf);
        buff.append('E');
        buff.append(pack.generateChecksumDigit());
        return outputBuffer;
    }

private:
    char outputBuffer[outputBufferSize];
    MyString buff;


};

#endif // UARTSENDER_H_INCLUDED
