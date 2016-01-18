#ifndef LIB_SERVICE_MSP_H_
#define LIB_SERVICE_MSP_H_

#include "Buffer.h"

// MSP Service Ids
typedef enum {
	//
	SERVICE_MSP_CONFIG_LOCKS = 0x00,
	
	SERVICE_MSP_START_SAMPLING = 0x01,
	SERVICE_MSP_STOP_SAMPLING = 0x02,
	SERVICE_MSP_SELECT_SENSOR = 0x03,

	SERVICE_MSP_SAMPLE_ONCE = 0x04,
	SERVICE_MSP_RESET_COUNTER = 0x05,

	SERVICE_MSP_CONFIG_VALVES = 0x10,
	SERVICE_MSP_CONFIG_PUMPS = 0x11,
	SERVICE_MSP_CONFIG_HEATERS = 0x12,
	SERVICE_MSP_CONFIG_SD_LEDS = 0x13,

} Service_Msp;

size_t Service_Msp_serialize(char* buffer, Service_Msp service) {
	size_t size = 0;
	size += Buffer_writeUInt8(&buffer[size], service);
	return size;
}

#endif /* LIB_SERVICE_MSP_H_ */
