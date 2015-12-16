/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/heaps/HeapBuf.h>
#include <ti/sysbios/heaps/HeapMem.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/PWM.h>

/* Board Header file */
#include "Board.h"

/* Others */
#include "lib/UART0.h"
#include "lib/Clock.h"
#include "lib/ADC.h"
#include "lib/Sync.h"
#include "lib/PWM.h"
#include "lib/RemoteParsers.h"
#include "lib/RemoteClientServices.h"
#include "lib/Valves.h"
#include "lib/PID.h"

Void Clock_swi(UArg arg0) {
	uint64_t microseconds = Clock_increment();

	if (!(microseconds % 1000)) {
		GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN0);
	}

	if (ADC_samplingEnabled() && !(microseconds % ADC_getSamplingTime())) {
		ADC_Sample sample;
		ADC_Sample_initialize(&sample);
		sample.counter = ADC_getCounter();
		sample.timestamp = microseconds;
		Sync_postSample(&sample);
		ADC_incrementCounter();
	}
}

float smooth(uint16_t current, uint16_t past) {
	return 0.1*current + 0.9*past;
}

Void ADC_hwi() {
	/*
	 * A11 -> P4.2 -> SD2
	 * A12 -> P4.1 -> Heater
	 * A13 -> P4.0 -> SD1
	 * A14 -> P6.1 -> Box
	 * A15 -> P6.0 -> Pre heater
	 */
	uint64_t status = ADC14_getEnabledInterruptStatus();
	ADC14_clearInterruptFlag(status);

	if (ADC_INT15 & status) {
		ADC_Sensor_Id id = ADC_getCurrentSensorId();
		ADC_storeReading(id, smooth(ADC14MEM14, ADC_loadReading(id)));

		id = SENSOR_PREHEATER_TEMP;
		ADC_storeReading(id, smooth(ADC14MEM15, ADC_loadReading(id)));

		id = SENSOR_INTHEATER_TEMP;
		ADC_storeReading(id, smooth(ADC14MEM12, ADC_loadReading(id)));

		id = SENSOR_SD1;
		ADC_storeReading(id, smooth(ADC14MEM13, ADC_loadReading(id)));

		id = SENSOR_SD2;
		ADC_storeReading(id, smooth(ADC14MEM11, ADC_loadReading(id)));

		ADC_selectNextSensor();
	}
}

Void Task_heaterPID(UArg arg0, UArg arg1) {
	PID preheaterPID = {
		.outMax = 301.0,
		.outMin = 0.0,
		.setpoint = 0.0
	};

	PID intheaterPID = {
		.outMax = 301.0,
		.outMin = 0.0,
		.setpoint = 7500
	};

	PID_initialize(&preheaterPID, 10, 0, 0);
	PID_initialize(&intheaterPID, 10, 0, 0);
	PID_setup();

	while (1) {
		if (PID_getMode()) {
			/*
			uint16_t preTemp = ADC_loadReading(SENSOR_PREHEATER_TEMP);
			float preOut = PID_compute(&preheaterPID, preTemp);
			PWM_setDutyCycle(PWM_PREHEATER, preOut);

			uint16_t intTemp = ADC_loadReading(SENSOR_INTHEATER_TEMP);
			float intOut = PID_compute(&intheaterPID, intTemp);
			PWM_setDutyCycle(PWM_INTHEATER, intOut);
			*/

			float error = 7500 - ADC_loadReading(SENSOR_INTHEATER_TEMP);
			float u = error*10;
			if (u > 301) u = 301;
			else if (u < 0) u = 0;

			PWM_setDutyCycle(PWM_INTHEATER, u);
		}
		Task_sleep(5000);
	}
}

Void Task_uartReader(UArg arg0, UArg arg1) {

	Buffer buf = {
		.data = NULL,
		.size = 0
	};

	while (1) {
		/*
		 * Wait until a memory block is available for use.
		 * Although this is not supposed to happen.
		 */
		while (buf.data == NULL) {
			Error_Block error;

			IHeap_Handle heap = HeapBuf_Handle_upCast(heapBuf);
			buf.data = Memory_alloc(heap, 64, 0, &error);

			if (buf.data == NULL) {
				System_printf("Reader unable to allocate memory!");
				System_flush();
			}
		}

		/*
		 * Read uart until the start of packet byte ('#')
		 * and a valid size is passed ( > 0 and <= 64 )
		 */
		do {
			UART0_read(buf.data, 1);

			if (buf.data[0] == '!') {
				UART0_read(&buf.size, 1); // get payload size
			}
		} while (buf.size == 0 || buf.size > 64);

		UART0_read(buf.data, buf.size); // get payload
		Remote_parse(&buf);
	}
}

Void Task_uartWriter(UArg arg0, UArg arg1) {
	char sampleBuffer[32];
	Buffer message;

	while (1) {
		UInt events = Sync_pendEvent();

		// Check if the event is a sample event
		if (Sync_sampleEvent(&events)) {
			ADC_Sample sample;
			Sync_getSample(&sample);
			message.data = sampleBuffer;
			Service_Request_newADCSample(&message, &sample);
			UART0_writeFrame(&message);
		}

		if (Sync_bufferEvent(&events)) {
			Sync_getBuffer(&message);
			UART0_writeFrame(&message);
			IHeap_Handle heap = HeapBuf_Handle_upCast(heapBuf);
			Memory_free(heap, message.data, 64);
		}
	}
}


int main(void) {
    Board_initGeneral();
    Board_initGPIO();
    Board_initUART();

    FPU_enableModule();
    FPU_enableLazyStacking();

    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);

    PWM_setup();
    ADC_setup();
    UART0_setup();
	Clock_setup();
	Sync_setup();
	Valves_setup();
	Interrupt_enableMaster();

    /* Start BIOS */
    BIOS_start();
    return (0);
}
