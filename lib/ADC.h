#ifndef LIB_ADC_H_
#define LIB_ADC_H_

#include <stdatomic.h>

#include "Clock.h"
#include "ADCOptions.h"
#include "ADCSample.h"

void ADC_setup() {

    /*
     * >> Sampling Frequency = MCLK / SCALER / PULSE_WIDTH / MULTISEQUENCE =~ 97.66 Hz
     *    
     *    MCLK = 48 MHz
     *    SCALER = 64 * 8    (predivider * divider)
     *    PULSE_WIDTH = 192
     *    MULTISEQUENCE = 5  (number of pins to sample)
     *
     * >> Sampling Time =~ 10.24 ms
     */

	ADC_Sample_setup();
	ADC_Options_setup();

    ADC14_enableModule();
    ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_64, ADC_DIVIDER_8, ADC_NOROUTE);

    /*
     * A11 -> P4.2
     * A12 -> P4.1
     * A13 -> P4.0
     * A14 -> P6.1
     * A15 -> P6.0
     */

    // Automatically sample from A11 to A15, results will be stored in ADC_MEM11 to 15. 
    ADC14_configureMultiSequenceMode(ADC_MEM11, ADC_MEM15, true);
    ADC14_configureConversionMemory(ADC_MEM11, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A11, false);
    ADC14_configureConversionMemory(ADC_MEM12, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A12, false);
    ADC14_configureConversionMemory(ADC_MEM13, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A13, false);
    ADC14_configureConversionMemory(ADC_MEM14, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A14, false);
    ADC14_configureConversionMemory(ADC_MEM15, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A15, false);

    // Configure pins to ADC mode
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4,
    	GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2, GPIO_TERTIARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
        GPIO_PIN0 | GPIO_PIN1, GPIO_TERTIARY_MODULE_FUNCTION);

    // Hold the sample 192 times to improve signal stability
    ADC14_setSampleHoldTime(ADC_PULSE_WIDTH_192, ADC_PULSE_WIDTH_192);

    // Automatic start new conversion when one is done
    ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);
    ADC14_enableConversion();
    ADC14_toggleConversionTrigger();

    // Enabling Interrupts 
    // Interrupt should be fired after last sample is taken (A15)
    ADC14_enableInterrupt(ADC_INT15);
    // Enable global ADC14 interrupt
    Interrupt_enableInterrupt(INT_ADC14);
}

#endif /* LIB_ADC_H_ */
