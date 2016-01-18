#ifndef LIB_SERVICE_STATUS_H_
#define LIB_SERVICE_STATUS_H_

typedef enum {
	SERVICE_STATUS_SUCCESS = 0x00,  // Requested service is valid and was successfully executed
	SERVICE_STATUS_INVALID_SERVICE = 0x01,  // Requested service is invalid
	SERVICE_STATUS_INVALID_LENGHT = 0x02, // Request does not have the right size
	SERVICE_STATUS_INVALID_ARG = 0x03, // Request have an invalid argument
	SERVICE_STATUS_BUSY = 0x04, // Service is being provided and cannot be called again
	SERVICE_STATUS_NOT_IMPLEMENTED = 0xFF // Requested service is valid but was not implemented
} Service_Status;

size_t Service_Status_serialize(char* buffer, Service_Status status) {
	size_t size = 0;
	size += Buffer_writeUInt8(&buffer[size], status);
	return size;
}

#endif // LIB_SERVICE_STATUS_H_