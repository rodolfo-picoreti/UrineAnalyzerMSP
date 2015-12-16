#ifndef LIB_REMOTECLIENTSERVICES_H_
#define LIB_REMOTECLIENTSERVICES_H_

#include "Buffer.h"
#include "ADCSample.h"

void Service_Request_newADCSample(Buffer* packet, ADC_Sample* sample) {
	size_t size = 0;
	size += Buffer_writeUInt8(&packet->data[size], MESSAGE_REQUEST);
	size += Buffer_writeUInt8(&packet->data[size], CLIENT_SERVICE_NEW_ADC_SAMPLE);
	size += ADC_Sample_serialize(&packet->data[size], sample);
	packet->size = size;
}

void Service_Response_newADCSample(Buffer* packet) {

}

void Service_Request_timeSynchronization(Buffer* packet) {

}

void Service_Response_timeSynchronization(Buffer* packet) {

}

#endif /* LIB_REMOTECLIENTSERVICES_H_ */
