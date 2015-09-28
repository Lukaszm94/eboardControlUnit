#ifndef GLCD_H
#define GLCD_H

#include "widget.h"

#define MAX_WIDGET_COUNT 10
#define ROTATED_180 0

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
		GLCD_ClearScreen();
		GLCD_GoTo(0,0);
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
private:
	void redrawWidget(int i)
	{
		int xStart = widgetsData[i].xPos;
		int yStart = widgetsData[i].yPos;
		Widget* widget = widgetsData[i].widget;
		char* buffer = widget->getBitmap();
		#if !ROTATED_180
		int width = widget->getWidth();
		int height = widget->getHeight()/8;
		for(int x = 0; x < width; x++) {
			for(int y = 0; y < height; y++) {
				char octet = buffer[x*height + y];
				GLCD_WriteData(octet);
			}
		}
		#else
		xStart = KS0108_SCREEN_WIDTH - widgetsData[i].xPos;
		yStart = KS0108_SCREEN_HEIGHT - widgetsData[i].yPos;
		//TODO implement drawing rotated bitmaps
		#endif
	}
	
	int widgetCount;
	WidgetData widgetsData[MAX_WIDGET_COUNT];
};
#endif