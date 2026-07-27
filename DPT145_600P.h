// 600P_DPT145.h

#ifndef _DPT145_600P_h
#define _DPT145_600P_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
	#include "main.h"
#else
	#include "WProgram.h"
#endif

extern uint32_t HardwareStatus;

class DPT145Class
{
 protected:


 public:

	 float T = 0;
	 float Tdf = 0;
	 float Tdfa = 0;
	 uint16_t H2O = 0;
	 uint16_t P = 0;
	 float Rhoo = 0;
	 uint16_t Pnorm = 0;
	 uint16_t faultstatus = 0;
	 uint16_t onlinestatus = 0;
	 uint32_t errorcode = 0;
	 uint16_t autopurge = 0;
	 uint16_t startuppurge = 0;
	 uint16_t purgestatus = 0;
	 char serialNO[9] = { '\0' };
	 uint8_t slaveID = 0;

	void init();

};

extern DPT145Class sensor1;
extern DPT145Class sensor2;
extern DPT145Class sensor3;

class sensorClass
{
protected:

	#define BETWEENPOLLTIME 750

	uint8_t pollSequence = 0;
	uint32_t polltimeout = 0;

public:


	void init();
	void read();
};

extern sensorClass sensor;

#endif

