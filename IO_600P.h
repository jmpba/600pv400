// IO_600P.h

#ifndef _IO_600P_h
#define _IO_600P_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
	#include "main.h"
#else
	#include "WProgram.h"
#endif

extern uint32_t faultBITS;
extern uint8_t debugEN;

#define SENSORpowertime 8000

class IOClass
{
 protected:

	 uint8_t FAULTretrySENSOR1 = 0;
	 uint8_t FAULTretrySENSOR2 = 0;
	 uint8_t FAULTretrySENSOR3 = 0;
	 uint32_t PWRcycletime = SENSORpowertime;
	 uint32_t powercycletimer = 0;


 public:

	 uint8_t SDinserted = 1;
	 uint8_t numSensors = 3;
	 uint8_t PWRstage = 0;

	 typedef enum {

		 OFF,
		 SENSOR1,
		 SENSOR2,
		 SENSOR3,
		 TIMEADJUST,
		 COMPLETE

	 } DPTpowerstage;

	void init();
	void read();

};

extern IOClass IO;

#endif

