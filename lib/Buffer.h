#ifndef LIB_BUFFER_H_
#define LIB_BUFFER_H_

#include <xdc/std.h>

// Used to compose packets and to serialize/deserialize data

typedef struct {
	char* data;
	size_t size;
} Buffer;

size_t Buffer_writeUInt8(char* buffer, uint8_t value) {
	buffer[0] = value;
	return 1;
}

size_t Buffer_writeUInt16BigEndian(char* buffer, uint16_t value) {
	buffer[0] = (value & 0xFF00) >> 8;
	buffer[1] = (value & 0x00FF);
	return 2;
}

size_t Buffer_writeUInt32BigEndian(char* buffer, uint32_t value) {
	buffer[0] = (value & 0xFF000000) >> 24;
	buffer[1] = (value & 0x00FF0000) >> 16;
	buffer[2] = (value & 0x0000FF00) >> 8;
	buffer[3] = (value & 0x000000FF);
	return 4;
}

size_t Buffer_writeUInt48BigEndian(char* buffer, uint64_t value) {
	buffer[0] = (value & 0xFF0000000000) >> 40;
	buffer[1] = (value & 0x00FF00000000) >> 32;
	buffer[2] = (value & 0x0000FF000000) >> 24;
	buffer[3] = (value & 0x000000FF0000) >> 16;
	buffer[4] = (value & 0x00000000FF00) >> 8;
	buffer[5] = (value & 0x0000000000FF);
	return 6;
}

void Buffer_writeUIntBigEndian(char* buffer, size_t N, uint64_t value) {
	uint64_t mask = 0xFF;

	for (; N > 0; --N) {
		buffer[N-1] = value & mask;
		mask = mask << 8;
	}
}

uint64_t Buffer_readUIntBigEndian(char* buffer, size_t N) {
	uint64_t value = 0;

	int i;
	for (i = 0; i < N-1; ++i) {
		value |= buffer[i];
		value = value << 8;
	}
	value |= buffer[N-1];

	return value;
}

size_t Buffer_readUInt8(char* buffer, uint8_t* value) {
	*value = 0;
	*value |= buffer[0];
	return 1;
}

size_t Buffer_readUInt16BigEndian(char* buffer, uint16_t* value) {
	*value = 0;
	*value |= buffer[0] << 8;
	*value |= buffer[1];
	return 2;
}

size_t Buffer_readUInt32BigEndian(char* buffer, uint32_t* value) {
	*value = 0;
	*value |= buffer[0] << 24;
	*value |= buffer[1] << 16;
	*value |= buffer[2] << 8;
	*value |= buffer[3];
	return 4;
}

size_t Buffer_readUInt48BigEndian(char* buffer, uint64_t* value) {
	*value = Buffer_readUIntBigEndian(buffer, 6);
	return 6;
}


#endif /* LIB_BUFFER_H_ */
