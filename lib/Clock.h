#ifndef LIB_CLOCK_H_
#define LIB_CLOCK_H_

#include <xdc/runtime/Error.h>
#include <ti/sysbios/knl/Clock.h>

#include <stdatomic.h>
#include "driverlib.h"

static uint64_t milliseconds;

Void Clock_swi(UArg arg0);

uint64_t Clock_increment() {
	++milliseconds;
	return milliseconds;
}
uint64_t Clock_milliseconds() {
	return milliseconds;
}

void Clock_setup() {

	GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);

    Error_Block eb;
    Error_init(&eb);

    // Clock tick = 1s
	Clock_Params clkParams;
	Clock_Params_init(&clkParams);
	clkParams.period = 1; // Call swi every tick
	clkParams.startFlag = true;
	Clock_create(Clock_swi, 1, &clkParams, &eb);
}

#endif /* LIB_CLOCK_H_ */
