#ifndef LIB_ADCOPTIONS_H_
#define LIB_ADCOPTIONS_H_

/*
 * >> Options to control sampling behavior:
 * 		
 *		Sampling Time: This is not the actual sampling time, but the rate 
 *					   that the RPI will receive samples in seconds. Real 
 *					   sampling time is configured on "ADC.h" header.
 *
 *		Sampling Count: Stores how many samples have been collected since
 *                      the last time sampling as enabled.
 *
 *		Sampling Enable: Controls if sampling data should be send to the 
 *                       RPI (true) or not (false). 
 *
 * !!!! All the options are atomic in order to avoid race conditions between
 *      different threads
 */

typedef struct {
	atomic_bool samplingEnabled;
	atomic_uint_least32_t samplingTime; // seconds
	atomic_uint_least32_t sampleCount;
} ADC_Options;

static ADC_Options options;

void ADC_Options_setup() {
	atomic_store(&options.samplingEnabled, false);
}

void ADC_setSamplingTime(uint32_t ts) {
	atomic_store(&options.samplingTime, ts);
}

uint32_t ADC_getSamplingTime() {
	return atomic_load(&options.samplingTime);
}

void ADC_incrementCounter() {
	atomic_fetch_add_explicit(&options.sampleCount, 1, memory_order_relaxed);
}

void ADC_resetCounter() {
	atomic_store(&options.sampleCount, 0);
}

uint32_t ADC_getCounter() {
	return atomic_load(&options.sampleCount);
}

void ADC_enableSampling() {
	atomic_store(&options.samplingEnabled, true);
}

void ADC_disableSampling() {
	atomic_store(&options.samplingEnabled, false);
}

bool ADC_samplingEnabled() {
	return atomic_load(&options.samplingEnabled);
}


#endif /* LIB_ADCOPTIONS_H_ */
