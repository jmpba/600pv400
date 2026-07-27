#include "dataconversion_600P.h"

DataconversionClass dataconversion;

void DataconversionClass::init()
{


}

uint8_t DataconversionClass::bcd2bin(uint8_t val) {
	return val - 6 * (val >> 4); 
}

uint8_t DataconversionClass::bin2bcd(uint8_t val) {
	return val + 6 * (val / 10); 
}

uint16_t DataconversionClass::highWord(uint32_t ww) {
	return (uint16_t)((ww) >> 16);
}

uint16_t DataconversionClass::lowWord(uint32_t ww) {
	return (uint16_t)((ww) & 0xFFFF);
}

float DataconversionClass::float32_from_four_uint8(uint8_t MSSB_uint, uint8_t MSB_uint, uint16_t LSSB_uint, uint16_t LSB_uint) {

	uint16_t qw = ((uint16_t)MSSB_uint << 8) | MSB_uint;
	uint16_t qe = ((uint16_t)LSSB_uint << 8) | LSB_uint;

	union
	{
		float f_number;
		uint16_t uint16_arr[2];
	} union_for_conv;
	union_for_conv.uint16_arr[0] = qw;
	union_for_conv.uint16_arr[1] = qe;
	return union_for_conv.f_number;
}

uint32_t DataconversionClass::uint32_t_from_four_uint8(uint8_t MSSB_uint, uint8_t MSB_uint, uint16_t LSSB_uint, uint16_t LSB_uint) {

	uint16_t qw = ((uint16_t)MSSB_uint << 8) | MSB_uint;
	uint16_t qe = ((uint16_t)LSSB_uint << 8) | LSB_uint;

	union
	{
		uint32_t u_number;
		uint16_t uint16_arr[2];
	} union_for_conv;
	union_for_conv.uint16_arr[0] = qw;
	union_for_conv.uint16_arr[1] = qe;
	return union_for_conv.u_number;
}


float DataconversionClass::float32_from_two_uint16(uint16_t MSB_uint, uint16_t LSB_uint) {
	union
	{
		float f_number;
		uint16_t uint16_arr[2];
	} union_for_conv;
	union_for_conv.uint16_arr[0] = LSB_uint;
	union_for_conv.uint16_arr[1] = MSB_uint;
	return union_for_conv.f_number;
}

uint32_t DataconversionClass::uint32t_from_two_uint16_t(uint16_t MSB_uint, uint16_t LSB_uint) {
	union
	{
		uint32_t f_number;
		uint16_t uint16_arr[2];
	} union_for_conv;
	union_for_conv.uint16_arr[0] = LSB_uint;
	union_for_conv.uint16_arr[1] = MSB_uint;
	return union_for_conv.f_number;
}

uint16_t DataconversionClass::pressureconvert(float fval)
{
	if (fval < 0) return(0);
	if (fval > 65535) return(UINT16_MAX);
	return((uint16_t)lrintf(fval));
}