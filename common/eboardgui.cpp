#include "eboardgui.h"
#include <stdlib.h>

#define SPEED_LABEL_INDEX 0
#define DISTANCE_LABEL_INDEX 1
#define DISTANCE_UNIT_LABEL_INDEX 2
#define LOAD_LABEL_INDEX 3
#define CURRENT_LABEL_INDEX 4

#define SPEED_LABEL_X 5
#define SPEED_LABEL_Y 0
#define SPEED_LABEL_FONT_SIZE 5
#define SPEED_LABEL_WIDTH ((FONT_BASE_WIDTH*SPEED_LABEL_FONT_SIZE + 1)*2)
#define SPEED_LABEL_HEIGHT (FONT_BASE_HEIGHT*SPEED_LABEL_FONT_SIZE)

#define DISTANCE_LABEL_X 5
#define DISTANCE_LABEL_Y 40
#define DISTANCE_LABEL_FONT_SIZE 3
#define DISTANCE_LABEL_WIDTH ((FONT_BASE_WIDTH*DISTANCE_LABEL_FONT_SIZE + 1)*4)
#define DISTANCE_LABEL_HEIGHT (FONT_BASE_HEIGHT*DISTANCE_LABEL_FONT_SIZE)

#define DISTANCE_UNIT_LABEL_X 70
#define DISTANCE_UNIT_LABEL_Y 48
#define DISTANCE_UNIT_LABEL_FONT_SIZE 2
#define DISTANCE_UNIT_LABEL_WIDTH ((FONT_BASE_WIDTH*DISTANCE_UNIT_LABEL_FONT_SIZE + 1)*2)
#define DISTANCE_UNIT_LABEL_HEIGHT (FONT_BASE_HEIGHT*DISTANCE_UNIT_LABEL_FONT_SIZE)

#define LOAD_LABEL_X 64
#define LOAD_LABEL_Y 0
#define LOAD_LABEL_FONT_SIZE 3
#define LOAD_LABEL_WIDTH ((FONT_BASE_WIDTH*LOAD_LABEL_FONT_SIZE + 1)*4)
#define LOAD_LABEL_HEIGHT (FONT_BASE_HEIGHT*LOAD_LABEL_FONT_SIZE)

#define CURRENT_LABEL_X 92
#define CURRENT_LABEL_Y 40
#define CURRENT_LABEL_FONT_SIZE 3
#define CURRENT_LABEL_WIDTH ((FONT_BASE_WIDTH*CURRENT_LABEL_FONT_SIZE + 1)*2)
#define CURRENT_LABEL_HEIGHT (FONT_BASE_HEIGHT*CURRENT_LABEL_FONT_SIZE)

#define TMP_BUFFER_SIZE 6

EboardGUI::EboardGUI()
{
	
}

void EboardGUI::init()
{
	labels[SPEED_LABEL_INDEX].init(SPEED_LABEL_WIDTH, SPEED_LABEL_HEIGHT);
	labels[SPEED_LABEL_INDEX].setFontMultiplier(SPEED_LABEL_FONT_SIZE);
	labels[SPEED_LABEL_INDEX].setText("12");
	
	labels[DISTANCE_LABEL_INDEX].init(DISTANCE_LABEL_WIDTH, DISTANCE_LABEL_HEIGHT);
	labels[DISTANCE_LABEL_INDEX].setFontMultiplier(DISTANCE_LABEL_FONT_SIZE);
	labels[DISTANCE_LABEL_INDEX].setText("12.3");
	
	labels[DISTANCE_UNIT_LABEL_INDEX].init(DISTANCE_UNIT_LABEL_WIDTH, DISTANCE_UNIT_LABEL_HEIGHT);
	labels[DISTANCE_UNIT_LABEL_INDEX].setFontMultiplier(DISTANCE_UNIT_LABEL_FONT_SIZE);
	labels[DISTANCE_UNIT_LABEL_INDEX].setText("km");
	
	labels[LOAD_LABEL_INDEX].init(LOAD_LABEL_WIDTH, LOAD_LABEL_HEIGHT);
	labels[LOAD_LABEL_INDEX].setFontMultiplier(LOAD_LABEL_FONT_SIZE);
	labels[LOAD_LABEL_INDEX].setText("1234");
	
	labels[CURRENT_LABEL_INDEX].init(CURRENT_LABEL_WIDTH, CURRENT_LABEL_HEIGHT);
	labels[CURRENT_LABEL_INDEX].setFontMultiplier(CURRENT_LABEL_FONT_SIZE);
	labels[CURRENT_LABEL_INDEX].setText("12");
	
	setSpeed(99);
	setDistance(99.9);
	setLoad(1994);
	setCurrent(24);
}

