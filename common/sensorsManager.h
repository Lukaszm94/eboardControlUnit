#ifndef SENSORSMANAGER_H
#define SENSORSMANAGER_H

#include "sensorInterface.h"
#include "adconverter.h"



#define SAMPLES_COUNT 10

#define CURRENT_SENSOR_COEFFICIENT_x10 357// V -> A
#define CURRENT_SENSORS_COUNT 2


#define LM35_MV_TO_C_COEFF 0.1 //degC per mV
#define THERMOMETERS_COUNT 6

#define CURRENT_SENSOR_1_CHANNEL 6
#define CURRENT_SENSOR_2_CHANNEL 7
#define TEMP_M1_CHANNEL 0
#define TEMP_M2_CHANNEL 1
#define TEMP_D1_CHANNEL 2
#define TEMP_D2_CHANNEL 3
#define TEMP_I1_CHANNEL 4
#define TEMP_I2_CHANNEL 5


const int samples = 50;

#define MAX_SENSORS_COUNT0 10

class SensorManager
{
public:
	SensorManager()
	{
		sensorsCount = 0;
	}
	bool addSensor(Sensor *sensor)
	{
		if(sensorsCount + 1 >= MAX_SENSORS_COUNT0) {
			return false;
		}
		sensorsTab[sensorsCount] = sensor;
		sensorsCount++;
		return true;
	}
	
	void readAll()
	{
		for(int i = 0; i < sensorsCount; i++) {
			sensorsTab[i]->insertNewReading(ADConverter::getAverageVoltage(i, SAMPLES_COUNT));
		}
	}
	
	float getSingleReading(int sensorIndex)
	{
		return (sensorsTab[sensorIndex]->getMeasuredValue());
	}
	
private:
	int sensorsCount;
	Sensor* sensorsTab[MAX_SENSORS_COUNT0];

};

#endif