#ifndef LIB_SERVICE_RPI_METHODS_H_
#define LIB_SERVICE_RPI_METHODS_H_

#include "Buffer.h"
#include "ServiceRpi.h"

#include "ADCSample.h"

/*
 * Raspberry Pi Services Methods Definitions
 */

void Service_Rpi_storeSample(Buffer* packet, ADC_Sample* sample) {
	size_t size = 0;
	size += Service_Type_serialize(&packet->data[size], SERVICE_TYPE_REQUEST);
	size += Service_Rpi_serialize(&packet->data[size], SERVICE_RPI_STORE_SAMPLE);
	size += ADC_Sample_serialize(&packet->data[size], sample);
	packet->size = size;
}

#endif /* LIB_SERVICE_RPI_METHODS_H_ */
