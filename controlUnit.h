#ifndef CONTROLUNIT_H
#define CONTROLUNIT_H

#include "KS0108.h"
#include "common/avriomanip.h"
#include "common/timer0.h"
#include "common/packet.h"
#include "common/debug.h"
#include "common/thermometer.h"
#include "common/adconverter.h"
#include "common/rgbled.h"
#include "common/gpsodometry.h"
#include "common/glcd.h"
#include "common/eboardgui.h"
#include "common/led.h"
#include "common/button.h"


#define BOARD_STATE_UPDATE_PERIOD_MS 200
#define BATTERY_LOAD_UPDATE_PERIOD_MS 200
#define LCD_UPDATE_PERIOD_MS 500
#define MILIAMPEROSECONDS_TO_MILIAMPEROHOURS 1.0/(60*60)

#define CURRENT_WARNING_THRESHOLD 30

#define TEMPERATURE_WARNING_THRESHOLD 45
#define TEMPERATURE_CRITICAL_THRESHOLD 65

#define THERMOMETERS_COUNT 7 //also defined in sensorsManager.h, including it just for one define is pointless
#define THERMOMETER_MOSFET_ADC_CHANNEL 2
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
#define MOTOR_BATTERY_SWITCH_PORT G
#define MOTOR_BATTERY_SWITCH_PIN 0
#define MOTOR_BATTERY_LOAD_MAX 9999

#define CU_BATTERY_VOLTAGE_ADC_CHANNEL 1
#define CU_BATTERY_VOLTAGE_COEFFICIENT ((10 + 40) / 10) // R1=40k, R2=10k
#define CU_BATTERY_VOLTAGE_SAMPLES_COUNT 5
#define CU_BATTERY_VOLTAGE_WARNING_THRESHOLD (3.4 * 3)

#define STATE_CONTROL_BLINK_FAST_PERIOD 500
#define STATE_CONTROL_BLINK_SLOW_PERIOD 1500
#define STATE_CONTROL_BLINK_DUTY_CYCLE 50

#define FRONT_LIGHTS_SLOW_BLINKING_PERIOD_MS 600
#define FRONT_LIGHTS_STROBE_PERIOD_MS 300
#define FRONT_LIGHTS_DDR DDRK
#define FRONT_LIGHTS_PORT PORTK
#define FRONT_LIGHTS_PIN 3

#define ENCODER_BUTTON_DDR DDRD
#define ENCODER_BUTTON_PORT PORTD
#define ENCODER_BUTTON_PIN_REG PIND
#define ENCODER_BUTTON_PIN 0
#define MIN_ENCODER_BUTTON_PRESS_INTERVAL_US 200000UL

#define DU_START_SENDING_COMMAND ('b')
#define DU_STOP_SENDING_COMMAND ('s')

#define DEBUG_NOTIFICATION_INTERVAL 1000

enum {FRONT_LIGHTS_OFF, FRONT_LIGHTS_LOW_DUTY_BLINKING, FRONT_LIGHTS_HALF_DUTY_BLINKING, FRONT_LIGHTS_SOLID};

class ControlUnit
{
public:
	ControlUnit()
	{
		batteryLoad = 0;
		currentTimeUs = batteryLoadUpdateTimer = boardStateUpdateTimer = stateControlsTimer = lcdUpdateTimer = odometerTimer = debugTimer = 0;
		latestPacket = NULL;
		motorBatteryVoltageOk = cuBatteryVoltageOk = temperaturesOk = true;
		newPacketReceived = false;
		highestTemperatureIndex = 0;
		
		uint8_t motorRGBPins[] = {5,4,PIN_UNASSIGNED};
		motorsRGB.init(&PORTK, &DDRK, motorRGBPins);
		uint8_t temperatureRGBPins[] = {6,7,PIN_UNASSIGNED};
		temperatureRGB.init(&PORTK, &DDRK, temperatureRGBPins);
		currentFrontLightsState = FRONT_LIGHTS_OFF;
	}
	
	void init()
	{
		ADConverter::init();
		glcd.init();
		//set MOSFET pin as output, turn MOSFET off
		DDR(MOTOR_BATTERY_SWITCH_PORT) |= (1<<MOTOR_BATTERY_SWITCH_PIN);
		turnMotorBatteryMosfetOff();
		frontLights.init(&(FRONT_LIGHTS_PORT), &(FRONT_LIGHTS_DDR), FRONT_LIGHTS_PIN);
		encoderButton.init(&(ENCODER_BUTTON_PORT), &(ENCODER_BUTTON_DDR), &(ENCODER_BUTTON_PIN_REG), ENCODER_BUTTON_PIN);
	}

