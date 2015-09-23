#ifndef SFLOAT_H_INCLUDED
#define SFLOAT_H_INCLUDED

#define Cstr_terminator '\0'

/**
* class used to store fixed point numbers in format xy.z or x.y (from -99.9 to 99.9)
*
*
*
*
*
*
*
*/

const int charBufferSize = 5;
const int maxIntegerValue = 99;

class sFloat
{
public:
    sFloat(int intPart = 0, int fracPart = 0)
    {
        integer = intPart;
        fractional = fracPart;
    }
	
	sFloat(float initValue)
	{
		setInteger((int) initValue);
		setFractional((int)(initValue * 10 + 0.5) % 10); // 0.5 is added to avoid floor rounding error
	}

    void setInteger(int intPart)
    {
        if(intPart > maxIntegerValue) {
            intPart = maxIntegerValue;
        }
        integer = intPart;
    }

    void setFractional(int fracPart)
    {
        if(fracPart > 9) {
            fracPart = 9;
        } else if(fracPart < 0) {
            fracPart = 0;
        }

        fractional = fracPart;
    }

    int getInteger()
    {
        return integer;
    }

    int getFractional()
    {
        return fractional;
    }
	
	float toFloat()
	{
		float result = integer + (float)fractional/10;
		return result;
	}

    char* toString()
    {
        int index = 0;
        if(integer > 9) {
            outputBuffer[index] = (char)(integer/10) + '0';
            index++;
        }
        outputBuffer[index] = (char)integer%10 + '0';
        index++;
        outputBuffer[index] = '.';
        index++;
        outputBuffer[index] = (char)fractional + '0';
        index++;
        outputBuffer[index] = Cstr_terminator;


        return outputBuffer;
    }

private:
    int integer;
    unsigned int fractional;
    char outputBuffer[charBufferSize];

};


#endif // SFLOAT_H_INCLUDED
