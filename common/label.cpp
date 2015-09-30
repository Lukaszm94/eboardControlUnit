#include "label.h"


Label::Label()
{
	fontMultiplier = 1;
}

void Label::setText(char* text)
{
	//translate text to pixels and save it to outputBuffer
	uint8_t currentX = 0;
	uint8_t textLength = strlen(text);

	
	for(uint8_t letterCount = 0; letterCount < textLength; letterCount++) {
		if(text[letterCount] == '\n') {
			//TODO set currentX and currentY
			continue;
		}
		currentX = drawLetterToBuffer(text[letterCount], currentX);
		currentX++;
	}
}

void Label::setFontMultiplier(uint8_t multiplier)
{
	fontMultiplier = multiplier;
}
/*
uint8_t Label::countCharOccurence(char* str, char c, uint8_t strLength)
{
	uint8_t charCounter = 0;
	if(strLength < 0) {
		strLength = strlen(str);
	}
	for(uint8_t i = 0; i < strLength; i++) {
		if(str[i] == c) {
			charCounter++;
		}
	}
	return charCounter;
}

uint8_t Label::findLongestLineLength(char* str, uint8_t strLength)
{
	uint8_t maxLineLength = 0;
	uint8_t currentLineLength = 0;
	if(strLength < 0) {
		strLength = strlen(str);
	}
	for(uint8_t i = 0; i < strLength; i++) {
		if(str[i] == '\n') {
			if(currentLineLength > maxLineLength)
				maxLineLength = currentLineLength;
			currentLineLength = 0;
		} else {
			currentLineLength++;
		}
	}
	return maxLineLength;
}*/

void Label::copyFontBaseToBuffer(char* buffer, char c)
{
	c -= 32; 
	for(uint8_t i = 0; i < 5; i++) {
		buffer[i] = (GLCD_ReadByteFromROMMemory((char *)((int)font5x8 + (5 * c) + i))); 
		//buffer[i] = *(font5x8 + (5 * c) + i);
	}
}


uint8_t Label::drawLetterToBuffer(char c, uint8_t currentX)
{
	char fontBaseBuffer[FONT_BASE_WIDTH];
	copyFontBaseToBuffer(fontBaseBuffer, c);
	
	for(uint8_t i = 0; i < FONT_BASE_WIDTH; i++) {
		prepareSingleOutputColumn(fontBaseBuffer[i]);
		sendFontColumnToBuffer(currentX);
		currentX += fontMultiplier;
	}
	sendBlankVerticalLine(currentX);
	return currentX;
}

void Label::prepareSingleOutputColumn(char baseColumn)
{
	uint8_t outputColumnCurrentPixel = 0;
	
	for(uint8_t basePixelCounter = 0; basePixelCounter < FONT_BASE_HEIGHT; basePixelCounter++) {
		bool isPixelSet = baseColumn & (1<<basePixelCounter);
		
		for(uint8_t repeatBasePixelCounter = 0; repeatBasePixelCounter < fontMultiplier; repeatBasePixelCounter++) {
			uint8_t outputColumnIndex = outputColumnCurrentPixel/8;
			uint8_t mask = (1 << (outputColumnCurrentPixel%8));
			if(isPixelSet) {
				singleOutputColumn[outputColumnIndex] |= mask;
			} else {
				singleOutputColumn[outputColumnIndex] &= ~mask;
			}
			outputColumnCurrentPixel++;
		}
	}

}

void Label::sendFontColumnToBuffer(uint8_t x)
{
	//Debug::println("Send single column");
	for(int columnsCounter = 0; columnsCounter < fontMultiplier; columnsCounter++) {
		for(int columnIndex = 0; columnIndex < fontMultiplier; columnIndex++) {
			//Debug::hex(singleOutputColumn[columnIndex]);
			//Debug::print(' ');
			setOctet(singleOutputColumn[columnIndex], x, columnIndex*8);
		}
		Debug::endl();
		x++;
	}
}

void Label::sendBlankVerticalLine(uint8_t x)
{
	for(uint8_t columnIndex = 0; columnIndex < fontMultiplier; columnIndex++) {
		setOctet(0x00, x, columnIndex*8);
	}
}