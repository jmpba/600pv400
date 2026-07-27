// 
// 
// 

#include "MCP98244_600P.h"

MCP98244Class MCP98244;

void MCP98244Class::init()
{


}

float MCP98244Class::readtemp() {

	uint8_t upper = 0; //MSB
	uint8_t lower = 0; //LSB
	float temp = 0;

	Wire1.beginTransmission(MCP98244_address);
	Wire1.write((uint8_t)(0x05));
	Wire1.endTransmission();

	Wire1.requestFrom(MCP98244_address, 2, 1);

	if (Wire1.available()) { upper = Wire1.read(); } //read MSB
	if (Wire1.available()) { lower = Wire1.read(); } //read LSB  

	if ((upper & 0x80) == 0x80) {} //code here for critical temp action
	if ((upper & 0x40) == 0x40) {} //code here for upper temp limit action
	if ((upper & 0x20) == 0x20) {} //code here for lower temp limit action

	upper = upper & 0x1F; //clear flag bits

	if ((upper & 0x10) == 0x10) {
		upper = upper & 0x0F; //clear sign (bit 12)
		temp = ((upper * pow(2, 4)) + (lower * pow(2, -4))) - 256;
	} //if temperature is a negative value use this calculation.

	else { temp = ((upper * pow(2, 4)) + (lower * pow(2, -4))); } // else if temperature is positive value use this calculation.

	return temp;

}


