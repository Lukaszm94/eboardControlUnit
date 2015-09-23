#ifndef SENSORINTERFACE_H
#define SENSORINTERFACE_H

class Sensor
{
public:
	virtual void init()
	{
	
	}
	
	float getMeasuredValue()
	{
		return measuredValue;
	}
	
	virtual void insertNewReading(float readV) = 0;

protected:
	float measuredValue;
};

#endif