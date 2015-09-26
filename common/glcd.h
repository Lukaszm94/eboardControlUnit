#ifndef GLCD_H
#define GLCD_H

#define MAX_WIDGET_COUNT 10
#define ROTATED_180 1

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
	
	void addWidget(Widget* newWidget, int x, int y)
	{
		if(widgetCount < MAX_WIDGET_COUNT) {
			//add widget and its position to widgets vector
			widgetsData[i].widget = newWidget;
			widgetsData[i].xPos = x;
			widgetsData[i].yPos = y;
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
		int yStart = widgetData[i].yPos;
		Widget* widget = widgetData[i].widget;
		char* buffer = widget->getBitmap();
		#if !ROTATED_180
		int width = widget->width()/8;
		int height = widget->height()/8;
		for(int x = 0; x < width; x++_ {
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