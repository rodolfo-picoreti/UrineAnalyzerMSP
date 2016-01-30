# System Overview

![System Overview](https://github.com/rodolfo-picoreti/UrineAnalyzerMSP/blob/master/images/urine-analyzer.png)

The system was designed using [TI-RTOS](http://processors.wiki.ti.com/index.php/TI-RTOS) a real-time operating system for TI microcontrollers. By using TI-RTOS we gained development speed and made the code more portable by eliminating the need to write system specific code. The system contains five [threads of execution](https://en.wikipedia.org/wiki/Thread_(computing)):

* UART Reader Task
* UART Writer Task
* Heater Control Task
* ADC Hardware Interrupt
* Clock Software Interrupt

### UART Reader Task 

This thread will read data from the UART and store it on a dynamic allocated buffer using TI-RTOS HeapBuf. HeapBuf is used for allocating fixed-size blocks of memory, and is designed to be fast and deterministic. The buffer is then parsed to see if it a valid service packet. If the packet describes a valid service call the thread will execute the appropriated service code. A response packet containing the result of the service call will be written using the same buffer and then a pointer to the buffer will be sent to be "UART Writer Task" using a mailbox. Mailboxes are useful because they can be used to ensure that the flow of incoming buffers does not exceed the ability of the system to process those buffers.

#### Services - Communication Protocol

The communication between the MSP and the application running on the Raspberry Pi uses the following protocol:

![Packet](https://github.com/rodolfo-picoreti/UrineAnalyzerMSP/blob/master/images/packet.png)

The first byte on the payload of a response packet should contain the status of the execution of the previously made request. The possible status values are listed below and can be found at **ServiceStatus.h**

![Status Values](https://github.com/rodolfo-picoreti/UrineAnalyzerMSP/blob/master/images/service-types.png)

### UART Writer Task

This thread will block waiting for the mailboxes to contain any data. 

* Buffer Mailbox: incoming buffers posted by the "UART Reader Task" containing MSP service response packets  
* Sample Mailbox: samples posted by the "Clock Software Interrupt" to be sent to the RPI as a service call

If one of the mailboxes contain data this thread will read it and then write the appropriated data to the UART. Note that this task in responsible for deallocating the buffer after the data has been written.

### Heater Control Task

This is the highest priority task in the system. It will periodically read the temperature sensors of the sensor box (using [atomic variables](https://en.wikipedia.org/wiki/Linearizability) to avoid race conditions with the ADC Hardware Interrupt) and compute the appropriated PWM duty cycle to be sent to the heaters in order to keep them at 37 degrees Celsius. The implemented controller uses a simple proportional integral controller with saturations.

### ADC Hardware Interrupt

Highest priority thread on the system. Periodically read all the conversion memory registers that hold the result of the analog to digital conversion and store them into atomic variables to avoid race conditions with reader threads. More details about how to change the sampling time on [ADC.h](https://github.com/rodolfo-picoreti/UrineAnalyzerMSP/blob/master/lib/ADC.h)


### Clock Software Interrupt

This thread will periodically read the sensors values and send to the RPI if the sampling service is enable. The sampling is enabled/disabled by the RPI by sending a service request to the MSP.

# Important Links
* [Video training on how to use TI-RTOS](https://training.ti.com/ti-rtos-workshop-series)
* [TI-RTOS Kernel Guide](http://www.ti.com/lit/ug/spruex3p/spruex3p.pdf)
* [TI-RTOS Drivers Guide](http://www.ti.com/lit/ug/spruhd4k/spruhd4k.pdf)
* [MSP432 Datasheet](http://www.ti.com/lit/ds/slas826a/slas826a.pdf)
