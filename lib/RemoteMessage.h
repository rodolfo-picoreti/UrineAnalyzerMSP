#ifndef LIB_REMOTEMESSAGE_H_
#define LIB_REMOTEMESSAGE_H_

typedef enum {
	MESSAGE_REQUEST = 0x00,
	MESSAGE_RESPONSE = 0x01
} Remote_Message_Type;

size_t Remote_Message_Type_serialize(char* buffer, Remote_Message_Type type) {
	size_t size = 0;
	size += Buffer_writeUInt8(&buffer[size], type);
	return size;
}

typedef enum {
	STATUS_SUCCESS = 0x00,  // Requested service is valid and was successfully executed
	STATUS_INVALID_SERVICE = 0x01,  // Requested service is invalid
	STATUS_INVALID_LENGHT = 0x02, // Request does not have the right size
	STATUS_INVALID_ARG = 0x03, // Request have an invalid argument
	STATUS_BUSY = 0x04, // Service is being provided and cannot be called again
	STATUS_NOT_IMPLEMENTED = 0xFF // Requested service is valid but was not implemented
} Remote_Message_Status;

size_t Remote_Message_Status_serialize(char* buffer, Remote_Message_Status status) {
	size_t size = 0;
	size += Buffer_writeUInt8(&buffer[size], status);
	return size;
}

#endif /* LIB_REMOTEMESSAGE_H_ */
