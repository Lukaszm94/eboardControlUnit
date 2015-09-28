#include "label.h"

#define FONT_BASE_WIDTH 5
#define FONT_BASE_HEIGHT 8
#define VERTICAL_LINE_MAX_HEIGHT 8

Label::Label()
{
	fontMultiplier = 1;
}

void Label::setText(char* text)
{
	//TODO translate text to pixels and save it to outputBuffer
	int currentX = padding, currentY = padding;
	int textLength = strlen(text);
	//int linesCount = countCharOccurence(text, '\n', textLength);
	//int newLabelWidth = 2 * padding + FONT_BASE_WIDTH * fontMultiplier * findLongestLineLength(text, textLength);
	//int newLabelHeight = 2 * padding + FONT_BASE_HEIGHT * fontMultiplier * linesCount;
	/*if(newLabelWidth > width) {
		newLabelWidth = width;
	}
	if(newLabelHeight > height) {
		newLabelHeight = height;
	}*/
	char fontBaseChar[FONT_BASE_WIDTH];
	char verticalBufferLine[VERTICAL_LINE_MAX_HEIGHT];
	int verticalBufferIndex = 0;
	for(int letterCount = 0; letterCount < textLength; letterCount++) {
		if(text[letterCount] == '\n') {
			//TODO set currentX and currentY
			continue;
		}
		copyFontBaseToBuffer(fontBaseChar, text[letterCount]);
		clearVerticalLineBuffer(verticalBufferLine, VERTICAL_LINE_MAX_HEIGHT);
		for(int i = 0; i < FONT_BASE_WIDTH; i++) {
			drawVerticalLineToBuffer(verticalBufferLine, fontBaseChar[i], fontMultiplier);
			sendVerticalLine(verticalBufferLine, fontMultiplier, currentX, currentY);
			currentX += fontMultiplier;
			/*Debug::print("Vertical line: ");
			for(int j = 0; j < fontMultiplier; j++) {
				Debug::hex(verticalBufferLine[j]);
				Debug::print(' ');
			}
			Debug::endl();*/
			clearVerticalLineBuffer(verticalBufferLine, fontMultiplier);
		}
		sendVerticalLine(verticalBufferLine, 1, currentX, currentY);
		currentX++;
	}
}

void Label::setFontMultiplier(int multiplier)
{
	fontMultiplier = multiplier;
}

int Label::countCharOccurence(char* str, char c, int strLength)
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

int Label::findLongestLineLength(char* str, int strLength)
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

void Label::copyFontBaseToBuffer(char* buffer, char c)
{
	c -= 32; 
	for(int i = 0; i < 5; i++) {
		//buffer[i] = (GLCD_ReadByteFromROMMemory((char *)((int)font5x8 + (5 * c) + i))); 
		buffer[i] = *(font5x8 + (5 * c) + i);
	}
	//GLCD_WriteData(0x00); ???
}

void Label::drawVerticalLineToBuffer(char* buffer, char c, int multiplier)
{
	int verticalBufferIndex = 0;
	for(int i = 0; i < FONT_BASE_HEIGHT; i++) {
		bool pixelSet = c & (1<<i);
		for(int j = 0; j < fontMultiplier; j++) {
			if(pixelSet) {
				buffer[verticalBufferIndex/8] |= (1 << (verticalBufferIndex%8));
			}
			verticalBufferIndex++;
		}
	}
}

void Label::clearVerticalLineBuffer(char* buffer, int bytes)
{
	for(int i = 0; i < bytes; i++) {
		buffer[i] = 0;
	}
}

void Label::sendVerticalLine(char* buffer, int multiplier, int x, int y)
{
	Debug::println("Printing vertical line:");
	//send buffer 'multiplier' times
	for(int i = 0; i < multiplier; i++) {
		//send bytes of vertical line
		for(int j = 0; j < multiplier; j++) {
			/*Debug::print("Byte: ");
			Debug::hex(buffer[j]);
			Debug::print(", x: ");
			Debug::print(x);
			Debug::print(", y: ");
			Debug::println(y + j*8);*/
			setOctet(buffer[j], x, y + j*8);
		}
		x++;
	}
}