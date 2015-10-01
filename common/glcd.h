#ifndef GLCD_H
#define GLCD_H

#include "widget.h"

#define MAX_WIDGET_COUNT 10
#define ROTATED_180 1
#define BACKLIGHT_PIN 3

struct WidgetData
{
	Widget* widget;
	int xPos;
	int yPos;
};



class GLCD
{
public:
	GLCD()
	{
		widgetCount = 0;
	}
	
	void init()
	{
		GLCD_Initalize();
		erase();
		GLCD_GoTo(0,0);
		DDRE |= (1<<BACKLIGHT_PIN);
	}
	
	void addWidget(Widget* newWidget, int x, int y)
	{
		if(widgetCount < MAX_WIDGET_COUNT) {
			//add widget and its position to widgets vector
			widgetsData[widgetCount].widget = newWidget;
			widgetsData[widgetCount].xPos = x;
			widgetsData[widgetCount].yPos = y;
			widgetCount++;
		}
	}
	
	void redraw()
	{
		for(int i = 0; i < widgetCount; i++) {
			redrawWidget(i);
		}
	}
	
	void removeWidget(uint8_t index)
	{
		if(index >= widgetCount) {
			return;
		}
		widgetsData[index].widget = NULL;
		widgetsData[index].xPos = 0;
		widgetsData[index].yPos = 0;
		widgetCount--;
	}
	
	void erase()
	{
		GLCD_ClearScreen();
	}
	
	void backlightOn()
	{
		PORTE |= (1<<BACKLIGHT_PIN);
	}
//private:
	void redrawWidget(int i)
	{
		
		Widget* widget = widgetsData[i].widget;
		char* buffer = widget->getBitmap();
		int width = widget->getWidth();
		int height = widget->getHeight()/8;
		
		#if !ROTATED_180
		int xStart = widgetsData[i].xPos;
		int yStart = widgetsData[i].yPos/8;
		
		for(int y = 0; y < height; y++) {
			GLCD_GoTo(xStart, yStart + y);
			drawHorizontalOctets(buffer + y, width, height);
		}
		
		#else
		//drawing rotated bitmaps
		int xStart = KS0108_SCREEN_WIDTH - widgetsData[i].xPos - width;
		int yStart = KS0108_SCREEN_HEIGHT/8 - widgetsData[i].yPos/8 - height;
		buffer = buffer + width * height - 1;

		for(uint8_t row = 0; row < height; row++) {
			GLCD_GoTo(xStart, yStart + row);
			for(uint8_t column = 0; column < width; column++) {
				char* bytePtr = (char*)(buffer - height * column - row);
				uint8_t memByte = *bytePtr;
				uint8_t mirroredBits = mirrorBits(memByte);
				GLCD_WriteData(mirroredBits);
			}
		}
		
		/*GLCD_GoTo(xStart, yStart);
		for(int column = 0; column < width; column++) {
			uint8_t memByte = *(buffer - height * column);
			uint8_t mirroredBits = mirrorBits(memByte);
			GLCD_WriteData(mirroredBits);
		}
		
		GLCD_GoTo(xStart, yStart + 1);
		for(int column = 0; column < width; column++) {
			uint8_t memByte = *(buffer - height * column - 1);
			uint8_t mirroredBits = mirrorBits(memByte);
			GLCD_WriteData(mirroredBits);
		}
		*/
		
		#endif
	}
	
	void drawHorizontalOctets(char* buffer, int width, int height)
	{
		for(int i = 0; i < width; i++) {
			GLCD_WriteData(*buffer);
			buffer += height;
		}
	}
	
	uint8_t mirrorBits(uint8_t v)
	{
		//v= input bits to be reversed
		uint8_t r = v; // r will be reversed bits of v; first get LSB of v
		uint8_t s = sizeof(v) * 8 - 1; // extra shift needed at end

		for (v >>= 1; v; v >>= 1)
		{
			r <<= 1;
			r |= v & 1;
			s--;
		}
		r <<= s;
		return r;
	}
	
	int widgetCount;
	WidgetData widgetsData[MAX_WIDGET_COUNT];
};
#endif