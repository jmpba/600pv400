// MCP98244_600P.h

#ifndef _MCP98244_600P_h
#define _MCP98244_600P_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
	#include "main.h"
#else
	#include "WProgram.h"
#endif

class MCP98244Class
{
 protected:


 public:

	float PCBtemp;
	float PCBtemp_1;

	void init();
	float readtemp();
};

extern MCP98244Class MCP98244;

#endif

