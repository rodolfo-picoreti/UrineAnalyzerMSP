#ifndef LIB_ADCSAMPLE_H_
#define LIB_ADCSAMPLE_H_

#include <stdatomic.h>
#include "Buffer.h"

#define SENSOR_NUM 9

atomic_uint_least16_t ADC_values[SENSOR_NUM];
static uint8_t ADC_currentSensorId;

typedef enum {
	SENSOR_PH = 0,
	SENSOR_NA = 1,
	SENSOR_CL = 2,
	SENSOR_K = 3,
	SENSOR_CONDUCTIVITY = 4,
	SENSOR_PREHEATER_TEMP = 5,
	SENSOR_INTHEATER_TEMP = 6,
	SENSOR_SD1 = 7,
	SENSOR_SD2 = 8,

} ADC_Sensor_Id;

ADC_Sensor_Id ADC_getCurrentSensorId() {
	return ADC_currentSensorId;
}

uint32_t samplePreScalerCounter = 0;

void ADC_selectNextSensor() {
	// Collect samples from the same sensor 100 times before changing to next
	if (samplePreScalerCounter > 100) {
		++ADC_currentSensorId;

		if (ADC_currentSensorId >= 5) {
			ADC_currentSensorId = 0;
		}
		/*
		 * 1.7, 5.1 - MUXSEL
		 * 3.7(0), 3.5 (1), 2.3 (2) - MUXSELECTS
		 */
		GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN7);
		GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN1);

		switch (ADC_currentSensorId) {
			case SENSOR_PH: // 0 1 1
				GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN7);
				GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN5);
				GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN3);
				break;

			case SENSOR_NA: // 0 0 0
				GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN7);
				GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN5);
				GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN3);
				break;

			case SENSOR_CL: // 0 1 0
				GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN7);
				GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN5);
				GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN3);
				break;

			case SENSOR_K: // 1 0 0
				GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN7);
				GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN5);
				GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN3);
				break;

			case SENSOR_CONDUCTIVITY: // 1 1 1
				GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN7);
				GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN5);
				GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN3);
				break;

			default:
				break;
		}
		samplePreScalerCounter = 0;
		GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
	}
	++samplePreScalerCounter;
}

uint16_t ADC_loadReading(ADC_Sensor_Id id) {
	return atomic_load(&ADC_values[id]);
}

void ADC_storeReading(ADC_Sensor_Id id, uint16_t value) {
	atomic_store(&ADC_values[id], value);
}

typedef struct {
	uint16_t values[SENSOR_NUM];
	uint64_t timestamp;
	uint32_t counter;
} ADC_Sample;

void ADC_Sample_setup() {
	ADC_currentSensorId = 0;
}

void ADC_Sample_initialize(ADC_Sample* sample) {
	uint8_t i;
	for (i = 0; i < SENSOR_NUM; ++i) {
		sample->values[i] = ADC_loadReading(i);
	}
}

size_t ADC_Sample_serialize(char* buffer, ADC_Sample* sample) {
	size_t size = 0;

	uint8_t i;
	for (i = 0; i < SENSOR_NUM; ++i) {
		size += Buffer_writeUInt16BigEndian(&buffer[size], sample->values[i]);
	}

	size += Buffer_writeUInt48BigEndian(&buffer[size], sample->timestamp);
	size += Buffer_writeUInt32BigEndian(&buffer[size], sample->counter);
	return size;
}

#endif /* LIB_SAMPLE_H_ */
