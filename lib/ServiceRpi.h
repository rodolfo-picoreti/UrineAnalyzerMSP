#ifndef LIB_SERVICE_RPI_H_
#define LIB_SERVICE_RPI_H_

// RPI Service Ids

typedef enum {
	SERVICE_RPI_STORE_SAMPLE = 0x00 
} Service_Rpi;

size_t Service_Rpi_serialize(char* buffer, Service_Rpi service) {
	size_t size = 0;
	size += Buffer_writeUInt8(&buffer[size], service);
	return size;
}

#endif /* LIB_SERVICE_RPI_H_ */
