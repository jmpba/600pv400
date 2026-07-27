// serialcommands_600P.h

#ifndef _SERIALCOMMANDS_600P_h
#define _SERIALCOMMANDS_600P_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
	#include "main.h"
#else
	#include "WProgram.h"
#endif

#define DEBUG SerialUSB
extern uint8_t currentflashbank;
extern uint8_t debugEN;
extern uint32_t PCBserialno;
extern uint32_t lastresetcause;
extern float PCBtemp;
extern uint32_t looptime;
extern uint32_t totaluptimeSECONDS;

class serialcommandsClass
{
 protected:
	 char DEBUGbuffer[128];
	 uint8_t count = 0;
	 uint32_t DEBUGtimer = 0;
	 bool validCommand = false;
	 const char* STARLINE = "****************************************************";

	 //************THESE COMMANDS ARE RECOGNISED WHEN TYPED INTO THE SERIAL MONITOR******************************************************

	 const char* basicPARA = "?\r";
	 const char* advancedPARA = "??\r";
	 const char* sensorVALUES = "???\r";
	 const char* debugEnablecommand = "DEBUG ON\r";
	 const char* debugDisablecommand = "DEBUG OFF\r";
	 const char* RS485ON = "RS485 ON\r";
	 const char* RS485OFF = "RS485 OFF\r";

 public:
	void init();
	void scan();
};

extern serialcommandsClass serialcommands;



#endif

