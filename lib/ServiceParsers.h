#ifndef LIB_SERVICEPARSERS_H_
#define LIB_SERVICEPARSERS_H_

#include "Buffer.h"
#include "Sync.h"
#include "ServiceStatus.h"
#include "ServiceType.h"
#include "ServiceMsp.h"
#include "ServiceMspMethods.h"

void Service_Msp_parse(Buffer* packet) {

	// The second byte represents the requested service id
	switch (packet->data[1]) {
		case SERVICE_MSP_CONFIG_LOCKS:
			Service_Msp_configureLocks(packet); break;
	
		case SERVICE_MSP_START_SAMPLING: 
			Service_Msp_startSampling(packet); break;

		case SERVICE_MSP_STOP_SAMPLING: 
			Service_Msp_stopSampling(packet); break;

		case SERVICE_MSP_SELECT_SENSOR:
			Service_Msp_selectSensor(packet); break;

		case SERVICE_MSP_RESET_COUNTER:
			Service_Msp_resetCounter(packet); break;

		case SERVICE_MSP_SAMPLE_ONCE:
			Service_Msp_sampleOnce(packet); break;

		case SERVICE_MSP_CONFIG_VALVES:
			Service_Msp_configureValves(packet); break;
		
		case SERVICE_MSP_CONFIG_PUMPS:
			Service_Msp_configurePumps(packet); break;
		
		case SERVICE_MSP_CONFIG_HEATERS:
			Service_Msp_configureHeaters(packet); break;

		case SERVICE_MSP_CONFIG_SD_LEDS:
			Service_Msp_configureSD(packet); break;

		default:
			Service_Msp_Helper_notImplementd(packet); break;
	}
	
	/*
	 * This Buffer (packet) will be reused to send the response, therefore
	 * change the type to response
	 */
	
	// TODO: Change hardcoded positions 
	Service_Type_serialize(&packet->data[0], SERVICE_TYPE_RESPONSE);
}

void Service_parse(Buffer* packet) {
	// byte 0 contains the Message_Type
	switch (packet->data[0]) {
		case SERVICE_TYPE_REQUEST:
			// parses the request and reuse the buffer to send the response
			Service_Msp_parse(packet); 
			Sync_postBuffer(packet);
			packet->data = NULL;
			break;

		case SERVICE_TYPE_RESPONSE:
			// response to a request made by the MSP
			break;

		default:
			break;
	}
}

#endif /* LIB_SERVICEPARSERS_H_ */
