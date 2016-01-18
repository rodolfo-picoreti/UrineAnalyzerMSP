#ifndef LIB_SERVICE_MSP_METHODS_H_
#define LIB_SERVICE_MSP_METHODS_H_

#include "Buffer.h"

#include "PWM.h"
#include "driverlib.h"
#include "Valves.h"
#include "PID.h"
#include "ADCOptions.h"

#include "ServiceStatus.h"
#include "ServiceLock.h"

void Service_Msp_Helper_invalidLength(Buffer* packet) {
	Service_Status_serialize(&packet->data[2], SERVICE_STATUS_INVALID_LENGHT);
	packet->size = 3;
}

void Service_Msp_Helper_busy(Buffer* packet) {
	Service_Status_serialize(&packet->data[2], SERVICE_STATUS_BUSY);
	packet->size = 3;
}

void Service_Msp_Helper_success(Buffer* packet) {
	Service_Status_serialize(&packet->data[2], SERVICE_STATUS_SUCCESS);
	packet->size = 3;
}

void Service_Msp_Helper_invalidArg(Buffer* packet) {
	Service_Status_serialize(&packet->data[2], SERVICE_STATUS_INVALID_ARG);
	packet->size = 3;
}

void Service_Msp_Helper_notImplementd(Buffer* packet) {
	Service_Status_serialize(&packet->data[2], SERVICE_STATUS_NOT_IMPLEMENTED);
	packet->size = 3;
}

/*
 * MSP Services Methods Definitions
 */

void Service_Msp_configureLocks(Buffer* packet) {
	if ((packet->size % 2) != 0) {
		Service_Msp_Helper_invalidLength(packet);
		return;
	}

	uint8_t id;
	uint8_t value;
	size_t i;

	for (i = 2; i < packet->size; i += 2) {
		Buffer_readUInt8(&packet->data[i], &id);
		Buffer_readUInt8(&packet->data[i+1], &value);

		if (value) Service_lock(id);
		else Service_unlock(id);
	}

	Service_Msp_Helper_success(packet);
}

void Service_Msp_startSampling(Buffer* packet) {
	if (packet->size != 6) {
		Service_Msp_Helper_invalidLength(packet);
		return;
	}

	if (ADC_samplingEnabled()) {
		Service_Msp_Helper_busy(packet);
		return;
	}

	uint32_t samplingTime;
	Buffer_readUInt32BigEndian(&packet->data[2], &samplingTime);

	if (samplingTime == 0) {
		Service_Msp_Helper_invalidArg(packet);
		return;
	}

	ADC_setSamplingTime(samplingTime);
	ADC_resetCounter();
	ADC_enableSampling();

	Service_Msp_Helper_success(packet);
}

void Service_Msp_stopSampling(Buffer* packet) {
	if (packet->size != 2) {
		Service_Msp_Helper_invalidLength(packet);
		return;
	}

	ADC_disableSampling();
	Service_Msp_Helper_success(packet);
}

void Service_Msp_configureValves(Buffer* packet) {
	if ((packet->size < 4) || (packet->size > 12) || (packet->size % 2 != 0)) {
		Service_Msp_Helper_invalidLength(packet);
		return;
	}

	if (Service_isLocked(SERVICE_LOCK_VALVES)) {
		Service_Msp_Helper_busy(packet);
		return;
	}

	uint8_t valve;
	uint8_t value;
	size_t i;

	for (i = 2; i < packet->size; i += 2) {
		Buffer_readUInt8(&packet->data[i], &valve); 
		Buffer_readUInt8(&packet->data[i+1], &value); 
		Valves_output(valve, value);
	}

	Service_Msp_Helper_success(packet);
}

void Service_Msp_configurePumps(Buffer* packet) {
	if ((packet->size != 5) && (packet->size != 8)) {
		Service_Msp_Helper_invalidLength(packet);
		return;
	}
	
	if (Service_isLocked(SERVICE_LOCK_PUMPS)) {
		Service_Msp_Helper_busy(packet);
		return;
	}

	uint8_t pump;
	uint16_t dutyCycle;
	size_t i;

	for (i = 2; i < packet->size; i += 3) {
		Buffer_readUInt8(&packet->data[i], &pump);
		Buffer_readUInt16BigEndian(&packet->data[i + 1], &dutyCycle);
		
		if ((pump != PWM_REAGENT_PUMP) && (pump != PWM_SAMPLE_WASTE_PUMP)) {
			Service_Msp_Helper_invalidArg(packet);
			return;
		}
		
		PWM_setDutyCycle(pump, dutyCycle);
	}

	Service_Msp_Helper_success(packet);
}

void Service_Msp_configureHeaters(Buffer* packet) {
	if ((packet->size != 5) && (packet->size != 8)) {
		Service_Msp_Helper_invalidLength(packet);
		return;
	}

	if (Service_isLocked(SERVICE_LOCK_HEATERS)) {
		Service_Msp_Helper_busy(packet);
		return;
	}

	uint8_t heater;
	uint16_t dutyCycle;
	size_t i;

	for (i = 2; i < packet->size; i += 3) {
		Buffer_readUInt8(&packet->data[i], &heater);
		Buffer_readUInt16BigEndian(&packet->data[i+1], &dutyCycle);

		if ((heater != PWM_PREHEATER) && (heater != PWM_INTHEATER)) {
			Service_Msp_Helper_invalidArg(packet);
			return;
		}
		
		PWM_setDutyCycle(heater, dutyCycle);
	}

	Service_Msp_Helper_success(packet);
}

void Service_Msp_selectSensor(Buffer* packet) {
	if (packet->size != 3) {
		Service_Msp_Helper_invalidLength(packet);
		return;
	}

	if (Service_isLocked(SERVICE_LOCK_SENSOR_SELECT)) {
		Service_Msp_Helper_busy(packet);
		return;
	}

	uint8_t id;
	Buffer_readUInt8(&packet->data[2], &id);
	ADC_select(id);

	Service_Msp_Helper_success(packet);
}

void Service_Msp_sampleOnce(Buffer* packet) {
	if (ADC_samplingEnabled()) {
		Service_Msp_Helper_busy(packet);
		return;
	}

	ADC_Sample sample;
	ADC_Sample_initialize(&sample);
	ADC_incrementCounter();
	
	Service_Msp_Helper_success(packet);
	packet->size += ADC_Sample_serialize(&packet->data[3], &sample);
}

void Service_Msp_resetCounter(Buffer* packet) {
	if (ADC_samplingEnabled()) {
		Service_Msp_Helper_busy(packet);
		return;
	}

	ADC_resetCounter();
	Service_Msp_Helper_success(packet);
}

void Service_Msp_configureSD(Buffer* packet) {
	Service_Msp_Helper_notImplementd(packet);
}

#endif /* LIB_SERVICE_MSP_METHODS_H_ */