void EboardGUI::addGUIToDisplay(GLCD* glcd)
{
	glcd->addWidget(&labels[SPEED_LABEL_INDEX], SPEED_LABEL_X, SPEED_LABEL_Y);
	glcd->addWidget(&labels[DISTANCE_LABEL_INDEX], DISTANCE_LABEL_X, DISTANCE_LABEL_Y);
	glcd->addWidget(&labels[DISTANCE_UNIT_LABEL_INDEX], DISTANCE_UNIT_LABEL_X, DISTANCE_UNIT_LABEL_Y);
	glcd->addWidget(&labels[LOAD_LABEL_INDEX], LOAD_LABEL_X, LOAD_LABEL_Y);
	glcd->addWidget(&labels[CURRENT_LABEL_INDEX], CURRENT_LABEL_X, CURRENT_LABEL_Y);
}

Widget* EboardGUI::getWidget(uint8_t widgetIndex)
{
	if(widgetIndex < LABELS_COUNT) {
		return &(labels[widgetIndex]);
	}
	return NULL;
}

void EboardGUI::setSpeed(uint8_t newSpeed)
{
	//prepare string with speed
	char tmpBuffer[TMP_BUFFER_SIZE];
	if(newSpeed < 10) {
		tmpBuffer[0] = ' ';
	} else {
		tmpBuffer[0] = '0' + newSpeed/10;
	}
	tmpBuffer[1] = '0' + newSpeed % 10;
	tmpBuffer[2] = '\0';
	//send that string to label
	labels[SPEED_LABEL_INDEX].setText(tmpBuffer);
}

void EboardGUI::setDistance(float newDistance)
{
	if(newDistance > 100) {
		Debug::println("Distance too big, setting to 99");
		newDistance = 99;
	}
	char tmpBuffer[TMP_BUFFER_SIZE];
	dtostrf(newDistance, 4, 1, tmpBuffer);
	labels[DISTANCE_LABEL_INDEX].setText(tmpBuffer);
}

void EboardGUI::setLoad(uint16_t newLoad)
{
	if(newLoad > 10000) {
		newLoad = 9999;
	}
	char tmpBuffer[TMP_BUFFER_SIZE];
	for(int i = 0; i < TMP_BUFFER_SIZE; i++) {
		tmpBuffer[i] = ' ';
	}
	itoaRight(newLoad, 4, tmpBuffer);
	labels[LOAD_LABEL_INDEX].setText(tmpBuffer);
}

void EboardGUI::setCurrent(uint8_t newCurrent)
{
	if(newCurrent > 100) {
		newCurrent = 99;
	}
	char tmpBuffer[TMP_BUFFER_SIZE];

	itoaRight(newCurrent, 2, tmpBuffer);
	labels[CURRENT_LABEL_INDEX].setText(tmpBuffer);
}

void EboardGUI::itoaRight(unsigned int value, int width, char* buffer)
{
	bool fillWithSpaces = false;
	for(int i = 0; i < width; i++) {
		if(fillWithSpaces) {
			buffer[width - i - 1] = ' ';
			continue;
		}
		uint8_t digit = value%10;
		buffer[width - i - 1] = '0' + digit;
		value = value/10;
		if(value == 0) {
			fillWithSpaces = true;
		}
	}
	buffer[width] = '\0';
}
