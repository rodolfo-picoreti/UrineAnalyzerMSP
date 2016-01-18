#ifndef LIB_CLOCK_H_
#define LIB_CLOCK_H_

#include <xdc/runtime/Error.h>
#include <ti/sysbios/knl/Clock.h>

#include <stdatomic.h>
#include "driverlib.h"

// Count how many seconds passed since system startup
static atomic_uint_least32_t seconds;

void Clock_increment() {
	atomic_fetch_add_explicit(&seconds, 1, memory_order_relaxed);
}

uint32_t Clock_seconds() {
	return atomic_load(&seconds);
}

Void Clock_swi(UArg arg0);

/*
 * Create clock software interrupt other configuration parameters 
 * can be found on the RTOS .cfg file
 */
void Clock_setup() {
    Error_Block eb;
    Error_init(&eb);

    /* 
     * Clock tick = 1 millisecond 
     *	 
     *	!! This is defined on the .cfg file as "Clock.tickPeriod"
     */

	Clock_Params clkParams;
	Clock_Params_init(&clkParams);
	clkParams.period = 1000; // Call SWI every 1000 tick, therefore every second
	clkParams.startFlag = true;
	Clock_create(Clock_swi, 1, &clkParams, &eb);
}

#endif /* LIB_CLOCK_H_ */
