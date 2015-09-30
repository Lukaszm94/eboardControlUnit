#ifndef LABEL_H
#define LABEL_H

#include <string.h>
#include "widget.h"
#include "KS0108.h"
#include "font5x8.h"

#include "debug.h"

#define VERTICAL_LINE_MAX_HEIGHT 8
#define FONT_BASE_WIDTH 5
#define FONT_BASE_HEIGHT 8

class Label : public Widget
{
public:
	Label();
	void setText(char* text);
	
	//method to display bigger characters (x2, x3 etc of normal size)
	//caller should take care to initially set Widget's size big enough
	void setFontMultiplier(uint8_t multiplier);

private:
	//uint8_t countCharOccurence(char* str, char c, uint8_t strLength = -1);
	//uint8_t findLongestLineLength(char* str, uint8_t strLength = -1);
	void copyFontBaseToBuffer(char* buffer, char c);
	uint8_t drawLetterToBuffer(char c, uint8_t currentX);

	void sendVerticalLineToBuffer(uint8_t x);
	void sendBlankVerticalLine(uint8_t x);
	void prepareSingleOutputColumn(char baseColumn);
	void sendFontColumnToBuffer(uint8_t x);

	uint8_t fontMultiplier;
	char singleOutputColumn[VERTICAL_LINE_MAX_HEIGHT];
};

#endif