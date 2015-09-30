#ifndef WIDGET_H
#define WIDGET_H
#include <stdlib.h>
#include "debug.h"

class Widget
{
public:
	Widget()
	{
		outputBuffer = NULL;
		width = height = padding = 0;
	}
	
	//initialize output buffer. This method should be called only once!
	void init(uint8_t width_, uint8_t height_)
	{
		if(outputBuffer != NULL) {
			return;
		}
		//TODO add artificial dynamic memory allocation (allocate big tab and give chunks with fake malloc)
		outputBuffer = (char*)malloc(width*height/8);
		width = width_;
		height = height_;
		if(outputBuffer == NULL) {
			Debug::println("Widget error: could not allocate memory for buffer.");
		}
	}
	
	void setPadding(uint8_t newPadding)
	{
		if(newPadding < 0) {
			newPadding = 0;
		}
		padding = newPadding;
	}
	
	char* getBitmap()
	{
		return outputBuffer;
	}
	
	inline uint8_t getWidth()
	{
		return width;
	}
	
	inline uint8_t getHeight()
	{
		return height;
	}
//protected:
	inline void setOctet(char octet, uint8_t x, uint8_t y)
	{
		uint8_t verticalOcetets = height/8;
		char index = x*verticalOcetets + y/8;
		if(index > width*height/8) {
			Debug::println("Widget: Set octet: Index out of range");
			return;
		}
		outputBuffer[index] = octet;
	}

	char* outputBuffer;
	uint8_t width, height;
	uint8_t padding; //number of blank pixels from widget's edge
};

#endif