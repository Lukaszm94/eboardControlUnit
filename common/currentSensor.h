#ifndef CURRENTSENSOR_H
#define CURRENTSENSOR_H

#include "sensorInterface.h"

#define ACS709_CONVERSION_COEFFICIENT 35.714 // A/V
#define ACS709_OFFSET 2.5 // V

class CurrentSensor : public Sensor
{
public:
	static float offset;
	static float VToACoefficient;
	
	void insertNewReading(float readV)
	{
		measuredValue = convertRawADCToTemp(readV);
	}
	
private:
	float convertRawADCToTemp(float rawV)
	{
		
		float temp = (rawV - offset)*VToACoefficient;
		return fabs(temp); //fabs is used to avoid negative values, since transmission protocol doesn't support negative numbers
	}

};

float CurrentSensor::VToACoefficient = ACS709_CONVERSION_COEFFICIENT;
float CurrentSensor::offset = ACS709_OFFSET;
#endif