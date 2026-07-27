#ifndef _main_h
#define _main_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#include "DueFlashStorage.h"
#include "serialcommands_600P.h"
#include "DPT145_600P.h"
#include <Wire.h>
#include "EEPROM_600P.h"
#include "MCP98244_600P.h"
#include "DS1338_600P.h"
#include "dataconversion_600P.h"
#include "modbusRTU_600P.h"
#include "IO_600P.h"
#include <SPI.h>
#include "SD_600P.h"
#include "HL7650_600P.h"
#include "HL7650commands_600P.h"
#include "MQTT_600P.h"
#include "HL7650response_600P.h"

#else
#include "WProgram.h"
#endif

const float thisCODEversion = 4.00;
const char hardwareVersion[5] = "2.40";
const char Model[5] = "600P";

#define rs4851REDE 8
#define rs4852REDE 9
#define rs4853REDE 36

#define BLUE_LED 37
#define YELLOW_LED 38
#define RED_LED 39

#define FLGB1 23
#define FLGB2 24
#define FLGB3 25

#define sense1PWR 27
#define sense2PWR 28
#define sense3PWR 26


#define RS485 Serial3
#define modemUART Serial
#define btUART Serial1

#define EEPROM0 0x51               //I2C Address of CAT24C256 EEPROM chip
#define RTC0 0x68
#define MCP98244_address 0x1A

typedef enum  {

	GENERAL,
	BACKUP,
	WATCHDOG,
	SOFTWARE,
	USER

} resettype;


#endif

/*
faultBITS

bit 00 - RTC error
bit 01 - SD card error
bit 02 - 
bit 03 - 
bit 04 - 
bit 05 - 
bit 06 - 
bit 07 - 
bit 08 - 
bit 09 - 
bit 10 - 
bit 11 - 
bit 12 - Sensor A COMM error
bit 13 - Sensor B COMM error
bit 14 - Sensor C COMM error
bit 15 - Sensor A 24V fault
bit 16 - Sensor B 24V fault
bit 17 - Sensor C 24V fault
bit 18 - Sensor A fault
bit 19 - Sensor B fault
bit 20 - Sensor C fault
bit 21 -
bit 22 -
bit 23 -
bit 24 -
bit 25 -
bit 26 -
bit 27 -
bit 28 -
bit 29 -
bit 30 -
bit 31 -

*/


/*
HardwareStatus

bit 00 -
bit 01 - RS485 enabled
bit 02 - Sensor 1 installed
bit 03 - Sensor 2 installed
bit 04 - Sensor 3 installed
bit 05 - Sensor 1 online
bit 06 - Sensor 2 online
bit 07 - Sensor 3 online
bit 08 -
bit 09 -
bit 10 - PCB over temperature
bit 11 - Modem over temperature
bit 12 -
bit 13 -
bit 14 -
bit 15 -
bit 16 -
bit 17 -
bit 18 -
bit 19 -
bit 20 -
bit 21 -
bit 22 -
bit 23 - Sensor 1 auto purge enabled
bit 24 - Sensor 1 startup purge enabled
bit 25 - Sensor 1 purge status
bit 26 - Sensor 2 auto purge enabled
bit 27 - Sensor 2 startup purge enabled
bit 28 - Sensor 2 purge status
bit 29 - Sensor 3 auto purge enabled
bit 30 - Sensor 3 startup purge enabled
bit 31 - Sensor 3 purge status

*/