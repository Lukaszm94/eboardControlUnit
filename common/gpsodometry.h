#ifndef GPSODOMETRY_H
#define GPSODOMETRY_H
/* GPSOdometry class
 * used to estimate speed and travelled distance based on GPS data read via serial port
 * Speed is stored in m/s, distance is in meters
 *
 *
 *
 *
 */

#include "common/nmea.h"
#include "common/uart.h"
#define GPS_ODOMETRY_UPDATE_INTERVAL_MS 100

float fabs(float x)
{
	if(x < 0.0) {
		return (-x);
	}
	return x;
}

class GPSOdometry
{
public:
	GPSOdometry()
	{
		currentSpeed = 1;
		distanceTravelled = 0;
	}
	
	void init()
	{
		uart1_init(UART_BAUD_SELECT(9600, F_CPU));
	}
	
	void update()
	{
		updateSpeed();
		updateDistance();
	}
	
	void onNewGPSChar(char c)
	{
		nmea.fusedata(c);
	}
	
	float getSpeed()
	{
		return currentSpeed;
	}
	
	float getSpeedKmph()
	{
		return currentSpeed * 3.6;
	}
	
	unsigned long getDistance()
	{
		return (unsigned long) distanceTravelled;
	}
	
	float getDistanceKm()
	{
		return (distanceTravelled/1000);
	}
	
	void resetDistance()
	{
		distanceTravelled = 0;
	}
//private:
	void updateDistance()
	{
		distanceTravelled += (fabs(currentSpeed)*GPS_ODOMETRY_UPDATE_INTERVAL_MS)/1000;
	}
	
	void updateSpeed()
	{
		float nmeaSpeed = nmea.getSpeed(); //in km/h
		//currentSpeed = nmeaSpeed / 3.6;
		currentSpeed = nmeaSpeed / 3.6; //temporary 
	}
	
	NMEA nmea;
	float currentSpeed;
	float distanceTravelled;
};



#endif