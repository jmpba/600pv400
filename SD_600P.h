// SD_600P.h

#ifndef _SD_600P_h
#define _SD_600P_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
	#include "main.h"
	#include <SD.h>
#else
	#include "WProgram.h"
#endif

class SDcardClass
{
 protected:
	// const char* filename = "LOGFILE.txt";
	 char filename[16];

 public:
	void init();
	void logEVENT(const char* data);
};

extern SDcardClass SDcard;

#endif

