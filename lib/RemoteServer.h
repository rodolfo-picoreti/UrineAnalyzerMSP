#ifndef LIB_REMOTESERVER_H_
#define LIB_REMOTESERVER_H_

#include "Buffer.h"
#include "RemoteMessage.h"

typedef enum {
	SERVER_SERVICE_CONFIG_MOTOR = 0x00,
	SERVER_SERVICE_CONFIG_VALVES = 0x01,
	SERVER_SERVICE_START_SAMPLING = 0x02,
	SERVER_SERVICE_STOP_SAMPLING = 0x03,
	SERVER_SERVICE_PWM = 0x04,
	SERVER_SERVICE_VALVES = 0x05,
	SERVER_SERVICE_SD_LEDS = 0x06,
	SERVER_SERVICE_TEMP_CONTROL = 0x07,
	SERVER_SERVICE_BLINK_LED = 0xFF
} Remote_Server_Service;

size_t Remote_Server_Service_serialize(char* buffer, Remote_Server_Service service) {
	size_t size = 0;
	size += Buffer_writeUInt8(&buffer[size], service);
	return size;
}

#endif /* LIB_REMOTESERVER_H_ */
