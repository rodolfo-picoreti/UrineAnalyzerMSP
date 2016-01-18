#ifndef LIB_CONTROLLER_H_
#define LIB_CONTROLLER_H_

#include "ServiceLock.h"
#include "PWM.h"

// Heaters PI Controller
typedef struct {
	float setpoint;
	float integral;
} Controller;

void Controller_initialize(Controller* C, float setpoint) { 
	C->integral = 0.0f;
	C->setpoint = setpoint;
}

void saturate(float* value, float min, float max) {
	if (*value > max) *value = max;
	else if (*value < min) *value = min;
}

/*
 * Output = Kp*error + Ki*integral(error)
 * 	Kp = 0.5
 * 	Ki = 0.01
 */
float Controller_compute(Controller* C, float input) {
	float error = C->setpoint - input;
	if (error > 1000.0f) return PWM_dutyCycleMax();

	const float Kp = 0.5f;
	const float Ki = 0.01f;
	
	C->integral += Ki*error;
	saturate(&C->integral, 0.0f, 200.0f);

	float output = Kp*error + C->integral;
	saturate(&output, 0.0f, PWM_dutyCycleMax());

    return output;
}


#endif /* LIB_C_H_ */
