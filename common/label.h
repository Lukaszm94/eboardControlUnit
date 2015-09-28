#ifndef LABEL_H
#define LABEL_H

#include <string.h>
#include "widget.h"
#include "KS0108.h"
#include "font5x8.h"

#include "debug.h"

class Label : public Widget
{
public:
	Label();
	void setText(char* text);
	
	//method to display bigger characters (x2, x3 etc of normal size)
	//caller should take care to initially set Widget's size big enough
	void setFontMultiplier(int multiplier);

private:
	int countCharOccurence(char* str, char c, int strLength = -1);
	int findLongestLineLength(char* str, int strLength = -1);
	void copyFontBaseToBuffer(char* buffer, char c);
	void drawVerticalLineToBuffer(char* buffer, char c, int multiplier);
	void clearVerticalLineBuffer(char* buffer, int bytes);
	void sendVerticalLine(char* buffer, int multiplier, int x, int y);

	int fontMultiplier;
};

#endif