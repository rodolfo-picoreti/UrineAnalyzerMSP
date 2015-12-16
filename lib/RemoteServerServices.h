#ifndef LIB_REMOTESERVERSERVICES_H_
#define LIB_REMOTESERVERSERVICES_H_

#include "RemoteServer.h"
#include "Buffer.h"
#include "PWM.h"
#include "driverlib.h"
#include "Valves.h"
#include "PID.h"

void Service_notImplemented(Buffer* packet) {
	Remote_Message_Status_serialize(&packet->data[2], STATUS_NOT_IMPLEMENTED);
	packet->size = 3;
}

void Service_invalid(Buffer* packet) {
	Remote_Message_Status_serialize(&packet->data[2], STATUS_INVALID_SERVICE);
	packet->size = 3;
}

void Service_startSampling(Buffer* packet) {
	if (packet->size != 4) {
		Remote_Message_Status_serialize(&packet->data[2], STATUS_INVALID_LENGHT);
		packet->size = 3;
		return;
	}

	if (ADC_samplingEnabled()) {
		Remote_Message_Status_serialize(&packet->data[2], STATUS_BUSY);
		packet->size = 3;
		return;
	}

	uint16_t samplingTime;
	Buffer_readUInt16BigEndian(&packet->data[2], &samplingTime);

	if (samplingTime == 0) {
		Remote_Message_Status_serialize(&packet->data[2], STATUS_INVALID_ARG);
		packet->size = 3;
		return;
	}

	ADC_setSamplingTime(samplingTime);
	ADC_resetCounter();
	ADC_enableSampling();

	Remote_Message_Status_serialize(&packet->data[2], STATUS_SUCCESS);
	Buffer_writeUInt16BigEndian(&packet->data[3], samplingTime);
	packet->size = 5;
}

void Service_stopSampling(Buffer* packet) {
	if (packet->size != 2) {
		Remote_Message_Status_serialize(&packet->data[2], STATUS_INVALID_LENGHT);
	}
	else {
		ADC_disableSampling();
		Remote_Message_Status_serialize(&packet->data[2], STATUS_SUCCESS);
	}
	packet->size = 3;
}

void Service_pwm(Buffer* packet) {
	if (packet->size != 7) {
		Remote_Message_Status_serialize(&packet->data[2], STATUS_INVALID_LENGHT);
		packet->size = 3;
		return;
	}

	uint8_t pin;
	Buffer_readUInt8(&packet->data[2], &pin);

	uint32_t dutyCycle;
	Buffer_readUInt32BigEndian(&packet->data[3], &dutyCycle);

	if ((pin == PWM_INTHEATER || pin == PWM_PREHEATER) && PID_getMode()) {
		Remote_Message_Status_serialize(&packet->data[2], STATUS_BUSY);
		packet->size = 3;
		return;
	}

	PWM_setDutyCycle(pin, dutyCycle);

	Remote_Message_Status_serialize(&packet->data[2], STATUS_SUCCESS);
	Buffer_writeUInt8(&packet->data[3], pin);
	Buffer_writeUInt32BigEndian(&packet->data[4], dutyCycle);
	packet->size = 8;
}

void Service_valves(Buffer* packet) {
	if (packet->size != 4) {
		Remote_Message_Status_serialize(&packet->data[2], STATUS_INVALID_LENGHT);
		packet->size = 3;
		return;
	}

	uint8_t valve;
	Buffer_readUInt8(&packet->data[2], &valve);

	uint8_t value;
	Buffer_readUInt8(&packet->data[3], &value);

	Valves_output(valve, value);

	Remote_Message_Status_serialize(&packet->data[2], STATUS_SUCCESS);
	Buffer_writeUInt8(&packet->data[3], valve);
	Buffer_writeUInt8(&packet->data[4], value);
	packet->size = 5;
}

void Service_sdLeds(Buffer* packet) {
	if (packet->size != 3) {
		Remote_Message_Status_serialize(&packet->data[2], STATUS_INVALID_LENGHT);
		packet->size = 3;
		return;
	}

	uint8_t value;
	Buffer_readUInt8(&packet->data[2], &value);

	// Pins: 5.5 6.4
	if (value) {
		GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN5);
		GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN4);
	}
	else {
		GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN5);
		GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);
	}

	Remote_Message_Status_serialize(&packet->data[2], STATUS_SUCCESS);
	Buffer_writeUInt8(&packet->data[3], value);
	packet->size = 4;
}

void Service_temperatureControl(Buffer* packet) {
	if (packet->size != 3) {
		Remote_Message_Status_serialize(&packet->data[2], STATUS_INVALID_LENGHT);
		packet->size = 3;
		return;
	}

	uint8_t value;
	Buffer_readUInt8(&packet->data[2], &value);
	PID_setMode(value);

	Remote_Message_Status_serialize(&packet->data[2], STATUS_SUCCESS);
	Buffer_writeUInt8(&packet->data[3], value);
	packet->size = 4;
}

void Service_blinkLed(Buffer* packet) {
	GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);

	Remote_Message_Status_serialize(packet->data + 2, STATUS_SUCCESS);
	packet->size = 3;
}



#endif /* LIB_REMOTESERVERSERVICES_H_ */
