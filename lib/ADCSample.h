#ifndef LIB_ADCSAMPLE_H_
#define LIB_ADCSAMPLE_H_

#include <stdatomic.h>
#include "Buffer.h"

/*
 * Sensor definitions to increase readability
 */

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

/* 
 * Ph, Na, Cl, K and Conductivity sensors are sampled in
 * the same pin by using a multiplexer. 
 * ADC_currentSensorId controls which of then are being 
 * read at the moment.
 */
static uint8_t ADC_currentSensorId;

ADC_Sensor_Id ADC_getCurrentSensorId() {
	return ADC_currentSensorId;
}

/*
 * Each sensor in the multiplexer is read sampleHoldMax times
 * before changing to the next sensor
 */
static uint32_t sampleHold = 0;
static const uint32_t sampleHoldMax = 10;

/*
 * Atomic variables are used to avoid race conditions between 
 * the ADC interrupt and other threads.
 */

#define SENSOR_NUM 9
atomic_uint_least16_t ADC_values[SENSOR_NUM];

uint16_t ADC_loadReading(ADC_Sensor_Id id) {
	return atomic_load(&ADC_values[id]);
}

void ADC_storeReading(ADC_Sensor_Id id, uint16_t value) {
	atomic_store(&ADC_values[id], value);
}

float smooth(uint16_t current, uint16_t past) {
	return 0.05f*current + 0.95f*past;
}

void ADC_storeSmoothedReading(ADC_Sensor_Id id, uint16_t value) {
	ADC_storeReading(id, smooth(value, ADC_loadReading(id)));
}


// Configure mux select pins as outputs
void ADC_Sample_setup() {
	ADC_currentSensorId = 0;
	GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN7);
	GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN1);
	GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN7);
	GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN5);
	GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN3);
}

/*
 * Change mux select pins to correct value in order to select
 * the sensor with the specified id 
 */
void ADC_select(ADC_Sensor_Id id) {
	/*
	 * MUXSEL:
	 *    0: 1.7
	 *    1: 5.1
	 *
	 * MUXSELECTS:
	 *    0: 3.7
	 *    1: 3.5
     *    2: 2.3 
	 */

	switch (id) {
		case SENSOR_PH: // 3.7 = 0, 3.5 = 1, 2.3 = 1
			GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN7);
			GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN1);
			GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN7);
			GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN5);
			GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN3);
			break;

		case SENSOR_NA: // 3.7 = 0, 3.5 = 0, 2.3 = 0
			GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN7);
			GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN1);
			GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN7);
			GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN5);
			GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN3);
			break;

		case SENSOR_CL: // 3.7 = 0, 3.5 = 1, 2.3 = 0
			GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN7);
			GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN1);
			GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN7);
			GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN5);
			GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN3);
			break;

		case SENSOR_K: // 3.7 = 1, 3.5 = 0, 2.3 = 0
			GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN7);
			GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN1);
			GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN7);
			GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN5);
			GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN3);
			break;

		case SENSOR_CONDUCTIVITY: // 3.7 = 1, 3.5 = 1, 2.3 = 1
			GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN7);
			GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN1);
			GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN7);
			GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN5);
			GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN3);
			break;

		default:
			return;
	}

	ADC_currentSensorId = id;

	// Blink sampling led (debug only)
	GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN0);
}

void ADC_selectNextSensor() {
	// Sample the same sensor sampleHoldMax times before selecting next
	if (sampleHold > sampleHoldMax) {
		++ADC_currentSensorId;

		if (ADC_currentSensorId >= 5) {
			ADC_currentSensorId = 0;
		}

		ADC_select(ADC_currentSensorId);
		sampleHold = 0;
	}
	++sampleHold;
}

/*
 * Packs all the ADC measurements into one sampling object
 */
typedef struct {
	uint16_t values[SENSOR_NUM];
	uint32_t timestamp;
	uint32_t counter;
} ADC_Sample;

void ADC_Sample_initialize(ADC_Sample* sample) {
	uint8_t i;
	for (i = 0; i < SENSOR_NUM; ++i) {
		sample->values[i] = ADC_loadReading(i);
	}
	sample->counter = ADC_getCounter();
	sample->timestamp = Clock_seconds();
}

size_t ADC_Sample_serialize(char* buffer, ADC_Sample* sample) {
	size_t size = 0;

	uint8_t i;
	for (i = 0; i < SENSOR_NUM; ++i) {
		size += Buffer_writeUInt16BigEndian(&buffer[size], sample->values[i]);
	}

	size += Buffer_writeUInt32BigEndian(&buffer[size], sample->timestamp);
	size += Buffer_writeUInt32BigEndian(&buffer[size], sample->counter);
	return size;
}

#endif /* LIB_SAMPLE_H_ */
