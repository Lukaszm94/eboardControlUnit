#ifndef LABEL_H
#define LABEL_H

#include <string.h>
#include "widget.h"
#include "KS0108.h"
#include "font5x8.h"

#include "debug.h"

#define MAX_TEXT_LENGTH 20
#define FONT_BASE_WIDTH 5
#define FONT_BASE_HEIGHT 8
#define VERTICAL_LINE_MAX_HEIGHT 8


class Label : public Widget
{
public:
	Label() : Widget()
	{
		fontMultiplier = 1;
	}
	
	void setText(char* text)
	{
		//TODO translate text to pixels and save it to outputBuffer
		int currentX = padding, currentY = padding;
		int textLength = strlen(text);
		int linesCount = countCharOccurence(text, '\n', textLength);
		int newLabelWidth = 2 * padding + FONT_BASE_WIDTH * fontMultiplier * findLongestLineLength(text, textLength);
		int newLabelHeight = 2 * padding + FONT_BASE_HEIGHT * fontMultiplier * linesCount;
		if(newLabelWidth > width) {
			newLabelWidth = width;
		}
		if(newLabelHeight > height) {
			newLabelHeight = height;
		}
		char fontBaseChar[FONT_BASE_WIDTH];
		char verticalBufferLine[VERTICAL_LINE_MAX_HEIGHT];
		int verticalBufferIndex = 0;
		for(int count = 0; count < textLength; count++) {
			if(text[count] == '\n') {
				//TODO set currentX and currentY
				continue;
			}
			copyFontBaseToBuffer(fontBaseChar, text[count]);
			for(int n = 0; n < FONT_BASE_WIDTH; n++)
				Debdebug
			for(int i = 0; i < FONT_BASE_WIDTH; i++) {
				char fontBaseVerticalLine = fontBaseChar[i];
				for(int j = 0; j < FONT_BASE_HEIGHT; i++) {
					bool pixelSet = fontBaseVerticalLine & (1<<j);
					for(int k = 0; k < fontMultiplier; k++) {
						if(pixelSet) {
							verticalBufferLine[verticalBufferIndex/8] |= (1 << (verticalBufferIndex%8));
						} else {
							verticalBufferLine[verticalBufferIndex/8] &= ~(1 << (verticalBufferIndex%8));
						}
						verticalBufferIndex++;
					}
				}
				//TODO send vertical line to widget
				for(int j = 0; j < fontMultiplier; j++) {
					for(int k = 0; k < fontMultiplier; k++) {
						setOctet(verticalBufferLine[j], currentX, currentY + k * 8);
						Debug::print(verticalBufferLine[i]);
					}
					currentX ++;
				}
				
			}
		}
	}
	
	//method to display bigger characters (x2, x3 etc of normal size)
	//caller should take care to initially set Widget's size big enough
	void setFontMultiplier(int multiplier)
	{
		fontMultiplier = multiplier;
	}
private:
	int countCharOccurence(char* str, char c, int strLength = -1)
	{
		int charCounter = 0;
		if(strLength < 0) {
			strLength = strlen(str);
		}
		for(int i = 0; i < strLength; i++) {
			if(str[i] == c) {
				charCounter++;
			}
		}
		return charCounter;
	}
	
	int findLongestLineLength(char* str, int strLength = -1)
	{
		int maxLineLength = 0;
		int currentLineLength = 0;
		if(strLength < 0) {
			strLength = strlen(str);
		}
		for(int i = 0; i < strLength; i++) {
			if(str[i] == '\n') {
				if(currentLineLength > maxLineLength)
					maxLineLength = currentLineLength;
				currentLineLength = 0;
			} else {
				currentLineLength++;
			}
		}
		return maxLineLength;
	}
	
	void copyFontBaseToBuffer(char* buffer, char c)
	{
		c -= 32; 
		for(int i = 0; i < 5; i++) {
			buffer[i] = (GLCD_ReadByteFromROMMemory((char *)((int)font5x8 + (5 * c) + i))); 
		}
		//GLCD_WriteData(0x00); ???
	}

	char textBuffer[MAX_TEXT_LENGTH];
	int fontMultiplier;
};

#endif