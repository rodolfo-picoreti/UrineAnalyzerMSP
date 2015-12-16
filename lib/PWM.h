#ifndef LIB_PWM_H_
#define LIB_PWM_H_

#include "driverlib.h"

Timer_A_PWMConfig PWM_pinConfig_2_7 = {
	.clockSource = TIMER_A_CLOCKSOURCE_SMCLK,      	      // 48MHz
	.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_16, // 3MHz
	.timerPeriod = 300,		                              // 100us - 10kHz
	.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_4, // CCR4 of TIMER_A0 is pin 2.7
	.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET,
	.dutyCycle = 0
};

Timer_A_PWMConfig PWM_pinConfig_2_6 = {
	.clockSource = TIMER_A_CLOCKSOURCE_SMCLK,      	      // 48MHz
	.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_16, // 3MHz
	.timerPeriod = 300,		                              // 100us - 10kHz
	.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_3, // CCR3 of TIMER_A0 is pin 2.6
	.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET,
	.dutyCycle = 0
};

Timer_A_PWMConfig PWM_pinConfig_2_5 = {
	.clockSource = TIMER_A_CLOCKSOURCE_SMCLK,      	      // 48MHz
	.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_16, // 3MHz
	.timerPeriod = 300,		                              // 100us - 10kHz
	.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2, // CCR3 of TIMER_A0 is pin 2.5
	.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET,
	.dutyCycle = 0
};


Timer_A_PWMConfig PWM_pinConfig_2_4 = {
	.clockSource = TIMER_A_CLOCKSOURCE_SMCLK,      	      // 48MHz
	.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_16, // 3MHz
	.timerPeriod = 300,		                              // 100us - 10kHz
	.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1, // CCR3 of TIMER_A0 is pin 2.4
	.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET,
	.dutyCycle = 0
};

enum {
	PWM_REAGENT_PUMP = 0,
	PWM_SAMPLE_WASTE_PUMP = 1,
	PWM_PREHEATER = 2,
	PWM_INTHEATER = 3
};

void PWM_setDutyCycle(uint8_t pin, uint32_t value) {
	switch (pin) {
		case PWM_REAGENT_PUMP:
			PWM_pinConfig_2_5.dutyCycle = value;
			MAP_Timer_A_generatePWM(TIMER_A0_MODULE, &PWM_pinConfig_2_5);
			break;

		case PWM_SAMPLE_WASTE_PUMP:
			PWM_pinConfig_2_7.dutyCycle = value;
			MAP_Timer_A_generatePWM(TIMER_A0_MODULE, &PWM_pinConfig_2_7);
			break;

		case PWM_PREHEATER:
			PWM_pinConfig_2_6.dutyCycle = value;
			MAP_Timer_A_generatePWM(TIMER_A0_MODULE, &PWM_pinConfig_2_6);
			break;

		case PWM_INTHEATER:
			PWM_pinConfig_2_4.dutyCycle = value;
			MAP_Timer_A_generatePWM(TIMER_A0_MODULE, &PWM_pinConfig_2_4);
			break;

		default:
			break;
	}
}

void PWM_setup() {
	MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,
			GPIO_PIN7 | GPIO_PIN6 | GPIO_PIN5 | GPIO_PIN4,
			GPIO_PRIMARY_MODULE_FUNCTION);

	MAP_Timer_A_generatePWM(TIMER_A0_MODULE, &PWM_pinConfig_2_7);
	MAP_Timer_A_generatePWM(TIMER_A0_MODULE, &PWM_pinConfig_2_6);
	MAP_Timer_A_generatePWM(TIMER_A0_MODULE, &PWM_pinConfig_2_5);
	MAP_Timer_A_generatePWM(TIMER_A0_MODULE, &PWM_pinConfig_2_4);
}

#endif /* LIB_PWM_H_ */
