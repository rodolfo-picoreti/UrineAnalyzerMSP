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

/* Urine Analyzer Header files */
#include "lib/UART0.h"
#include "lib/Clock.h"
#include "lib/ADC.h"
#include "lib/Sync.h"
#include "lib/PWM.h"
#include "lib/ServiceParsers.h"
#include "lib/ServiceRpiMethods.h"
#include "lib/Valves.h"
#include "lib/Controller.h"
#include "lib/ServiceLock.h"

Void Clock_swi(UArg arg0) {
	// Clock debug led (every second)
	GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
	Clock_increment();

	static uint32_t lastSample = 0;
	uint32_t seconds = Clock_seconds();
	if (ADC_samplingEnabled()) {
		if (seconds - lastSample >= ADC_getSamplingTime()) {
			lastSample = seconds;
			Swi_post(adcSwi);
		}
	}
}

Void ADC_swi(UArg arg0, UArg arg1) {
	// Compose sample packet
	ADC_Sample sample;
	ADC_Sample_initialize(&sample);
	ADC_incrementCounter();

	// Send it to Task_uartWriter
	Sync_postSample(&sample);
}

Void ADC_hwi() {
	/*
	 * A11 -> P4.2 -> SD2
	 * A12 -> P4.1 -> Pre Heater
	 * A13 -> P4.0 -> SD1
	 * A14 -> P6.1 -> Box
	 * A15 -> P6.0 -> heater
	 */
	uint64_t status = ADC14_getEnabledInterruptStatus();
	ADC14_clearInterruptFlag(status);

	if (ADC_INT15 & status) {
		ADC_storeSmoothedReading(ADC_getCurrentSensorId(), ADC14MEM14);
		ADC_storeSmoothedReading(SENSOR_PREHEATER_TEMP, ADC14MEM12);
		ADC_storeSmoothedReading(SENSOR_INTHEATER_TEMP, ADC14MEM15);
		ADC_storeSmoothedReading(SENSOR_SD1, ADC14MEM13);
		ADC_storeSmoothedReading(SENSOR_SD2, ADC14MEM11);

		// If user has the lock (is manually selecting the sensor) than do nothing
		if (Service_isLocked(SERVICE_LOCK_SENSOR_SELECT)) {
			ADC_selectNextSensor();
		}
	}
}

/*
 * Control Heaters Temperature
 */
Void Task_heaterControl(UArg arg0, UArg arg1) {
	// Controller implements a PI controller

	Controller preheater;
	Controller heater;
	Controller_initialize(&preheater, 6900.0f);
	Controller_initialize(&heater, 7300.0f);

	float temperature;
	float output;

	while (1) {
		// Check if service is locked (user manually controlling the heaters)
		if (Service_isLocked(SERVICE_LOCK_HEATERS)) {
			temperature = ADC_loadReading(SENSOR_PREHEATER_TEMP);
			output = Controller_compute(&preheater, temperature);
			PWM_setDutyCycle(PWM_PREHEATER, output);

			temperature = ADC_loadReading(SENSOR_INTHEATER_TEMP);
			output = Controller_compute(&heater, temperature);
			PWM_setDutyCycle(PWM_INTHEATER, output);
		}

		Task_sleep(5000);
	}
}

/*
 * Read and parse RPI Services Requests from the UART:
 * 		Read -> Parse -> Execute Service -> Compose Response (Buffer) -> Send Buffer to uartWriter
 */
Void Task_uartReader(UArg arg0, UArg arg1) {

	Buffer buf = {
		.data = NULL,
		.size = 0
	};

	while (1) {
		/*
		 * Wait until a memory block is available for use.
		 * Although this should NEVER happen.
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
		Service_parse(&buf);
	}
}

/*
 * Transmit RPI Service Requests and MSP Service responses
 */
Void Task_uartWriter(UArg arg0, UArg arg1) {
	char sampleBuffer[32];
	Buffer message;

	while (1) {
		UInt events = Sync_pendEvent();

		// Check if the event is a sample event
		if (Sync_sampleEvent(&events)) {
			ADC_Sample sample;
			Sync_getSample(&sample); // acquire sample object
			
			// compose buffer with a request to the RPI
			message.data = sampleBuffer;
			Service_Rpi_storeSample(&message, &sample);

			UART0_writeFrame(&message); // transmit buffer
		}

		// Check if there is a new buffer (service packet) to be sent
		if (Sync_bufferEvent(&events)) {
			Sync_getBuffer(&message); // acquire buffer
			UART0_writeFrame(&message); // transmit buffer

			// freeing the buffer is our responsability
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

    // Debug leds setup
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);

    PWM_setup();
    ADC_setup();
    UART0_setup();
	Clock_setup();
	Sync_setup();
	Valves_setup();
	Service_Lock_setup();

	Interrupt_enableMaster();
    BIOS_start();
    return (0);
}
