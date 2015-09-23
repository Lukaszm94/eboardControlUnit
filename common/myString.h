#ifndef MYSTRING_H_INCLUDED
#define MYSTRING_H_INCLUDED

#include <string.h>
#include "debug.h"

#define Cstr_terminator '\0'

using namespace std;

class MyString
{
public:
    MyString(char* buff, int buffSize)
    {
        buffer = buff;
        bufferSize = buffSize;
        index = 0;
    }

    void clear()
    {
        index = 0;
    }

    void append(int number)
    {

    }

    void append(sFloat number)
    {
        strcpy((buffer + index), number.toString());
        updateEndPointer();
    }

    void append(char letter)
    {
        *(buffer + index) = letter;
        incrementBufferEndPointer();
        *(buffer + index) = Cstr_terminator;
    }

    void append(char* str)
    {
        int stringLength = strlen(str);
        if(stringLength + index >= bufferSize ) {
            Debug::print("Unable to copy the string, buffer too small");
			Debug::endl();
            return;
        }
        strcpy((buffer + index), str);
        updateEndPointer();
    }

    int find(char c)
    {
        for(int i = 0; i <= index; i++) {
            if(buffer[i] == c) {
                return i;
            }
        }
        return -1;
    }

    char at(int position)
    {
        if(position >= bufferSize) {
            return '~';
        }
        return buffer[position];
    }

    int size()
    {
        return index;
    }

    void shiftLeft(int shiftIndex)
    {
        if(shiftIndex < 1 || shiftIndex > index) {
            Debug::print("Buffer error: incorrect shift index!");
            Debug::endl();
            return;
        }
        strcpy(buffer, buffer + shiftIndex);
        index -= shiftIndex;
    }

    char* toCstr()
    {
        return buffer;
    }
private:
    int bufferSize;
    char *buffer;
    int index;

    void incrementBufferEndPointer()
    {
        index++;
        if(index > bufferSize - 1) {
            Debug::print("MyString: buffer overflow!!");;
            Debug::endl();
        }
    }

    void updateEndPointer()
    {
        while(*(buffer + index) != Cstr_terminator ) {
            incrementBufferEndPointer();
        }
    }

};

#endif // MYSTRING_H_INCLUDED
