#ifndef CONTROLUNIT_H
#define CONTROLUNIT_H

#include "KS0108.h"
#include "../common/timer0.h"
#include "../common/packet.h"
#include "../common/debug.h"
#include "../common/thermometer.h"
#include "../common/adconverter.h"

#define BATTERY_LOAD_UPDATE_PERIOD_MS 200
#define LCD_UPDATE_PERIOD_MS 500
#define MILIAMPEROSECONDS_TO_MILIAMPEROHOURS 1.0/(60*60)

#define TEMPERATURE_WARNING_THRESHOLD 45

#define THERMOMETERS_COUNT 7 //also defined in sensorsManager.h, including it just for one define is pointless
#define THERMOMETER_MOSFET_ADC_CHANNEL 0
#define THERMOMETER_SAMPLES_COUNT 5

#define THERMOMETER_M1_INDEX 0
#define THERMOMETER_M2_INDEX 1
#define THERMOMETER_D1_INDEX 2
#define THERMOMETER_D2_INDEX 3
#define THERMOMETER_I1_INDEX 4
#define THERMOMETER_I2_INDEX 5
#define THERMOMETER_MOSFET_INDEX 6

#define MOTORS_BATTERY_VOLTAGE_ADC_CHANNEL 1
#define MOTORS_BATTERY_VOLTAGE_COEFFICIENT ((10 + 120) / 10) // R1=120k, R2=10k
#define MOTORS_BATTERY_VOLTAGE_SAMPLES_COUNT 5

class ControlUnit
{
public:
	ControlUnit()
	{
		batteryLoad = 0;
		batteryLoadUpdateTimer = 0;
		lcdUpdateTimer = 0;
		latestPacket = NULL;
		temperaturesOk = true;
		newPacketReceived = false;
		highestTemperature = mosfetTemperature = 20;
		highestTemperatureIndex = 0;
	}
	
	void init()
	{
		ADConverter::init();
		GLCD_Initalize();
	}

	void update()
	{
		batteryLoadUpdateTimer += INTERRUPT_PERIOD_MS;
		lcdUpdateTimer += INTERRUPT_PERIOD_MS;
		if(batteryLoadUpdateTimer >= BATTERY_LOAD_UPDATE_PERIOD_MS) {
			batteryLoadUpdateTimer = 0;
			batteryLoadUpdate();
		}
		
		
		if(lcdUpdateTimer >= LCD_UPDATE_PERIOD_MS) {
			lcdUpdateTimer = 0;
			lcdUpdate();
		}
	}
	
	void onNewPacketReceived(Packet *pack)
	{
		latestPacket = pack;
		newPacketReceived = true;
	}


private:
	void updateBoardState()
	{
		if(newPacketReceived) {
			extractDataFromPacket();
			updateMosfetTemperature();
			//TODO
		}
	
	}

	void lcdUpdate()
	{
		if(latestPacket == NULL) {
			return;
		}
		if(newPacketReceived) {
			newPacketReceived = false;
			analyzePacket();
		}
		mosfetTemperature = readMosfetTemperature();
		
		displayData();
	}
	
	void batteryLoadUpdate()
	{
		if(latestPacket == NULL) {
			return;
		}
		float totalCurrent = latestPacket->Ia.toFloat() + latestPacket->Ib.toFloat();
		batteryLoad += (totalCurrent * BATTERY_LOAD_UPDATE_PERIOD_MS)*MILIAMPEROSECONDS_TO_MILIAMPEROHOURS; //since dt is in ms, we get load in mAs, multimply that by 1/60*60 to get mAh
		
	}
	
	void displayData()
	{
		LCD_clear();
		LCD_goto(0,0);
		int current = latestPacket->Ia.getInteger(); //TOFIX
		if(latestPacket->Ia.getFractional() >= 5) {
			current++;
		}
		LCD_int(current);
		LCD_str("A ");
		current = latestPacket->Ib.getInteger(); //TOFIX
		if(latestPacket->Ib.getFractional() >= 5) {
			current++;
		}
		LCD_int(current);
		LCD_str("A ");
		
		LCD_goto(9,0); //first line, display load
		LCD_int((int)(batteryLoad + 0.5));
		LCD_goto(13,0);
		LCD_str("mAh");
		
		//second line
		LCD_goto(0,1);
		LCD_str("T ");
		if(temperaturesOk) {
			LCD_str("ok "); 
			//enough space on LCD to print battery voltage
			lcdPrintFloat(getBatteryVoltage());
		} else {
			LCD_str("not OK");
		}
		
		LCD_goto(11,1);
		LCD_int(highestTemperatureIndex + 1);
		LCD_char('H');
		LCD_int(highestTemperature);
		LCD_char('C');
	}
	
	void extractDataFromPacket()
	{
		latestPacket->loadTemperaturesToArray(temperatures);
		
		highestTemperatureIndex = findMaxIntIndex(temperatures, THERMOMETERS_COUNT);
		highestTemperature = temperatures[highestTemperatureIndex];
		if(highestTemperature >= TEMPERATURE_WARNING_THRESHOLD) {
			temperaturesOk = false;
		} else {
			temperaturesOk = true;
		}
	}
	
	void updateMosfetTemperature()
	{
		temperatures[THERMOMETER_MOSFET_INDEX] = readMosfetTemperature();
	}
	
	int findMaxIntIndex(int *tab, int size) //returns INDEX of biggest int in given array
	{
		int maxTempIndex = 0;
		for(int i = 1; i < size; i++) {
			if(tab[i] > tab[maxTempIndex]) {
				maxTempIndex = i;
			}
		}
		return maxTempIndex;
	}
	
	int readMosfetTemperature()
	{
		mosfetThermometer.insertNewReading(ADConverter::getAverageVoltage(THERMOMETER_MOSFET_ADC_CHANNEL, THERMOMETER_SAMPLES_COUNT));
		
		Debug::print("Mosfet temp: ");
		Debug::println((int)mosfetThermometer.getMeasuredValue());
		
		return (int)mosfetThermometer.getMeasuredValue();
	}
	
	float getBatteryVoltage()
	{
		float batVADC = ADConverter::getAverageVoltage(MOTORS_BATTERY_VOLTAGE_ADC_CHANNEL , MOTORS_BATTERY_VOLTAGE_SAMPLES_COUNT);
		return (batVADC*MOTORS_BATTERY_VOLTAGE_COEFFICIENT);
		
	}
	
	float getTotalMotorsCurrent()
	{
		return (m1current + m2current);
	}
	
	void lcdPrintFloat(float number)
	{
		LCD_int((int)number);
		LCD_char('.');
		int frac = (int)(number*10) % 10;
		LCD_int(frac);
	}
	
	float batteryLoad; //in mAh
	unsigned long lcdUpdateTimer;
	unsigned long batteryLoadUpdateTimer;
	bool newPacketReceived;
	Packet *latestPacket;
	
	Thermometer mosfetThermometer;
	
	float m1current, m2current;
	
	int temperatures[THERMOMETERS_COUNT];
	int highestTemperatureIndex;
	bool temperaturesOk;
};

#endif