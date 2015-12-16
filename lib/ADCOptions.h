#ifndef LIB_ADCOPTIONS_H_
#define LIB_ADCOPTIONS_H_

typedef struct {
	atomic_bool samplingEnabled;
	atomic_uint_least16_t samplingTime; // seconds
	atomic_uint_least32_t sampleCount;
} ADC_Options;

static ADC_Options options;

void ADC_Options_setup() {
	atomic_store(&options.samplingEnabled, false);
}

void ADC_setSamplingTime(uint16_t ts) {
	atomic_store(&options.samplingTime, ts);
}

uint16_t ADC_getSamplingTime() {
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
