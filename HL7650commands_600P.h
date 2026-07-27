// HL7650commands_600P.h

#ifndef _HL7650COMMANDS_600P_h
#define _HL7650COMMANDS_600P_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
	#include "main.h"
#else
	#include "WProgram.h"
#endif

#define MODEMRESET 0
#define ATK3 1
#define ATE0 2
#define ATKSREP 3 
#define ATKSREPmodify 4 
#define ATCREG 5
#define ATCREGmodify 6
#define ATW0 7
#define ATWMANTSEL 10
#define ATWMANTSEL_modify 11
#define ATCOPS 20
#define ATCOPSauto 21
#define ATCOPSderegister 22
#define ATCPIN 30
#define ATCMEE 40
#define ATCMEEmodify 41
#define ATCGDCONT 50
#define ATCGDCONTmodify 51
#define ATCGDCONTdelete 52
#define ATWPPP 60
#define ATCGACT 70
#define ATCGACTactivate 71
#define ATCGACTdeactivate 72
#define ATKCNXCFG 100
#define ATKCNXCFGmodify 101
#define ATKSLEEP 110
#define ATKSLEEPmodify 111
#define ATCTZU 120
#define ATCTZUmodify 121
#define ATCTZR 130
#define ATCTZRmodify 131
#define ATCCLK 140
#define ATKCNXTIMER 150
#define ATKCNXTIMERmodify 151
#define ATKTCPCFG 160
#define ATKTCPCFGmodify 161
#define ATKTCPCFGwait 162
#define ATKTCPCNX 170

#define MQTTconnect 180
#define MQTTconnectSEND 181
#define MQTTconnectRECV 182

#define ATKCGPADDR 230
#define ATCSQ 240
#define ATKBND 250
#define MQTTpublish 260
#define MQTTpublishSEND 270
#define MQTTpublishRECV 280
#define MQTTpublishRESULT 290

#define MQTTdisconnect 300
#define MQTTdisconnectSEND 310
#define MQTTdisconnectRECV 320

#define TCPdisconnect 800
#define TCPsocketCLOSE 810
#define TCPdelete 820

#define POWERoff 900

#define CMEERROR 1000

class HL7650commandClass
{
 protected:

	 uint16_t msglen = 0;
	 

 public:

	void init();
	void process(void);
	void printMQTTresponse(void);
	uint8_t sendcmd(const char* cmd);
	void senddata(uint8_t* sendingbuffer, uint16_t message_len);
	void complete(uint16_t nextstep);
};

extern HL7650commandClass HL7650command;

#endif

