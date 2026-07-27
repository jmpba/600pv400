// DS1338_600P.h

#ifndef _DS1338_600P_h
#define _DS1338_600P_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
	#include "main.h"
	//#include "dataconversion_600P.h"
#else
	#include "WProgram.h"
#endif

extern uint8_t debugEN;
extern uint32_t faultBITS;


class DS1338Class
{
 protected:

	


 public:

	uint32_t epoch;

	void init();
	uint8_t read(int16_t deviceaddress, uint16_t eeaddress);
	void write(int16_t deviceaddress, uint16_t eeaddress, uint8_t data);
	uint32_t readRTC(int16_t deviceaddress);
	uint8_t setRTC(int16_t deviceaddress, time_t time);
	void writenvRAMlong(int16_t deviceaddress, uint16_t eeaddress, uint32_t data);
	uint32_t readnvRAMlong(int16_t deviceaddress, uint16_t eeaddress);

};

extern DS1338Class DS1338;

#endif

/**
 * EERAM MEMORY BLOCKS
 *****************************************************************************************************************
 * |   DEC    |   HEX    | 1 BYTE   | 1 BYTE   | 1 BYTE   | 1 BYTE   | 1 BYTE   | 1 BYTE   | 1 BYTE   | 1 BYTE   |
 * |          |          |    7     |    6     |    5     |    4     |    3     |    2     |    1     |    0     |
 * |----------|----------|----------|----------|----------|----------|----------|----------|----------|----------|
 * | 0        | 0x00     |			|   	   |		  |			 | 			|		   |	      |		     | 
 * | 1        | 0x01     |			|		   |		  |			 | 			|		   |	      |		     |
 * | 2        | 0x02     |			|		   |		  |			 | 			|		   |	      |		     |
 * | 3        | 0x03     |			|		   |		  |			 | 			|		   |	      |		     |
 * | 4        | 0x04     |			|		   |		  |			 | 			|		   |	      |		     |
 * | 5        | 0x05     |			|		   |		  |			 | 			|		   |	      |		     |
 * | 6        | 0x06     |			|		   |		  |			 | 			|		   |	      |		     |
 * | 7        | 0x07     |			|		   |		  |			 | 			|		   |	      |		     |
 * | 8        | 0x08     |			|		   |		  |			 | 			|		   |	      |		     |
 * | 9        | 0x09     |			|		   |		  |			 | 			|		   |	      |		     |
 * | 10       | 0x0A     |			|		   |		  |			 | 			|		   |	      |		     |
 * | 11       | 0x0B     |			|		   |		  |			 | 			|		   |	      |		     |
 * | 12       | 0x0C     |          |          |          |          |          |          |          |          |
 * | 13       | 0x0D     |          |          |          |          |          |          |          |          |
 * | 14       | 0x0E     |          |          |          |          |          |          |          |          |
 * | 15       | 0x0F     |          |          |          |          |          |          |          |          |
 * | 16       | 0x10     |	TIMEZONE OFFSET                                                                      |
 * | 17       | 0x11     |	TIMEZONE OFFSET                                                                      |
 * | 18       | 0x12     |	TIMEZONE OFFSET                                                                      |
 * | 19       | 0x13     | 	TIMEZONE OFFSET                                                                      |
 * | 20       | 0x14     |  LAST SEND TIME                                                                       |
 * | 21       | 0x15     |  LAST SEND TIME                                                                       |
 * | 22       | 0x16     |  LAST SEND TIME                                                                       |
 * | 23       | 0x17     |  LAST SEND TIME                                                                       |
 * | 24       | 0x18     |          |          |          |          |          |          |          |          |
 * | 25       | 0x19     |          |          |          |          |          |          |          |          |
 * | 26       | 0x1A     |          |          |          |          |          |          |          |          |
 * | 27       | 0x1B     |          |          |          |          |          |          |          |          |
 * | 28       | 0x1C     |          |          |          |          |          |          |          |          |
 * | 29       | 0x1D     |          |          |          |          |          |          |          |          |
 * | 30       | 0x1E     |          |          |          |          |          |          |          |          |
 * | 31       | 0x1F     |          |          |          |          |          |          |          |          |
 * | 32       | 0x20     |          |          |          |          |          |          |          |          |
 * | 33       | 0x21     |          |          |          |          |          |          |          |          |
 * | 34       | 0x22     |          |          |          |          |          |          |          |          |
 * | 35       | 0x23     |          |          |          |          |          |          |          |          |
 * | 36       | 0x24     |          |          |          |          |          |          |          |          |
 * | 37       | 0x25     |          |          |          |          |          |          |          |          |
 * | 38       | 0x26     |          |          |          |          |          |          |          |          |
 * | 39       | 0x27     |          |          |          |          |          |          |          |          |
 * | 40       | 0x28     |          |          |          |          |          |          |          |          |
 * | 41       | 0x29     |          |          |          |          |          |          |          |          |
 * | 42       | 0x2A     |          |          |          |          |          |          |          |          |
 * | 43       | 0x2B     |          |          |          |          |          |          |          |          |
 * | 44       | 0x2C     |          |          |          |          |          |          |          |          |
 * | 45       | 0x2D     |          |          |          |          |          |          |          |          |
 * | 46       | 0x2E     |          |          |          |          |          |          |          |          |
 * | 47       | 0x2F     |          |          |          |          |          |          |          |          |
 * | 48       | 0x30     |          |          |          |          |          |          |          |          |
 * | 49       | 0x31     |          |          |          |          |          |          |          |          |
 * | 50       | 0x32     |          |          |          |          |          |          |          |          |
 * | 51       | 0x33     |          |          |          |          |          |          |          |          |
 * | 52       | 0x34     |          |          |          |          |          |          |          |          |
 * | 53       | 0x35     |          |          |          |          |          |          |          |          |
 * | 54       | 0x36     |          |          |          |          |          |          |          |          |
 * | 55       | 0x37     |          |          |          |          |          |          |          |          |
 

 * Abbreviation's (CHECKSUM FOR PARAMETER IS DENOTED BY #)
 ***********************************************************************************************************
*
 */
