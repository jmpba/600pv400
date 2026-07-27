// 
// 
// 

#include "DS1338_600P.h"

DS1338Class DS1338;

void DS1338Class::init()
{
	//^^^^^^^^^Verify and/or set control register^^^^^^^^^^

	uint8_t controlreg = DS1338.read(RTC0, 0x07);

	if (controlreg != 0) {
		if (debugEN == 1) { 
			DEBUG.println(F("Setting DS1338 control register!"));
		}

		DS1338.write(RTC0, 0x07, 0x00);
		controlreg = DS1338.read(RTC0, 0x07);
		if (controlreg != 0) { bitSet(faultBITS, 0); } //flag RTC error

	}

	//^^^^^^^^^Verify and/or set CH register^^^^^^^^^^

	uint8_t CHreg = DS1338.read(RTC0, 0x00);

	if (bitRead(CHreg, 7) == 1) {
		if (debugEN == 1) {
			DEBUG.println(F("Oscillator disabled..."));
		}

		DS1338.write(RTC0, 0x00, (CHreg & 0x7F));
		CHreg = DS1338.read(RTC0, 0x00);
		if (bitRead(CHreg,7) != 0) { bitSet(faultBITS, 0); } //flag RTC error
	}

	//^^^^^^^^^Verify and/or set 24hr register^^^^^^^^^^

	uint8_t reg24hr = DS1338.read(RTC0, 0x02);

	if (bitRead(reg24hr, 6) == 1) {
		if (debugEN == 1) {
			DEBUG.println(F("24 hour time not configured..."));
		}

		DS1338.write(RTC0, 0x02, (reg24hr & 0xBF));
		reg24hr = DS1338.read(RTC0, 0x02);
		if (bitRead(reg24hr, 6) != 0) { bitSet(faultBITS, 0); } //flag RTC error
	}

	DS1338.epoch = DS1338.readRTC(RTC0); //Update RTC time variable
	
}

uint8_t DS1338Class::read(int16_t deviceaddress, uint16_t eeaddress) {

	uint8_t value = 0;

	Wire1.beginTransmission(deviceaddress);
	Wire1.write((uint8_t)(eeaddress)); //control address

	uint8_t devResponse = Wire1.endTransmission(false);

	if (devResponse == 0) {

		Wire1.requestFrom(deviceaddress, 1);

		if (Wire1.available()) { value = Wire1.read(); }

		return value;
	}

	else {
		bitSet(faultBITS, 0); //flag RTC error
		return 0;
	}
}

void DS1338Class::write(int16_t deviceaddress, uint16_t eeaddress, uint8_t data) {

	Wire1.beginTransmission(deviceaddress);
	Wire1.write((uint8_t)(eeaddress));

	Wire1.write((uint8_t)data);

	uint8_t devResponse = Wire1.endTransmission();

	if (devResponse != 0) { bitSet(faultBITS, 0); } //flag RTC error

	delayMicroseconds(6000);

}

uint32_t DS1338Class::readRTC(int16_t deviceaddress) {

	uint8_t bcd_registers[7] = { 0 };
	uint8_t bin_registers[7] = { 0 };
	uint8_t count = 0;

	Wire1.beginTransmission(deviceaddress);
	Wire1.write((uint8_t)(0x00));

	uint8_t devResponse = Wire1.endTransmission(false);

	if (devResponse == 0) {

		Wire1.requestFrom(deviceaddress, 7);

		while (count < 7) {
			if (Wire1.available()) { bcd_registers[count] = Wire1.read(); }
			count++;
		}

		bin_registers[6] = dataconversion.bcd2bin(bcd_registers[6]); //+ 2000U; year
		bin_registers[5] = dataconversion.bcd2bin(bcd_registers[5]); //month
		bin_registers[4] = dataconversion.bcd2bin(bcd_registers[4]); //date
		bin_registers[3] = dataconversion.bcd2bin(bcd_registers[3]); //day
		bin_registers[2] = dataconversion.bcd2bin(bcd_registers[2]); //hour
		bin_registers[1] = dataconversion.bcd2bin(bcd_registers[1]); //min
		bin_registers[0] = dataconversion.bcd2bin(bcd_registers[0] & 0x7F); //sec

		struct tm t;
		time_t t_of_day;

		t.tm_year = bin_registers[6] + 100;//2019 - 1900;  // Year - 1900
		t.tm_mon = bin_registers[5] - 1;           // Month, where 0 = jan
		t.tm_mday = bin_registers[4];          // Day of the month
		t.tm_hour = bin_registers[2];
		t.tm_min = bin_registers[1];
		t.tm_sec = bin_registers[0];
		t.tm_isdst = -1;        // Is DST on? 1 = yes, 0 = no, -1 = unknown
		t_of_day = mktime(&t);

		//DEBUG.print("seconds since the Epoch: ");
		//DEBUG.println((long)t_of_day);

		return (uint32_t)t_of_day;
	}

	else {
		bitSet(faultBITS, 0); //flag RTC error
		return 0;
	}

}

