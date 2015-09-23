#ifndef ADCONVERTER_H
#define ADCONVERTER_H

#include <avr/io.h>

namespace ADConverterSettings
{
	float referenceVoltage;
	uint16_t maxValue;
};

class ADConverter
{
public:
	ADConverter()
	{
		
	}
	
	static void init()
	{
		ADMUX |= (1<<REFS0); //ref is AVcc
		ADCSRA |= (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); //enable ADC, 128div factor
		
		ADConverterSettings::referenceVoltage = 5.0; //V
		ADConverterSettings::maxValue = 1024;
	}
	
	uint16_t static getRawMeasurement(uint8_t channel)
	{
		setChannel(channel);
		startConversion();
		while(!isCoversionFinished());
		return ADC;
	}
	
	//offset is SUBTRACTED from every raw reading!
	uint32_t static getRawSum(uint8_t channel, int samplesCount, int16_t offset = 0) 
	{
		uint32_t sum = 0;
		for(int i =0; i < samplesCount; i++) {
			uint16_t singleRawMeasurement = getRawMeasurement(channel);
			
			if(singleRawMeasurement < offset) { //in case when offset is larger than measurement, in order not to get overflow
				singleRawMeasurement = 0;
			} else {
				singleRawMeasurement -= offset;
			}
			sum += singleRawMeasurement;
		}
		return sum;
	}
	
	float static getVoltage(uint8_t channel)
	{
		uint16_t raw = getRawMeasurement(channel);
		return convertRawMeasurementToVoltage(raw);
	}
	
	float static getAverageVoltage(uint8_t channel, int numberOfSamples) {
		unsigned long sum = 0;
		for(int i = 0; i < numberOfSamples; i++) {
			sum += getRawMeasurement(channel);
		}
		uint16_t avg = sum/numberOfSamples;
		return convertRawMeasurementToVoltage(avg);
	}
private:

	inline static void setChannel(uint8_t channel)
	{
		ADMUX = (ADMUX & 0xF0) | channel; //set appropiate channel
	}
	inline static void startConversion()
	{
		ADCSRA |= (1<<ADSC); //start conversion
	}
	inline static bool isCoversionFinished()
	{
		return !(ADCSRA & (1<<ADSC)); //checking AD Start Conversion flag, which is set to 1 if conversion is pending
	}
	float static convertRawMeasurementToVoltage(uint16_t rawReading)
	{
		return (float)rawReading/ADConverterSettings::maxValue*ADConverterSettings::referenceVoltage;
	}


};
#endif
