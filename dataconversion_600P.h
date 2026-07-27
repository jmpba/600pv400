// dataconversion_600P.h

#ifndef _DATACONVERSION_600P_h
#define _DATACONVERSION_600P_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
	#include "main.h"
#else
	#include "WProgram.h"
#endif

class DataconversionClass
{
 protected:


 public:

	void init();
	uint8_t bcd2bin(uint8_t val);
	uint8_t bin2bcd(uint8_t val);
	uint16_t highWord(uint32_t ww);
	uint16_t lowWord(uint32_t ww);
	float float32_from_four_uint8(uint8_t MSSB_uint, uint8_t MSB_uint, uint16_t LSSB_uint, uint16_t LSB_uint);
	uint32_t uint32_t_from_four_uint8(uint8_t MSSB_uint, uint8_t MSB_uint, uint16_t LSSB_uint, uint16_t LSB_uint);
	float float32_from_two_uint16(uint16_t MSB_uint, uint16_t LSB_uint);
	uint32_t uint32t_from_two_uint16_t(uint16_t MSB_uint, uint16_t LSB_uint);
	uint16_t pressureconvert(float fval);
};

extern DataconversionClass dataconversion;

#endif

