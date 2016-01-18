#ifndef LIB_SYNC_H_
#define LIB_SYNC_H_

#include <xdc/runtime/Error.h>

#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Mailbox.h>

#include "ADCSample.h"

// Send Samples collected on the ADC HWI to the UART Writer Task.
Mailbox_Handle Mailbox_samples;

// Send the result of Remote Calls (service responses) to the UART Writer Task.
Mailbox_Handle Mailbox_buffers;

// Synchorize read of the Sampling and Remote mailboxes on the UART Writer Task.
Event_Handle Event_uart;

void Sync_postSample(ADC_Sample* sample) {
	if (!Mailbox_post(Mailbox_samples, sample, BIOS_NO_WAIT)) {
		// Mailbox is full! This should NEVER happen!
		System_abort("Mailbox is full!");
	}
}

void Sync_postBuffer(Buffer* buffer) {
	Mailbox_post(Mailbox_buffers, buffer, BIOS_WAIT_FOREVER);
}

UInt Sync_pendEvent() {
	return Event_pend(Event_uart, Event_Id_NONE, Event_Id_00 + Event_Id_01, BIOS_WAIT_FOREVER);
}

bool Sync_sampleEvent(UInt* event) {
	return Event_Id_00 & *event;
}

bool Sync_bufferEvent(UInt* event) {
	return Event_Id_01 & *event;
}

void Sync_getSample(ADC_Sample* sample) {
	Mailbox_pend(Mailbox_samples, sample, BIOS_NO_WAIT);
}

void Sync_getBuffer(Buffer* buffer) {
	Mailbox_pend(Mailbox_buffers, buffer, BIOS_NO_WAIT);
}

// Initialize all synchronization mechanisms
void Sync_setup() {
	atomic_uint_least32_t atomic;

	// Check if memory access to 32 bit is atomic
	if (!atomic_is_lock_free(&atomic)) {
    	System_abort("Fatal! uint32_t is not lock free");
	}

	Error_Block eb;
	Error_init(&eb);

	Event_uart = Event_create(NULL, &eb);
	if (Event_uart == NULL) {
		System_abort("Fatal! Uart Event create failed");
	}

	Mailbox_Params mailboxParams;
	Mailbox_Params_init(&mailboxParams);

	mailboxParams.readerEvent = Event_uart;
	mailboxParams.readerEventId = Event_Id_00;

	Mailbox_samples = Mailbox_create(sizeof(ADC_Sample), 16, &mailboxParams, &eb);
	if (Mailbox_samples == NULL) {
		System_abort("Fatal! Could not initialize samples mailbox");
	}

	mailboxParams.readerEventId = Event_Id_01;

	Mailbox_buffers = Mailbox_create(sizeof(Buffer), 4, &mailboxParams, &eb);
	if (Mailbox_buffers == NULL) {
		System_abort("Fatal! Could not initialize buffers mailbox");
	}
}

#endif /* LIB_SYNC_H_ */
