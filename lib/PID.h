#ifndef LIB_PID_H_
#define LIB_PID_H_

/*
typedef struct {
	float setpoint;
	float error[3]; // past 3 error values
	float K[3]; // controller gain
	float out;
} PID;


void PID_initialize(PID* pid, float kp, float ki, float kd) {
	pid->K[0] = kp + ki + kd;
	pid->K[1] = -kp -2*kd;
	pid->K[2] = kd;

	pid->out = 0;

	int i;
	for (i = 0; i < 3; ++i) {
		pid->error[i] = 0;
	}
}

float PID_compute(PID* pid, float input) {

	pid->error[2] = pid->error[1];
	pid->error[1] = pid->error[0];
    pid->error[0] = pid->setpoint - input;

    int i;
    for (i = 0; i < 3; ++i) {
    	pid->out += pid->K[i]*pid->error[i];
	}

    // saturate output
    if (pid->out > pid->outMax) pid->out = pid->outMax;
    else if (pid->out < pid->outMin) pid->out = pid->outMin;

    return pid->out;
}


*/
#endif /* LIB_PID_H_ */
