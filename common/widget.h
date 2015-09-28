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
	void init(int width_, int height_)
	{
		if(outputBuffer != NULL) {
			return;
		}
		outputBuffer = (char*)malloc(width*height/8);
		width = width_;
		height = height_;
		if(outputBuffer == NULL) {
			Debug::println("Widget error: could not allocate memory for buffer.");
		}
	}
	
	void setPadding(int newPadding)
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
	
	int getWidth()
	{
		return width;
	}
	
	int getHeight()
	{
		return height;
	}
//protected:
	void setOctet(char octet, int x, int y)
	{
		int verticalOcetets = height/8;
		char index = x*verticalOcetets + y/8;
		outputBuffer[index] = octet;
	}

	char* outputBuffer;
	int width, height;
	int padding; //number of blank pixels from widget's edge
};

#endif