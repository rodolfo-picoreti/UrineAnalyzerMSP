
# Services - Packet

The communication between the MSP and the application running on the Raspberry Pi uses the following protocol:

[[https://github.com/rodolfo-picoreti/UrineAnalyzerMSP/blob/master/images/packet.png|alt=octocat]]

The first byte on the payload of a response packet should contain the status of the execution of the previously made request. The possible status values are listed below and can be found at **ServiceStatus.h**

[[https://github.com/rodolfo-picoreti/UrineAnalyzerMSP/blob/master/images/service-types.png|alt=octocat]]

# System Overview

[[https://github.com/rodolfo-picoreti/UrineAnalyzerMSP/blob/master/images/urine-analyzer.png|alt=octocat]]
