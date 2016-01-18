#ifndef LIB_VALVES_H_
#define LIB_VALVES_H_

#include "driverlib.h"
#include "ServiceLock.h"

/*
 * Valve definitions to increase readability
 */

typedef enum {
	VALVE_PROBE_LOWER = 0,   // 5.6
	VALVE_PROBE_UPPER = 1,   // 5.7
	VALVE_WASH = 2,          // 6.6
	VALVE_SLOPE_REAGENT = 3, // 1.6
	VALVE_CAL_REAGENT = 4    // 6.7
} Valves_Id;

void Valves_setup() {
	GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN6);
	GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN6);

	GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN7);
	GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN7);

	GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN6);
	GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN6);

	GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN6);
	GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);

	GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN7);
	GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN7);
}

void Valves_output(Valves_Id valve, bool value) {
	switch (valve) {
		case VALVE_PROBE_LOWER:
			if (value) GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN6);
			else GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN6);
			break;

		case VALVE_PROBE_UPPER:
			if (value) GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN7);
			else GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN7);
			break;

		case VALVE_WASH:
			if (value) GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN6);
			else GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN6);
			break;

		case VALVE_SLOPE_REAGENT:
			if (value) GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN6);
			else GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);
			break;

		case VALVE_CAL_REAGENT:
			if (value) GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN7);
			else GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN7);
			break;

		default:
			break;
	}
}

#endif /* LIB_VALVES_H_ */
