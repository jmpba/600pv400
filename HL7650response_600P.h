// HL7650response_600P.h

#ifndef _HL7650RESPONSE_600P_h
#define _HL7650RESPONSE_600P_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
	#include "main.h"

#else
	#include "WProgram.h"
#endif

#define recieve_buffer_size 384

class HL7650responseClass
{
 protected:

	 char PROCESSING_buffer[recieve_buffer_size] = { '\0' };

 public:

	 

	void init();
	void ProcessModemResponse();
	void printTCPerror(uint8_t tcp_notif);
	void printnetworkstatus();
	void printsocketstatus(uint8_t statussocket);
	void printstartupURCstatus(uint8_t URCstat);


};

extern HL7650responseClass HL7650response;

#endif

