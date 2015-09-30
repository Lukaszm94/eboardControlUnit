#ifndef EBOARDGUI_H
#define EBOARDGUI_H

#include "label.h"
#include "glcd.h"

#define LABELS_COUNT 5


class EboardGUI
{
public:
	EboardGUI();
	void init();
	void addGUIToDisplay(GLCD* glcd);
	Widget* getWidget(uint8_t widgetIndex);
	void setSpeed(uint8_t newSpeed);
	void setDistance(float newDistance);
	void setLoad(uint16_t newLoad);
	void setCurrent(uint8_t newCurrent);
private:
	Label labels[LABELS_COUNT];

};


#endif