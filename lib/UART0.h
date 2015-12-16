#ifndef LIB_UART0_H_
#define LIB_UART0_H_

#include <ti/drivers/UART.h>
#include "Buffer.h"

UART_Handle uart0;

void UART0_setup() {

    UART_Params params;
    UART_Params_init(&params);
    params.writeDataMode = UART_DATA_BINARY;
    params.readDataMode = UART_DATA_BINARY;
    params.readReturnMode = UART_RETURN_FULL;
    params.readEcho = UART_ECHO_OFF;
    params.baudRate = 115200;

    uart0 = UART_open(Board_UART0, &params);
    if (uart0 == NULL) {
        System_abort("Fatal! Could not open UART0");
    }
}

void UART0_write(void* buf, size_t size) {
	UART_write(uart0, buf, size);
}

void UART0_read(void* buf, size_t size) {
	UART_read(uart0, buf, size);
}

void UART0_writeFrame(Buffer* buf) {
	char startOfPacket = '!';
	UART0_write(&startOfPacket, 1);
	UART0_write(&buf->size, 1);
	UART0_write(buf->data, buf->size);
}

#endif /* LIB_UART0_H_ */