	void update()
	{
		currentTimeUs += INTERRUPT_PERIOD_MS;
		batteryLoadUpdateTimer += INTERRUPT_PERIOD_MS;
		lcdUpdateTimer += INTERRUPT_PERIOD_MS;
		boardStateUpdateTimer += INTERRUPT_PERIOD_MS;
		stateControlsTimer += INTERRUPT_PERIOD_MS;
		odometerTimer += INTERRUPT_PERIOD_MS;
		debugTimer += INTERRUPT_PERIOD_MS;
		
		//analyze packet(if new received), read other sensors, check if parameters are in allowed boundaries
		if(boardStateUpdateTimer >= BOARD_STATE_UPDATE_PERIOD_MS) {
			boardStateUpdateTimer = 0;
			updateBoardState();
			checkParameters();
			updateStateControls();
		}
		
		if(stateControlsTimer >= RGBLED_UPDATE_INTERVAL_MS) {
			stateControlsTimer = 0;
			motorsRGB.update();
			temperatureRGB.update();
		}
		
		//simple current integration to get drawn load
		if(batteryLoadUpdateTimer >= BATTERY_LOAD_UPDATE_PERIOD_MS) {
			batteryLoadUpdateTimer = 0;
			batteryLoadUpdate();
		}
		
		if(odometerTimer >= GPS_ODOMETRY_UPDATE_INTERVAL_MS) {
			odometerTimer = 0;
			//odometer.update();
		}
		
		//display data on LCD
		if(lcdUpdateTimer >= LCD_UPDATE_PERIOD_MS) {
			lcdUpdateTimer = 0;
			lcdUpdate();
		}
		
		if(debugTimer >= DEBUG_NOTIFICATION_INTERVAL) {
			debugTimer = 0;
			debugUpdate();
		}
		
		lightsUpdate();
	}
	
	void onNewPacketReceived(Packet *pack)
	{
		latestPacket = pack;
		newPacketReceived = true;
	}
	
	inline void newGPSChar(char c)
	{
		odometer.onNewGPSChar(c);
	}
	
	void startupSequence()
	{
		Label infoLabel;
		infoLabel.init(128, 8);
		glcd.addWidget(&infoLabel, 0, 0);
		motorBatteryVoltage = getMotorBatteryVoltage();
		if(!isMotorBatteryConnected()) {
			infoLabel.setText("Mbat not connected:");
			glcd.redraw();
			//TODO set diode blinking
			while(!isMotorBatteryConnected()) {
				motorBatteryVoltage = getMotorBatteryVoltage();
			}
		}
		turnMotorBatteryMosfetOn();
		infoLabel.setText("All ok");
		glcd.redraw();
		_delay_ms(1000);
		//if successfull - delete info label and draw regular gui
		infoLabel.freeBuffer();
		glcd.removeWidget(0);
		gui.init();
		gui.addGUIToDisplay(&glcd);
		glcd.erase();
		glcd.redraw();
		
		turnDUDataOn();
		glcd.backlightOn();
	}


//private:
	void updateBoardState()
	{
		if(newPacketReceived) {
			extractDataFromPacket();
		}
		updateMosfetTemperature();
		sanitizeTemperatures();
		findHighestTemperature();
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
		if(isMotorBatteryConnected()) {
			if(isMotorBatteryLow()) {
				motorsRGB.setBlinking(YELLOW, STATE_CONTROL_BLINK_FAST_PERIOD, STATE_CONTROL_BLINK_DUTY_CYCLE);
			} else {
				if(isMotorBatterySwitchOn()) {
					motorsRGB.setSolid(GREEN);
				} else {
					motorsRGB.setBlinking(GREEN, STATE_CONTROL_BLINK_FAST_PERIOD, STATE_CONTROL_BLINK_DUTY_CYCLE);
				}
			}
		} else {
			motorsRGB.setBlinking(YELLOW, STATE_CONTROL_BLINK_SLOW_PERIOD, STATE_CONTROL_BLINK_DUTY_CYCLE);
		}
		
		if(isTemperatureOk()) {
			temperatureRGB.setSolid(GREEN);
		} else if(getHighestTemperature() < TEMPERATURE_CRITICAL_THRESHOLD) {
			temperatureRGB.setBlinking(YELLOW, STATE_CONTROL_BLINK_FAST_PERIOD, STATE_CONTROL_BLINK_DUTY_CYCLE);
		} else {
			temperatureRGB.setBlinking(RED, STATE_CONTROL_BLINK_FAST_PERIOD, STATE_CONTROL_BLINK_DUTY_CYCLE);
		}
		
	}

	void lcdUpdate()
	{
		gui.setSpeed((int)(odometer.getSpeedKmph()+0.5));
		gui.setDistance(odometer.getDistanceKm());
		gui.setCurrent((int)(getTotalMotorsCurrent() + 0.5));
		gui.setLoad(batteryLoad);
		glcd.redraw();
	}
	
	void batteryLoadUpdate()
	{
		if(latestPacket == NULL) {
			return;
		}
		float totalCurrent = latestPacket->Ia.toFloat() + latestPacket->Ib.toFloat();
		batteryLoad += (totalCurrent * BATTERY_LOAD_UPDATE_PERIOD_MS)*MILIAMPEROSECONDS_TO_MILIAMPEROHOURS; //since dt is in ms, we get load in mAs, multimply that by 1/60*60 to get mAh
		if(batteryLoad >= MOTOR_BATTERY_LOAD_MAX) {
			batteryLoad = 0;
		}
	}
	
