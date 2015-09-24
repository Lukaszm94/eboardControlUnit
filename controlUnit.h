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

#define CURRENT_WARNING_THRESHOLD 30

#define TEMPERATURE_WARNING_THRESHOLD 45
#define TEMPERATURE_CRITICAL_THRESHOLD 65

#define THERMOMETERS_COUNT 7 //also defined in sensorsManager.h, including it just for one define is pointless
#define THERMOMETER_MOSFET_ADC_CHANNEL 0
#define THERMOMETER_SAMPLES_COUNT 5
#define THERMOMETER_UNCONNECTED_MIN_VALUE 96 //if thermometer in DU is unconnected its reading is around 98-99C,
//so we can detect faulty reading (assuming that measured temperature is less than that
#define THERMOMETER_UNCONNECTED_VALUE 0 //value assigned to readings of unconnected thermometers (instead of 98-99)
#define THERMOMETER_M1_INDEX 0
#define THERMOMETER_M2_INDEX 1
#define THERMOMETER_D1_INDEX 2
#define THERMOMETER_D2_INDEX 3
#define THERMOMETER_I1_INDEX 4
#define THERMOMETER_I2_INDEX 5
#define THERMOMETER_MOSFET_INDEX 6

#define MOTOR_BATTERY_VOLTAGE_ADC_CHANNEL 0
#define MOTOR_BATTERY_VOLTAGE_COEFFICIENT ((10 + 120) / 10) // R1=120k, R2=10k
#define MOTOR_BATTERY_VOLTAGE_SAMPLES_COUNT 5
#define MOTOR_BATTERY_VOLTAGE_WARNING_THRESHOLD (3.4 * 6)
#define MOTOR_BATTERY_DISCONNECTED_THRESHOLD 2.0 // if battery is disconnected voltage divide is pulled to ground by attached resistor

#define CU_BATTERY_VOLTAGE_ADC_CHANNEL 1
#define CU_BATTERY_VOLTAGE_COEFFICIENT ((10 + 40) / 10) // R1=40k, R2=10k
#define CU_BATTERY_VOLTAGE_SAMPLES_COUNT 5
#define CU_BATTERY_VOLTAGE_WARNING_THRESHOLD (3.4 * 3)

class ControlUnit
{
public:
	ControlUnit()
	{
		batteryLoad = 0;
		batteryLoadUpdateTimer = boardStateUpdateTimer = lcdUpdateTimer = 0;
		latestPacket = NULL;
		motorBatteryVoltageOk = cuBatteryVoltageOk = temperaturesOk = true;
		newPacketReceived = false;
		mosfetTemperature = 20;
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
		boardStateUpdateTimer += INTERRUPT_PERIOD_MS;
		
		//analyze packet(if new received), read other sensors, check if parameters are in allowed boundaries
		if(boardStateUpdateTimer >= BOARD_STATE_UPDATE_PERIOD_MS) {
			boardStateUpdateTimer = 0;
			updateBoardState();
			checkParameters();
			updateStateControls();
		}
		
		//simple current integration to get drawn load
		if(batteryLoadUpdateTimer >= BATTERY_LOAD_UPDATE_PERIOD_MS) {
			batteryLoadUpdateTimer = 0;
			batteryLoadUpdate();
		}
		
		//display data on LCD
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
			findHighestTemperature();
		}
		updateMosfetTemperature();
		motorBatteryVoltage = getMotorBatteryVoltage();
		cuBatteryVoltage = getCuBatteryVoltage();
	}
	
	void checkParameters()
	{
		temperaturesOk = isTemperatureOk();
		cuBatteryVoltageOk = isCuBatteryOk();
		motorBatteryVoltageOk = isMotorBatteryOk();
		
	}
	
	void updateStateControls()
	{
		
	}

	void lcdUpdate()
	{
		//TODO
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
		m1current = latestPacket->Ia.toFloat();
		m2current = latestPacket->Ib.toFloat();
		
		
	}
	
	void findHighestTemperature()
	{
		highestTemperatureIndex = findMaxIntIndex(temperatures, THERMOMETERS_COUNT);
	}
	
	int getHighestTemperature()
	{
		return temperatures[highestTemperatureIndex];
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
		return (int)mosfetThermometer.getMeasuredValue();
	}
	
	float getMotorBatteryVoltage()
	{
		float batVADC = ADConverter::getAverageVoltage(MOTOR_BATTERY_VOLTAGE_ADC_CHANNEL , MOTOR_BATTERY_VOLTAGE_SAMPLES_COUNT);
		return (batVADC*MOTOR_BATTERY_VOLTAGE_COEFFICIENT);
	}
	
	//checks if battery is connected and if voltage is higher than min level
	bool isMotorBatteryOk()
	{
		bool lowVoltage = (motorBatteryVoltage < MOTOR_BATTERY_VOLTAGE_WARNING_THRESHOLD);
		motorBatteryConnected = (motorBatteryVoltage > MOTOR_BATTERY_DISCONNECTED_THRESHOLD);
		
		return (!lowVoltage && motorBatteryConnected);
	}
	
	bool isCuBatteryOk()
	{
		return (cuBatteryVoltage > CU_BATTERY_VOLTAGE_WARNING_THRESHOLD);
	}
	
	bool isTemperatureOk()
	{
		findHighestTemperature();
		return (getHighestTemperature() < TEMPERATURE_WARNING_THRESHOLD);
	}
	
	float getCuBatteryVoltage()
	{
		float batVADC = ADConverter::getAverageVoltage(CU_BATTERY_VOLTAGE_ADC_CHANNEL , CU_BATTERY_VOLTAGE_SAMPLES_COUNT);
		return (batVADC*CU_BATTERY_VOLTAGE_COEFFICIENT);
	}
	
	float getTotalMotorsCurrent()
	{
		return (m1current + m2current);
	}
	
	
	unsigned long lcdUpdateTimer;
	unsigned long batteryLoadUpdateTimer;
	unsigned long boardStateUpdateTimer;
	
	bool newPacketReceived;
	Packet *latestPacket;
	
	Thermometer mosfetThermometer;
	
	float m1current, m2current;
	float batteryLoad; //in mAh
	
	float motorBatteryVoltage;
	float cuBatteryVoltage;
	
	int temperatures[THERMOMETERS_COUNT];
	int highestTemperatureIndex;
	
	bool temperaturesOk;
	bool cuBatteryVoltageOk;
	bool motorBatteryVoltageOk;
	bool motorBatteryConnected;
};

#endif