uint8_t DS1338Class::setRTC(int16_t deviceaddress, time_t time) {

	struct tm thetime;
	thetime = *localtime(&time);

	uint8_t bcd_registers[7] = { 0 };

	bcd_registers[0] = dataconversion.bin2bcd(thetime.tm_sec & 0x7F);
	bcd_registers[1] = dataconversion.bin2bcd(thetime.tm_min);
	bcd_registers[2] = dataconversion.bin2bcd(thetime.tm_hour & 0xBF);
	bcd_registers[3] = dataconversion.bin2bcd(thetime.tm_wday + 1);
	bcd_registers[4] = dataconversion.bin2bcd(thetime.tm_mday);
	bcd_registers[5] = dataconversion.bin2bcd(thetime.tm_mon + 1);
	bcd_registers[6] = dataconversion.bin2bcd(thetime.tm_year - 100);

	Wire1.beginTransmission(deviceaddress);
	Wire1.write((uint8_t)(0x00));
	Wire1.write((uint8_t)(bcd_registers[0]));
	Wire1.write((uint8_t)(bcd_registers[1]));
	Wire1.write((uint8_t)(bcd_registers[2]));
	Wire1.write((uint8_t)(bcd_registers[3]));
	Wire1.write((uint8_t)(bcd_registers[4]));
	Wire1.write((uint8_t)(bcd_registers[5]));
	Wire1.write((uint8_t)(bcd_registers[6]));
	uint8_t devResponse = Wire1.endTransmission();

	return devResponse;
}


void DS1338Class::writenvRAMlong(int16_t deviceaddress, uint16_t eeaddress, uint32_t data) {

	eeaddress += 0x08; //MEMORY ADDRESS OFFSET

	Wire1.beginTransmission(deviceaddress);
	Wire1.write((uint8_t)(eeaddress));

	unsigned int valueH = (data >> 16);
	unsigned int valueL = (data & 0xFFFF);

	Wire1.write((uint8_t)(highByte(valueH)));
	Wire1.write((uint8_t)(lowByte(valueH)));
	Wire1.write((uint8_t)(highByte(valueL)));
	Wire1.write((uint8_t)(lowByte(valueL)));

	uint8_t devResponse = Wire1.endTransmission();

	if (devResponse != 0) { bitSet(faultBITS, 0); } //flag RTC error

	delayMicroseconds(6000);

}

uint32_t DS1338Class::readnvRAMlong(int16_t deviceaddress, uint16_t eeaddress) {

	unsigned long value = 0;
	eeaddress += 0x08; //MEMORY ADDRESS OFFSET

	Wire1.beginTransmission(deviceaddress);
	Wire1.write((uint8_t)(eeaddress));

	uint8_t devResponse = Wire1.endTransmission(false);

	if (devResponse == 0) {

		Wire1.requestFrom(deviceaddress, 4);

		if (Wire1.available()) { value = Wire1.read(); }
		if (Wire1.available()) { value = value * 256 + Wire1.read(); }
		if (Wire1.available()) { value = value * 256 + Wire1.read(); }
		if (Wire1.available()) { value = value * 256 + Wire1.read(); }

		//delayMicroseconds(6000);

		return value;
	}

	else {
		bitSet(faultBITS, 0); //flag RTC error
		return 0;
	}

}