	void lightsUpdate()
	{
		if(encoderButton.isPressed()) {
			if(currentTimeUs - lastEncoderButtonPressedTime > MIN_ENCODER_BUTTON_PRESS_INTERVAL_US) {
				incrementFrontLightsState();
				lastEncoderButtonPressedTime = currentTimeUs;
			}
		}
		frontLights.update();
	}
	
	void incrementFrontLightsState()
	{
		if(currentFrontLightsState == FRONT_LIGHTS_OFF) {
			frontLights.setBlinking(FRONT_LIGHTS_STROBE_PERIOD_MS, 0.2 * FRONT_LIGHTS_STROBE_PERIOD_MS);
			currentFrontLightsState = FRONT_LIGHTS_LOW_DUTY_BLINKING;
		} else if(currentFrontLightsState == FRONT_LIGHTS_LOW_DUTY_BLINKING) {
			frontLights.setBlinking(FRONT_LIGHTS_SLOW_BLINKING_PERIOD_MS, 0.5*FRONT_LIGHTS_SLOW_BLINKING_PERIOD_MS);
			currentFrontLightsState = FRONT_LIGHTS_HALF_DUTY_BLINKING;
		} else if(currentFrontLightsState == FRONT_LIGHTS_HALF_DUTY_BLINKING) {
			frontLights.setSolid(true);
			currentFrontLightsState = FRONT_LIGHTS_SOLID;
		} else if(currentFrontLightsState == FRONT_LIGHTS_SOLID) {
			frontLights.setSolid(false);
			currentFrontLightsState = FRONT_LIGHTS_OFF;
		} else {
			frontLights.setSolid(false);
			currentFrontLightsState = FRONT_LIGHTS_OFF;
		}
	}
	
	void extractDataFromPacket()
	{
		latestPacket->loadTemperaturesToArray(temperatures);
		m1current = latestPacket->Ia.toFloat();
		m2current = latestPacket->Ib.toFloat();
	}
	
	void sanitizeTemperatures()
	{
		for(int i = 0; i < THERMOMETERS_COUNT; i++) {
			if(temperatures[i] >= THERMOMETER_UNCONNECTED_MIN_VALUE) {
				temperatures[i] = THERMOMETER_UNCONNECTED_VALUE;
			}
		}
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
		bool lowVoltage = isMotorBatteryLow();
		motorBatteryConnected = isMotorBatteryConnected();
		
		return (!lowVoltage && motorBatteryConnected);
	}
	
	bool isMotorBatteryLow()
	{
		return (motorBatteryVoltage < MOTOR_BATTERY_VOLTAGE_WARNING_THRESHOLD);
	}
	
	bool isMotorBatteryConnected()
	{
		return (motorBatteryVoltage > MOTOR_BATTERY_DISCONNECTED_THRESHOLD);
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
	
	bool isMotorBatterySwitchOn()
	{
		return (PORT(MOTOR_BATTERY_SWITCH_PORT) & pin(MOTOR_BATTERY_SWITCH_PIN));
	}
	
	void turnMotorBatteryMosfetOn()
	{
		PORT(MOTOR_BATTERY_SWITCH_PORT) |= (1<<MOTOR_BATTERY_SWITCH_PIN);
	}
	
	void turnMotorBatteryMosfetOff()
	{
		PORT(MOTOR_BATTERY_SWITCH_PORT) &= ~(1<<MOTOR_BATTERY_SWITCH_PIN);
	}
	
	void turnDUDataOn()
	{
		uart_putc(DU_START_SENDING_COMMAND);
	}
	
	void debugUpdate()
	{
		Debug::print("Temp: ");
		for(int i = 0; i <THERMOMETERS_COUNT; i++) {
			Debug::print(temperatures[i]);
			Debug::print(", ");
		}
		Debug::endl();
		Debug::print("Current: ");
		Debug::print(m1current);
		Debug::print(", ");
		Debug::println(m2current);
		Debug::print("Voltage: CU= ");
		Debug::print(cuBatteryVoltage);
		Debug::print(", motors= ");
		Debug::println(motorBatteryVoltage);
		if(temperaturesOk) {
			Debug::println("Temps ok");
		} else {
			Debug::println("Temps not ok");
		}
		if(motorBatteryVoltageOk) {
			Debug::println("Motor battery ok");
		} else {
			Debug::println("Motor battery not ok");
		}
		Debug::println("----------------------");
	}
	
	unsigned long currentTimeUs;
	unsigned long lcdUpdateTimer;
	unsigned long batteryLoadUpdateTimer;
	unsigned long boardStateUpdateTimer;
	unsigned long stateControlsTimer;
	unsigned long odometerTimer;
	unsigned long debugTimer;
	unsigned long lastEncoderButtonPressedTime;
	
	bool newPacketReceived;
	Packet* latestPacket;
	
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
	
	RGBLed motorsRGB;
	RGBLed temperatureRGB;
	GPSOdometry odometer;
	GLCD glcd;
	EboardGUI gui;
	LED frontLights;
	Button encoderButton;
	uint8_t currentFrontLightsState;
};

#endif