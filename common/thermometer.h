/*
 * thermometer.h
 *
 *  Created on: 19 lip 2015
 *      Author: LukaszSSD
 */

#ifndef THERMOMETER_H_
#define THERMOMETER_H_

/*
*	This class is written with assumption that all used sensors are the same and are of type with analog output proportional to temperature
*
*
*
*/

#include "sensorInterface.h"

#define LM35_CONVERSION_RATIO 100 //V -> C
#define LM35_OFFSET 0.0 //V



class Thermometer : public Sensor
{
public:
	Thermometer() 
	{
		measuredValue = 20.0;
	}
	
	static float offset;
	static float VToCelsiusCoefficient;
	
	static void setCoefficient(float VToCelsiusCoefficient_)
	{
		Thermometer::VToCelsiusCoefficient = VToCelsiusCoefficient_;
	}
	
	void insertNewReading(float readV)
	{
		measuredValue = convertRawADCToTemp(readV);
	}
private:
	float convertRawADCToTemp(float rawV)
	{
		float temp = (rawV - offset)*VToCelsiusCoefficient;
		return temp;
	}

};

float Thermometer::VToCelsiusCoefficient = LM35_CONVERSION_RATIO;
float Thermometer::offset = LM35_OFFSET;

#endif /* THERMOMETER_H_ */
