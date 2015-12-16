#ifndef LIB_VALVES_H_
#define LIB_VALVES_H_

#include "driverlib.h"

// TODO: GIMME DECENT NAMES!

enum {
	VALVE_A = 0,
	VALVE_B = 1,
	VALVE_C = 2,
	VALVE_D = 3,
	VALVE_E = 4
};

// 5.6, 5.7, 6.6, 1.6 - VALVES

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

void Valves_output(uint8_t valve, uint8_t value) {
	switch (valve) {
		case VALVE_A:
			if (value) GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN6);
			else GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN6);
			break;

		case VALVE_B:
			if (value) GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN7);
			else GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN7);
			break;

		case VALVE_C:
			if (value) GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN6);
			else GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN6);
			break;

		case VALVE_D:
			if (value) GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN6);
			else GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);
			break;

		case VALVE_E:
			if (value) GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN7);
			else GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN7);
			break;

		default:
			break;
	}
}

#endif /* LIB_VALVES_H_ */
