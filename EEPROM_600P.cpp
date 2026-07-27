// 
// 
// 

#include "EEPROM_600P.h"

EEPROMClass EEPROM;

void EEPROMClass::init()
{
	uint8_t read = 0;

	debugEN = EEPROM.read(EEPROM0, 0);

	read = EEPROM.read(EEPROM0, 2);
	if (read == 0 || read == 1) { bitWrite(HardwareStatus, 1, read); } //RS485 enabled

	modemdebugEN = EEPROM.read(EEPROM0, 4);

	PCBserialno = EEPROM.readlong(EEPROM0, 13);

}

uint8_t EEPROMClass::read(int16_t deviceaddress, uint16_t eeaddress) {

	uint8_t readdata = 0x09;

	Wire1.beginTransmission(deviceaddress);
	Wire1.write((uint8_t)(eeaddress >> 8));   // MSB
	Wire1.write((uint8_t)(eeaddress & 0xFF)); // LSB
	Wire1.endTransmission();

	Wire1.requestFrom(deviceaddress, 1, 1);

	if (Wire1.available()) readdata = Wire1.read();

	return readdata;
}

void EEPROMClass::write(int16_t deviceaddress, uint16_t eeaddress, uint8_t data) {

	Wire1.beginTransmission(deviceaddress);
	Wire1.write((uint8_t)(eeaddress >> 8));   // MSB
	Wire1.write((uint8_t)(eeaddress & 0xFF)); // LSB
	Wire1.write(data);
	Wire1.endTransmission();

	delay(5);

}

uint32_t EEPROMClass::readlong(int16_t deviceaddress, uint16_t eeaddress) {

	uint32_t value = 0;

	Wire1.beginTransmission(deviceaddress);
	Wire1.write((uint8_t)(eeaddress >> 8));   // MSB
	Wire1.write((uint8_t)(eeaddress & 0xFF)); // LSB
	Wire1.endTransmission();

	Wire1.requestFrom(deviceaddress, 4, 1);

	if (Wire1.available()) { value = Wire1.read(); }
	if (Wire1.available()) { value = value * 256 + Wire1.read(); }
	if (Wire1.available()) { value = value * 256 + Wire1.read(); }
	if (Wire1.available()) { value = value * 256 + Wire1.read(); }

	Wire1.endTransmission();
	return value;

}

uint8_t EEPROMClass::getflashbank() {
	
	uint8_t currentflashbank = 0;

	if (flash_is_gpnvm_set(2) == EFC_RC_YES) { //SETS VALUE TO 2 IF PROGRAM IS CURRENTLY RUNNNING ON FLASHBANK 1
		currentflashbank = 2;
	}

	if (flash_is_gpnvm_set(2) == EFC_RC_NO) { //SETS VALUE TO 1 IF PROGRAM IS CURRENTLY RUNNNING ON FLASHBANK 0
		currentflashbank = 1;
	}

	if (currentflashbank != 1 && currentflashbank != 2) {

		currentflashbank = 0; //return error
	}

	return currentflashbank;
	
}



