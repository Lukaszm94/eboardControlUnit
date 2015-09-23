#ifndef PACKET_H_INCLUDED
#define PACKET_H_INCLUDED

#include "sFloat.h"

const int minPackLength = 43;

class Packet
{
public:
    Packet(float intValue = 0.0)
    {
        Ia = Ib = Ta = Tb = Tc = Td = Te = Tf = intValue;
    }

    void setRandomValues()
    {

    }

    char generateChecksumDigit()
    {
        int integerSum = 0, fractionalSum = 0;
        integerSum = Ia.getInteger() + Ib.getInteger() + Ta.getInteger() + Tb.getInteger() + Tc.getInteger() + Td.getInteger() + Te.getInteger() + Tf.getInteger();
        fractionalSum = Ia.getFractional() + Ib.getFractional() + Ta.getFractional() + Tb.getFractional() + Tc.getFractional() + Td.getFractional() + Te.getFractional() + Tf.getFractional(); //TODO works erroneously when sFloat is negative

        integerSum += fractionalSum/10;
        fractionalSum = fractionalSum % 10;

        int digitsSum = sumDigits(integerSum) + fractionalSum;
        digitsSum = digitsSum % 10;
        char checksumDigit = '0' + digitsSum;
        return checksumDigit;
    }

    sFloat Ia, Ib, Ta, Tb, Tc, Td, Te, Tf;

private:

    int sumDigits(unsigned long long n, const int base = 10) {
        int sum = 0;
        for (; n; n /= base)
            sum += n % base;
        return sum;
    }
};


#endif // PACKET_H_INCLUDED
