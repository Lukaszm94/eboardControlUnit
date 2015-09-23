#ifndef PACKETPARSER_H_INCLUDED
#define PACKETPARSER_H_INCLUDED

#include <ctype.h>

#include "packet.h"
#include "myString.h"
#include "debug.h"

#define Cstr_terminator '\0'

class PacketParser
{
public:
    PacketParser()
    {
        parsedBufferPointer = NULL;
    }

    bool correctInputBuffer(MyString *buff)
    {
        int startCharPosition = buff->find('s');
        if(startCharPosition < 0) {
            return false;
        }
        //Debug::print("Trying to correct input buffer, shift left by: ");
        //Debug::println(startCharPosition);

        buff->shiftLeft(startCharPosition);
        return true;
    }

    bool parse(MyString *buff)
    {
        if(buff->size() > minPackLength && buff->at(0) != 's') {
            if(!correctInputBuffer(buff)) {
                Debug::println("ERROR: COULD NOT CORRECT BUFFER!!");
                return false;
            }
        }

        int endCharacterPosition = buff->find('E');
        if((endCharacterPosition > 0) && buff->at(0) == 's' && (buff->size() > endCharacterPosition + 1 )) {
            if(coreParse(buff)) {
                    return true;
            }
        }
        return false;
    }



    Packet getPacket()
    {
        return parsedPacket;
    }

private:
    Packet parsedPacket;
    char* parsedBufferPointer;
    int parsedBufferCurrentIndex;
    int parsedBufferTextLength;

    bool coreParse(MyString *buff)
    {   parsedBufferTextLength = buff->size();
        parsedBufferCurrentIndex = 0;
        parsedBufferPointer = buff->toCstr();

        //Ia
        parsedPacket.Ia = getNextsFloat();
        //Ib
        parsedPacket.Ib = getNextsFloat();
        parsedPacket.Ta = getNextsFloat();
        parsedPacket.Tb = getNextsFloat();
        parsedPacket.Tc = getNextsFloat();
        parsedPacket.Td = getNextsFloat();
        parsedPacket.Te = getNextsFloat();
        parsedPacket.Tf = getNextsFloat();

        shiftToDigit();
        char checksum = extractInt() + '0';
        //Debug::print("Checksum received: ");
        //Debug::println(checksum);
        if(checksum != parsedPacket.generateChecksumDigit()) {
            Debug::println("Checksum incorrect!");
            return false;
        }

        return true;
    }

    bool incrementBufferPointer()
    {
        if(parsedBufferCurrentIndex < parsedBufferTextLength) {
            parsedBufferCurrentIndex++;
            return true;
        } else {
            Debug::println("Parsing error: pointer outside buffer!");
            return false;
        }
    }

    //always moves at least by one char
    void shiftToDigit()
    {
        do {
            incrementBufferPointer();
        }
        while(!isdigit(*getBufferPointer()) && *getBufferPointer() != Cstr_terminator);
    }
    //starts extractin from location str
    int extractInt()
    {
        int number = 0;
        while(*getBufferPointer() != Cstr_terminator && isdigit(*getBufferPointer())) {
            number = number * 10 + *getBufferPointer() - '0';
            incrementBufferPointer();
        }
        return number;
    }

    char* getBufferPointer()
    {
        return (parsedBufferPointer + parsedBufferCurrentIndex);
    }

    sFloat getNextsFloat()
    {
        sFloat result;
        shiftToDigit();
        result.setInteger(extractInt());
        shiftToDigit();
        result.setFractional(extractInt());
        return result;
    }

};

#endif // PACKETPARSER_H_INCLUDED
