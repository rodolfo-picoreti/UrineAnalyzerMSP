#ifndef LIB_REMOTEPARSERS_H_
#define LIB_REMOTEPARSERS_H_

#include "Buffer.h"
#include "RemoteMessage.h"
#include "RemoteServer.h"
#include "RemoteClient.h"
#include "RemoteServerServices.h"
#include "RemoteClientServices.h"
#include "Sync.h"

void Remote_Server_Service_parse(Buffer* packet) {
/*
 * Response Packet:
 *
 * 	  Byte			Description
 *  	0			 Message_Type
 *  	1			 Service
 *  	2			 Status
 *  	3~254        Service_Result
 */
	Buffer_writeUInt8(packet->data, MESSAGE_RESPONSE);

	switch (packet->data[1]) {
		case SERVER_SERVICE_CONFIG_MOTOR:
		case SERVER_SERVICE_CONFIG_VALVES:
			Service_notImplemented(packet);
			break;

		case SERVER_SERVICE_START_SAMPLING:
			Service_startSampling(packet);
			break;

		case SERVER_SERVICE_STOP_SAMPLING:
			Service_stopSampling(packet);
			break;

		case SERVER_SERVICE_PWM:
			Service_pwm(packet);
			break;

		case SERVER_SERVICE_VALVES:
			Service_valves(packet);
			break;

		case SERVER_SERVICE_SD_LEDS:
			Service_sdLeds(packet);
			break;

		case SERVER_SERVICE_TEMP_CONTROL:
			Service_temperatureControl(packet);
			break;

		case SERVER_SERVICE_BLINK_LED:
			Service_blinkLed(packet);
			break;

		default:
			Service_invalid(packet);
			break;
	}
}

void Remote_Client_Response_parse(Buffer* packet) {
	Remote_Client_Service service;
	Remote_Message_Status status;
	Buffer_readUInt8(&packet->data[1], &service);
	Buffer_readUInt8(&packet->data[2], &status);
}

void Remote_parse(Buffer* packet) {
	// byte 0 contains the Message_Type
	switch (packet->data[0]) {
		case MESSAGE_REQUEST:
			Remote_Server_Service_parse(packet);
			Sync_postBuffer(packet);
			packet->data = NULL;
			break;

		case MESSAGE_RESPONSE:
			Remote_Client_Response_parse(packet);
			break;

		default:
			break;
	}
}

#endif /* LIB_REMOTEPARSERS_H_ */
