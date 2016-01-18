#ifndef LIB_SERVICE_TYPE_H_
#define LIB_SERVICE_TYPE_H_

typedef enum {
	SERVICE_TYPE_REQUEST = 0x00, // Packet contains a service request
	SERVICE_TYPE_RESPONSE = 0x01 // Packet contains a service response (response to a previous request)
} Service_Type;

size_t Service_Type_serialize(char* buffer, Service_Type type) {
	size_t size = 0;
	size += Buffer_writeUInt8(&buffer[size], type);
	return size;
}

#endif // LIB_SERVICE_TYPE_H_