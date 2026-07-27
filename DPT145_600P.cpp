// 
// 
// 

#include "DPT145_600P.h"

DPT145Class sensor1;
DPT145Class sensor2;
DPT145Class sensor3;
sensorClass sensor;

void DPT145Class::init()
{


}

void sensorClass::init()
{
	RS485.begin(19200, SERIAL_8E1); //open serial 3 port for RS485

	sensor1.slaveID = 1;
	sensor2.slaveID = 2;
	sensor3.slaveID = 3;

	bitWrite(HardwareStatus, 2, 1); //enable sensor 1
	bitWrite(HardwareStatus, 3, 1); //enable sensor 2
	bitWrite(HardwareStatus, 4, 1); //enable sensor 3
}

void sensorClass::read()
{
	modbusRTU.process_slave_response();
	modbusRTU.ModbusMasterTransaction();

	if (bitRead(HardwareStatus, 1) == 1) { //If RS485 enabled

		switch (pollSequence) {

			//--------------------------------------------------------------------------------------------------------------------------------
			//------------------------------------------------------  SENSOR 1  --------------------------------------------------------------
			//--------------------------------------------------------------------------------------------------------------------------------

		case 0: //SF6 Paramters #####################################################################################################

			if (bitRead(HardwareStatus, 2) == 1 && IO.PWRstage == IO.COMPLETE) { //IF RS485 ENABLED
				modbusRTU.readHoldingRegisters(sensor1.slaveID, 0x04, 46);  // Poll modbus registers 0x04 to 0x31 (5 to 50) ((0x04), 46);
				pollSequence++;
			}

			else { pollSequence += 3; } //skip sensor 1 poll if disabled

			break;

		case 1: //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

			if (modbusRTU.u8MBStatus == ku8MBSuccess) {

				if (bitRead(faultBITS, 12) == 1) { bitWrite(faultBITS, 12, 0); } //CLEAR sensor 1 COMM fault

				unsigned int register2 = 0;
				unsigned int register1 = 0;
				float combinedregister = 0;
				uint16_t combinedintregister = 0;

				register2 = modbusRTU.getResponseBuffer(0x01); //Temperature
				register1 = modbusRTU.getResponseBuffer(0x00);
				combinedregister = dataconversion.float32_from_two_uint16(register2, register1);
				if (isnan(combinedregister) == 1) { combinedregister = -2; }
				combinedregister = roundf(combinedregister * 100) / 100; //round to 2dp
				if (combinedregister != sensor1.T) { //if the value changes then update
					sensor1.T = combinedregister; 
				
				} 

				register2 = modbusRTU.getResponseBuffer(0x03); //Dewpoint
				register1 = modbusRTU.getResponseBuffer(0x02);
				combinedregister = dataconversion.float32_from_two_uint16(register2, register1);
				if (isnan(combinedregister) == 1) { combinedregister = -2; }
				combinedregister = roundf(combinedregister * 100) / 100; //round to 2dp
				if (combinedregister != sensor1.Tdf) {  //if the value changes then update
					sensor1.Tdf = combinedregister; 
				
				}

				register2 = modbusRTU.getResponseBuffer(0x07); //Dewpoint atm
				register1 = modbusRTU.getResponseBuffer(0x06);
				combinedregister = dataconversion.float32_from_two_uint16(register2, register1);
				if (isnan(combinedregister) == 1) { combinedregister = -2; }
				combinedregister = roundf(combinedregister * 100) / 100; //round to 2dp
				if (combinedregister != sensor1.Tdfa) { //if the value changes then update
					sensor1.Tdfa = combinedregister; 
				
				} 

				register2 = modbusRTU.getResponseBuffer(0x11); //Moisture
				register1 = modbusRTU.getResponseBuffer(0x10);
				combinedregister = dataconversion.float32_from_two_uint16(register2, register1);
				if (isnan(combinedregister) == 1) { combinedregister = -2; }
				combinedintregister = dataconversion.pressureconvert(combinedregister);
				if (combinedintregister != sensor1.H2O) { //if the value changes then update
					sensor1.H2O = combinedintregister;
				
				} 

				register2 = modbusRTU.getResponseBuffer(0x29); //Pressure
				register1 = modbusRTU.getResponseBuffer(0x28);
				combinedregister = dataconversion.float32_from_two_uint16(register2, register1);
				if (isnan(combinedregister) == 1) { combinedregister = -2; }
				else { combinedregister = combinedregister * 1000; }//convert bar to mbar
				combinedintregister = dataconversion.pressureconvert(combinedregister);
				if (combinedintregister != sensor1.P) { //if the value changes then update
					sensor1.P = combinedintregister;

				}

				register2 = modbusRTU.getResponseBuffer(0x2B); //Density
				register1 = modbusRTU.getResponseBuffer(0x2A);
				combinedregister = dataconversion.float32_from_two_uint16(register2, register1);
				if (isnan(combinedregister) == 1) { combinedregister = -2; }
				combinedregister = roundf(combinedregister * 100) / 100; //round to 2dp
				if (combinedregister != sensor1.Rhoo) { //if the value changes then update
					sensor1.Rhoo = combinedregister;

				}

				register2 = modbusRTU.getResponseBuffer(0x2D); //Pressure @20C
				register1 = modbusRTU.getResponseBuffer(0x2C);
				combinedregister = dataconversion.float32_from_two_uint16(register2, register1);
				if (isnan(combinedregister) == 1) { combinedregister = -2; }
				else { combinedregister = combinedregister * 1000; }//convert bar to mbar
				combinedintregister = dataconversion.pressureconvert(combinedregister);
				if (combinedintregister != sensor1.Pnorm) { //if the value changes then update
					sensor1.Pnorm = combinedintregister;

				}

				modbusRTU.u8MBStatus = ku8MBDone;
			}

			else if (modbusRTU.u8MBStatus != ku8MBWaiting) {
				modbusRTU.print_the_error((const uint8_t)1);
				if (bitRead(faultBITS, 12) == 0 && bitRead(HardwareStatus, 2) == 1) { bitWrite(faultBITS, 12, 1); } //FLAG sensor 1 COMM fault		
				modbusRTU.u8MBStatus = ku8MBDone;
			}

			if (modbusRTU.u8MBStatus == ku8MBDone) {
				polltimeout = millis();
				pollSequence++;
			}

			break;

		case 2: //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

			if ((millis() - polltimeout) >= BETWEENPOLLTIME) { pollSequence++; } //TIME DELAY BETWEEN POLLS

			if (millis() < polltimeout) { polltimeout = millis(); } //reset in case ul_tickcount rolls over to 0

			break;

		case 3: //Device Identification Objects #####################################################################################################

			if (bitRead(HardwareStatus, 2) == 1 && IO.PWRstage == IO.COMPLETE) { //IF RS485 ENABLED
				modbusRTU.readDeviceIdentification(sensor1.slaveID, 0, 0x0E, 3); //(SLAVEID, OBJECT ID, MEI TYPE, READ DEVICE ID CODE)
				pollSequence++;
			}

			else { pollSequence += 3; } //skip sensor 1 poll if disabled

			break;

		case 4: //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

			if (modbusRTU.u8MBStatus == ku8MBSuccess) {

				if (bitRead(faultBITS, 12) == 1) { bitWrite(faultBITS, 12, 0); } //CLEAR sensor 1 COMM fault

				char dataParameters[64];
				modbusRTU.getCode2BResponse(0x80, dataParameters, 64);

				if (strstr(sensor1.serialNO, dataParameters) == NULL) {//UPDATE SENSOR SERIAL NUMBER TO SERVER IF IT HAS CHANGED.
					strncpy(sensor1.serialNO, dataParameters, 8);
					MQTT.queue_MQTT_update(sensor1.serialNO, "S1"); //add new value to MQTT send queue
					//if (debugEN == 1) { DEBUG.print(F("SENSOR 1 S/N UPDATE: ")); DEBUG.println(sensor1.serialNO); }
				}

				/*
				DEBUG.println(F("SENSOR 1"));
				DEBUG.print(F("Vendor Name: "));
				modbusRTU.getCode2BResponse(0x00, dataParameters, 64);
				DEBUG.println(dataParameters);

				DEBUG.print(F("Product Code: "));
				modbusRTU.getCode2BResponse(0x01, dataParameters, 64);
				DEBUG.println(dataParameters);

				DEBUG.print(F("Version: "));
				modbusRTU.getCode2BResponse(0x02, dataParameters, 64);
				DEBUG.println(dataParameters);

				DEBUG.print(F("Website: "));
				modbusRTU.getCode2BResponse(0x03, dataParameters, 64);
				DEBUG.println(dataParameters);

				DEBUG.print(F("Product Name: "));
				modbusRTU.getCode2BResponse(0x04, dataParameters, 64);
				DEBUG.println(dataParameters);

				DEBUG.print(F("Serial No: "));
				modbusRTU.getCode2BResponse(0x80, dataParameters, 64);
				DEBUG.println(dataParameters);

				DEBUG.print(F("Cal Date: "));
				modbusRTU.getCode2BResponse(0x81, dataParameters, 64);
				DEBUG.println(dataParameters);

				DEBUG.print(F("Cal Note: "));
				modbusRTU.getCode2BResponse(0x82, dataParameters, 64);
				DEBUG.println(dataParameters);
				*/		
				modbusRTU.u8MBStatus = ku8MBDone;
			}

			else if (modbusRTU.u8MBStatus != ku8MBWaiting) {
				modbusRTU.print_the_error((const uint8_t)2);
				if (bitRead(faultBITS, 12) == 0 && bitRead(HardwareStatus, 2) == 1) { bitWrite(faultBITS, 12, 1); } //FLAG sensor 1 COMM faul	
				modbusRTU.u8MBStatus = ku8MBDone;
			}

			if (modbusRTU.u8MBStatus == ku8MBDone) {
				polltimeout = millis();
				pollSequence++;
			}

			break;

		case 5: //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

			if ((millis() - polltimeout) >= BETWEENPOLLTIME) { pollSequence++; } //TIME DELAY BETWEEN POLLS

			if (millis() < polltimeout) { polltimeout = millis(); } //reset in case ul_tickcount rolls over to 0

			break;

		case 6: //DPT145 FAULT STATUS, ONLINE STATUS AND ERROR CODE #####################################################################################################

			if (bitRead(HardwareStatus, 2) == 1 && IO.PWRstage == IO.COMPLETE) { //IF RS485 ENABLED
				modbusRTU.readHoldingRegisters(sensor1.slaveID, 0x200, 5);  // Poll modbus registers 0x200 to 0x204 (513 to 514)
				pollSequence++;
			}

			else { pollSequence += 3; } //skip sensor 1 poll if disabled

			break;

		case 7: //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

			if (modbusRTU.u8MBStatus == ku8MBSuccess) {

				if (bitRead(faultBITS, 12) == 1) { bitWrite(faultBITS, 12, 0); } //CLEAR sensor 1 COMM fault

				sensor1.faultstatus = modbusRTU.getResponseBuffer(0);  //0x200(513)
				if (sensor1.faultstatus == 1) { bitWrite(faultBITS, 18, 0); }
				else if (sensor1.faultstatus == 0) { bitWrite(faultBITS, 18, 1); }

				sensor1.onlinestatus = modbusRTU.getResponseBuffer(1);  //0x201(514)
				if (sensor1.onlinestatus == 1) { bitWrite(HardwareStatus, 5, 1); }
				else if (sensor1.onlinestatus == 0) { bitWrite(HardwareStatus, 5, 0); }

				uint32_t errcode = dataconversion.uint32t_from_two_uint16_t(modbusRTU.getResponseBuffer(4), modbusRTU.getResponseBuffer(3)); //0x203 & 0x204 (516 & 517)

				if (sensor1.errorcode != errcode) {
					sensor1.errorcode = errcode;
					MQTT.queue_MQTT_update(sensor1.errorcode, "E1"); //add new value to MQTT send queue
				}

				modbusRTU.u8MBStatus = ku8MBDone;
			}

			else if (modbusRTU.u8MBStatus != ku8MBWaiting) {
				modbusRTU.print_the_error((const uint8_t)3);
				if (bitRead(faultBITS, 12) == 0 && bitRead(HardwareStatus, 2) == 1) { bitWrite(faultBITS, 12, 1); }//FLAG sensor 1 COMM fault
				bitWrite(HardwareStatus, 5, 0); //FLAG sensor 1 offline	
				modbusRTU.u8MBStatus = ku8MBDone;
			}

			if (modbusRTU.u8MBStatus == ku8MBDone) {
				polltimeout = millis();
				pollSequence++;
			}

			break;

		case 8: //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

			if ((millis() - polltimeout) >= BETWEENPOLLTIME) { pollSequence++; } //TIME DELAY BETWEEN POLLS

			if (millis() < polltimeout) { polltimeout = millis(); } //reset in case ul_tickcount rolls over to 0

			break;

		case 9: //DPT145 PURGE STATUS #####################################################################################################

			if (bitRead(HardwareStatus, 2) == 1 && IO.PWRstage == IO.COMPLETE) { //IF RS485 ENABLED
				modbusRTU.readHoldingRegisters(sensor1.slaveID, 0x502, 3);  // Poll modbus register 0x502 - 0x504 (1283 - 1285)
				pollSequence++;
			}

			else { pollSequence += 3; } //skip sensor 1 poll if disabled

			break;

		case 10: //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

			if (modbusRTU.u8MBStatus == ku8MBSuccess) {

				if (bitRead(faultBITS, 12) == 1) { bitWrite(faultBITS, 12, 0); } //CLEAR sensor 1 COMM fault

				sensor1.autopurge = modbusRTU.getResponseBuffer(0);  //0x502(1283)
				if (sensor1.autopurge == 0) { bitWrite(HardwareStatus, 23, 0); }
				else if (sensor1.autopurge == 1) { bitWrite(HardwareStatus, 23, 1); }

				sensor1.startuppurge = modbusRTU.getResponseBuffer(1);  //0x503(1284)
				if (sensor1.startuppurge == 0) { bitWrite(HardwareStatus, 24, 0); }
				else if (sensor1.startuppurge == 1) { bitWrite(HardwareStatus, 24, 1); }

				sensor1.purgestatus = modbusRTU.getResponseBuffer(2);  //0x504(1285)
				if (sensor1.purgestatus == 0) { bitWrite(HardwareStatus, 25, 0); }
				else if (sensor1.purgestatus == 1) { bitWrite(HardwareStatus, 25, 1); }

				modbusRTU.u8MBStatus = ku8MBDone;
			}

			else if (modbusRTU.u8MBStatus != ku8MBWaiting) {
				modbusRTU.print_the_error((const uint8_t)4);
				if (bitRead(faultBITS, 12) == 0 && bitRead(HardwareStatus, 2) == 1) { bitWrite(faultBITS, 12, 1); }//FLAG sensor 1 COMM fault				
				bitWrite(HardwareStatus, 25, 0); //FLAG sensor 1 purging status OFF
				modbusRTU.u8MBStatus = ku8MBDone;
			}

			if (modbusRTU.u8MBStatus == ku8MBDone) {
				polltimeout = millis();
				pollSequence++;
			}

			break;

		case 11: //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

			if ((millis() - polltimeout) >= BETWEENPOLLTIME) { pollSequence++; } //TIME DELAY BETWEEN POLLS

			if (millis() < polltimeout) { polltimeout = millis(); } //reset in case ul_tickcount rolls over to 0

			break;

			//--------------------------------------------------------------------------------------------------------------------------------
			//------------------------------------------------------  SENSOR 2  --------------------------------------------------------------
			//--------------------------------------------------------------------------------------------------------------------------------

		case 12: //SF6 Paramters #####################################################################################################

			if (bitRead(HardwareStatus, 3) == 1 && IO.PWRstage == IO.COMPLETE) { //IF RS485 ENABLED
				modbusRTU.readHoldingRegisters(sensor2.slaveID, 0x04, 46);  // Poll modbus registers 0x04 to 0x31 (5 to 50) ((0x04), 46);
				pollSequence++;
			}

			else { pollSequence += 3; } //skip sensor 1 poll if disabled

			break;

		case 13: //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

			if (modbusRTU.u8MBStatus == ku8MBSuccess) {

				if (bitRead(faultBITS, 13) == 1) { bitWrite(faultBITS, 13, 0); } //CLEAR sensor 1 COMM fault

				unsigned int register2 = 0;
				unsigned int register1 = 0;
				float combinedregister = 0;
				uint16_t combinedintregister = 0;

				register2 = modbusRTU.getResponseBuffer(0x01); //Temperature
				register1 = modbusRTU.getResponseBuffer(0x00);
				combinedregister = dataconversion.float32_from_two_uint16(register2, register1);
				if (isnan(combinedregister) == 1) { combinedregister = -2; }
				combinedregister = roundf(combinedregister * 100) / 100; //round to 2dp
				if (combinedregister != sensor2.T) { //if the value changes then update
					sensor2.T = combinedregister;

				}

				register2 = modbusRTU.getResponseBuffer(0x03); //Dewpoint
				register1 = modbusRTU.getResponseBuffer(0x02);
				combinedregister = dataconversion.float32_from_two_uint16(register2, register1);
				if (isnan(combinedregister) == 1) { combinedregister = -2; }
				combinedregister = roundf(combinedregister * 100) / 100; //round to 2dp
				if (combinedregister != sensor2.Tdf) {  //if the value changes then update
					sensor2.Tdf = combinedregister;

				}

				register2 = modbusRTU.getResponseBuffer(0x07); //Dewpoint atm
				register1 = modbusRTU.getResponseBuffer(0x06);
				combinedregister = dataconversion.float32_from_two_uint16(register2, register1);
				if (isnan(combinedregister) == 1) { combinedregister = -2; }
				combinedregister = roundf(combinedregister * 100) / 100; //round to 2dp
				if (combinedregister != sensor2.Tdfa) { //if the value changes then update
					sensor2.Tdfa = combinedregister;

				}

				register2 = modbusRTU.getResponseBuffer(0x11); //Moisture
				register1 = modbusRTU.getResponseBuffer(0x10);
				combinedregister = dataconversion.float32_from_two_uint16(register2, register1);
				if (isnan(combinedregister) == 1) { combinedregister = -2; }
				combinedintregister = dataconversion.pressureconvert(combinedregister);
				if (combinedintregister != sensor2.H2O) { //if the value changes then update
					sensor2.H2O = combinedintregister;

				}

				register2 = modbusRTU.getResponseBuffer(0x29); //Pressure
				register1 = modbusRTU.getResponseBuffer(0x28);
				combinedregister = dataconversion.float32_from_two_uint16(register2, register1);
				if (isnan(combinedregister) == 1) { combinedregister = -2; }
				else { combinedregister = combinedregister * 1000; }//convert bar to mbar
				combinedintregister = dataconversion.pressureconvert(combinedregister);
				if (combinedintregister != sensor2.P) { //if the value changes then update
					sensor2.P = combinedintregister;

				}

				register2 = modbusRTU.getResponseBuffer(0x2B); //Density
				register1 = modbusRTU.getResponseBuffer(0x2A);
				combinedregister = dataconversion.float32_from_two_uint16(register2, register1);
				if (isnan(combinedregister) == 1) { combinedregister = -2; }
				combinedregister = roundf(combinedregister * 100) / 100; //round to 2dp
				if (combinedregister != sensor2.Rhoo) { //if the value changes then update
					sensor2.Rhoo = combinedregister;

				}

				register2 = modbusRTU.getResponseBuffer(0x2D); //Pressure @20C
				register1 = modbusRTU.getResponseBuffer(0x2C);
				combinedregister = dataconversion.float32_from_two_uint16(register2, register1);
				if (isnan(combinedregister) == 1) { combinedregister = -2; }
				else { combinedregister = combinedregister * 1000; }//convert bar to mbar
				combinedintregister = dataconversion.pressureconvert(combinedregister);
				if (combinedintregister != sensor2.Pnorm) { //if the value changes then update
					sensor2.Pnorm = combinedintregister;

				}

				modbusRTU.u8MBStatus = ku8MBDone;
			}

			else if (modbusRTU.u8MBStatus != ku8MBWaiting) {
				modbusRTU.print_the_error((const uint8_t)5);
				if (bitRead(faultBITS, 13) == 0 && bitRead(HardwareStatus, 3) == 1) { bitWrite(faultBITS, 13, 1); }//FLAG sensor 2 COMM fault	
				modbusRTU.u8MBStatus = ku8MBDone;
			}

			if (modbusRTU.u8MBStatus == ku8MBDone) {
				polltimeout = millis();
				pollSequence++;
			}

			break;

		case 14: //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

			if ((millis() - polltimeout) >= BETWEENPOLLTIME) { pollSequence++; } //TIME DELAY BETWEEN POLLS

			if (millis() < polltimeout) { polltimeout = millis(); } //reset in case ul_tickcount rolls over to 0

			break;

		case 15: //Device Identification Objects #####################################################################################################

			if (bitRead(HardwareStatus, 3) == 1 && IO.PWRstage == IO.COMPLETE) { //IF RS485 ENABLED
				modbusRTU.readDeviceIdentification(sensor2.slaveID, 0, 0x0E, 3); //(SLAVEID, OBJECT ID, MEI TYPE, READ DEVICE ID CODE)
				pollSequence++;
			}

			else { pollSequence += 3; } //skip sensor 1 poll if disabled

			break;

		case 16: //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

			if (modbusRTU.u8MBStatus == ku8MBSuccess) {

				if (bitRead(faultBITS, 13) == 1) { bitWrite(faultBITS, 13, 0); } //CLEAR sensor 1 COMM fault

				char dataParameters[64];
				modbusRTU.getCode2BResponse(0x80, dataParameters, 64);

				if (strstr(sensor2.serialNO, dataParameters) == NULL) {//UPDATE SENSOR SERIAL NUMBER TO SERVER IF IT HAS CHANGED.
					strncpy(sensor2.serialNO, dataParameters, 8);
					MQTT.queue_MQTT_update(sensor2.serialNO, "S2"); //add new value to MQTT send queue
				}

				modbusRTU.u8MBStatus = ku8MBDone;
			}

			else if (modbusRTU.u8MBStatus != ku8MBWaiting) {
				modbusRTU.print_the_error((const uint8_t)6);
				if (bitRead(faultBITS, 13) == 0 && bitRead(HardwareStatus, 3) == 1) { bitWrite(faultBITS, 13, 1); }//FLAG sensor 2 COMM fault	
				modbusRTU.u8MBStatus = ku8MBDone;
			}

			if (modbusRTU.u8MBStatus == ku8MBDone) {
				polltimeout = millis();
				pollSequence++;
			}

			break;

		case 17: //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

			if ((millis() - polltimeout) >= BETWEENPOLLTIME) { pollSequence++; } //TIME DELAY BETWEEN POLLS

			if (millis() < polltimeout) { polltimeout = millis(); } //reset in case ul_tickcount rolls over to 0

			break;

		case 18: //DPT145 FAULT STATUS, ONLINE STATUS AND ERROR CODE #####################################################################################################

			if (bitRead(HardwareStatus, 3) == 1 && IO.PWRstage == IO.COMPLETE) { //IF RS485 ENABLED
				modbusRTU.readHoldingRegisters(sensor2.slaveID, 0x200, 5);  // Poll modbus registers 0x200 to 0x204 (513 to 514)
				pollSequence++;
			}

			else { pollSequence += 3; } //skip sensor 1 poll if disabled

			break;

		case 19: //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

			if (modbusRTU.u8MBStatus == ku8MBSuccess) {

				if (bitRead(faultBITS, 13) == 1) { bitWrite(faultBITS, 13, 0); } //CLEAR sensor 1 COMM fault

				sensor2.faultstatus = modbusRTU.getResponseBuffer(0);  //0x200(513)
				if (sensor2.faultstatus == 1) { bitWrite(faultBITS, 19, 0); }
				else if (sensor2.faultstatus == 0) { bitWrite(faultBITS, 19, 1); }

				sensor2.onlinestatus = modbusRTU.getResponseBuffer(1);  //0x201(514)
				if (sensor2.onlinestatus == 1) { bitWrite(HardwareStatus, 6, 1); }
				else if (sensor2.onlinestatus == 0) { bitWrite(HardwareStatus, 6, 0); }

				uint32_t errcode = dataconversion.uint32t_from_two_uint16_t(modbusRTU.getResponseBuffer(4), modbusRTU.getResponseBuffer(3)); //0x203 & 0x204 (516 & 517)

				if (sensor2.errorcode != errcode) {
					sensor2.errorcode = errcode;
					MQTT.queue_MQTT_update(sensor2.errorcode, "E2"); //add new value to MQTT send queue
				}

				modbusRTU.u8MBStatus = ku8MBDone;
			}

			else if (modbusRTU.u8MBStatus != ku8MBWaiting) {
				modbusRTU.print_the_error((const uint8_t)7);
				if (bitRead(faultBITS, 13) == 0 && bitRead(HardwareStatus, 3) == 1) { bitWrite(faultBITS, 13, 1); }//FLAG sensor 2 COMM fault	
				bitWrite(HardwareStatus, 6, 0); //FLAG sensor 2 offline	
				modbusRTU.u8MBStatus = ku8MBDone;
			}

			if (modbusRTU.u8MBStatus == ku8MBDone) {
				polltimeout = millis();
				pollSequence++;
			}

			break;

		case 20: //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

			if ((millis() - polltimeout) >= BETWEENPOLLTIME) { pollSequence++; } //TIME DELAY BETWEEN POLLS

			if (millis() < polltimeout) { polltimeout = millis(); } //reset in case ul_tickcount rolls over to 0

			break;

		case 21: //DPT145 PURGE STATUS #####################################################################################################

			if (bitRead(HardwareStatus, 3) == 1 && IO.PWRstage == IO.COMPLETE) { //IF RS485 ENABLED
				modbusRTU.readHoldingRegisters(sensor2.slaveID, 0x502, 3);  // Poll modbus register 0x502 - 0x504 (1283 - 1285)
				pollSequence++;
			}

			else { pollSequence += 3; } //skip sensor 1 poll if disabled

			break;

		case 22: //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

			if (modbusRTU.u8MBStatus == ku8MBSuccess) {

				if (bitRead(faultBITS, 13) == 1) { bitWrite(faultBITS, 13, 0); } //CLEAR sensor 1 COMM fault

				sensor2.autopurge = modbusRTU.getResponseBuffer(0);  //0x502(1283)
				if (sensor2.autopurge == 0) { bitWrite(HardwareStatus, 26, 0); }
				else if (sensor2.autopurge == 1) { bitWrite(HardwareStatus, 26, 1); }

				sensor2.startuppurge = modbusRTU.getResponseBuffer(1);  //0x503(1284)
				if (sensor2.startuppurge == 0) { bitWrite(HardwareStatus, 27, 0); }
				else if (sensor2.startuppurge == 1) { bitWrite(HardwareStatus, 27, 1); }

				sensor2.purgestatus = modbusRTU.getResponseBuffer(2);  //0x504(1285)
				if (sensor2.purgestatus == 0) { bitWrite(HardwareStatus, 28, 0); }
				else if (sensor2.purgestatus == 1) { bitWrite(HardwareStatus, 28, 1); }

				modbusRTU.u8MBStatus = ku8MBDone;
			}

			else if (modbusRTU.u8MBStatus != ku8MBWaiting) {
				modbusRTU.print_the_error((const uint8_t)8);
				if (bitRead(faultBITS, 13) == 0 && bitRead(HardwareStatus, 3) == 1) { bitWrite(faultBITS, 13, 1); }//FLAG sensor 2 COMM fault	
				bitWrite(HardwareStatus, 28, 0); //FLAG sensor 2 purging status OFF
				modbusRTU.u8MBStatus = ku8MBDone;
			}

			if (modbusRTU.u8MBStatus == ku8MBDone) {
				polltimeout = millis();
				pollSequence++;
			}

			break;

		case 23: //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

			if ((millis() - polltimeout) >= BETWEENPOLLTIME) { pollSequence++; } //TIME DELAY BETWEEN POLLS

			if (millis() < polltimeout) { polltimeout = millis(); } //reset in case ul_tickcount rolls over to 0

			break;

			//--------------------------------------------------------------------------------------------------------------------------------
			//------------------------------------------------------  SENSOR 3  --------------------------------------------------------------
			//--------------------------------------------------------------------------------------------------------------------------------

		case 24: //SF6 Paramters #####################################################################################################

			if (bitRead(HardwareStatus, 4) == 1 && IO.PWRstage == IO.COMPLETE) { //IF RS485 ENABLED
				modbusRTU.readHoldingRegisters(sensor3.slaveID, 0x04, 46);  // Poll modbus registers 0x04 to 0x31 (5 to 50) ((0x04), 46);
				pollSequence++;
			}

			else { pollSequence += 3; } //skip sensor 1 poll if disabled

			break;

		case 25: //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

			if (modbusRTU.u8MBStatus == ku8MBSuccess) {

				if (bitRead(faultBITS, 14) == 1) { bitWrite(faultBITS, 14, 0); } //CLEAR sensor 1 COMM fault

				unsigned int register2 = 0;
				unsigned int register1 = 0;
				float combinedregister = 0;
				uint16_t combinedintregister = 0;

				register2 = modbusRTU.getResponseBuffer(0x01); //Temperature
				register1 = modbusRTU.getResponseBuffer(0x00);
				combinedregister = dataconversion.float32_from_two_uint16(register2, register1);
				if (isnan(combinedregister) == 1) { combinedregister = -2; }
				combinedregister = roundf(combinedregister * 100) / 100; //round to 2dp
				if (combinedregister != sensor3.T) { //if the value changes then update
					sensor3.T = combinedregister;

				}

				register2 = modbusRTU.getResponseBuffer(0x03); //Dewpoint
				register1 = modbusRTU.getResponseBuffer(0x02);
				combinedregister = dataconversion.float32_from_two_uint16(register2, register1);
				if (isnan(combinedregister) == 1) { combinedregister = -2; }
				combinedregister = roundf(combinedregister * 100) / 100; //round to 2dp
				if (combinedregister != sensor3.Tdf) {  //if the value changes then update
					sensor3.Tdf = combinedregister;

				}

				register2 = modbusRTU.getResponseBuffer(0x07); //Dewpoint atm
				register1 = modbusRTU.getResponseBuffer(0x06);
				combinedregister = dataconversion.float32_from_two_uint16(register2, register1);
				if (isnan(combinedregister) == 1) { combinedregister = -2; }
				combinedregister = roundf(combinedregister * 100) / 100; //round to 2dp
				if (combinedregister != sensor3.Tdfa) { //if the value changes then update
					sensor3.Tdfa = combinedregister;

				}

				register2 = modbusRTU.getResponseBuffer(0x11); //Moisture
				register1 = modbusRTU.getResponseBuffer(0x10);
				combinedregister = dataconversion.float32_from_two_uint16(register2, register1);
				if (isnan(combinedregister) == 1) { combinedregister = -2; }
				combinedintregister = dataconversion.pressureconvert(combinedregister);
				if (combinedintregister != sensor3.H2O) { //if the value changes then update
					sensor3.H2O = combinedintregister;

				}

				register2 = modbusRTU.getResponseBuffer(0x29); //Pressure
				register1 = modbusRTU.getResponseBuffer(0x28);
				combinedregister = dataconversion.float32_from_two_uint16(register2, register1);
				if (isnan(combinedregister) == 1) { combinedregister = -2; }
				else { combinedregister = combinedregister * 1000; }//convert bar to mbar
				combinedintregister = dataconversion.pressureconvert(combinedregister);
				if (combinedintregister != sensor3.P) { //if the value changes then update
					sensor3.P = combinedintregister;

				}

				register2 = modbusRTU.getResponseBuffer(0x2B); //Density
				register1 = modbusRTU.getResponseBuffer(0x2A);
				combinedregister = dataconversion.float32_from_two_uint16(register2, register1);
				if (isnan(combinedregister) == 1) { combinedregister = -2; }
				combinedregister = roundf(combinedregister * 100) / 100; //round to 2dp
				if (combinedregister != sensor3.Rhoo) { //if the value changes then update
					sensor3.Rhoo = combinedregister;

				}

				register2 = modbusRTU.getResponseBuffer(0x2D); //Pressure @20C
				register1 = modbusRTU.getResponseBuffer(0x2C);
				combinedregister = dataconversion.float32_from_two_uint16(register2, register1);
				if (isnan(combinedregister) == 1) { combinedregister = -2; }
				else { combinedregister = combinedregister * 1000; }//convert bar to mbar
				combinedintregister = dataconversion.pressureconvert(combinedregister);
				if (combinedintregister != sensor3.Pnorm) { //if the value changes then update
					sensor3.Pnorm = combinedintregister;

				}

				modbusRTU.u8MBStatus = ku8MBDone;
			}

			else if (modbusRTU.u8MBStatus != ku8MBWaiting) {
				modbusRTU.print_the_error((const uint8_t)9);
				if (bitRead(faultBITS, 14) == 0 && bitRead(HardwareStatus, 4) == 1) { bitWrite(faultBITS, 14, 1); }//FLAG sensor 3 COMM fault	
				modbusRTU.u8MBStatus = ku8MBDone;
			}

			if (modbusRTU.u8MBStatus == ku8MBDone) {
				polltimeout = millis();
				pollSequence++;
			}

			break;

		case 26: //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

			if ((millis() - polltimeout) >= BETWEENPOLLTIME) { pollSequence++; } //TIME DELAY BETWEEN POLLS

			if (millis() < polltimeout) { polltimeout = millis(); } //reset in case ul_tickcount rolls over to 0

			break;

		case 27: //Device Identification Objects #####################################################################################################

			if (bitRead(HardwareStatus, 4) == 1 && IO.PWRstage == IO.COMPLETE) { //IF RS485 ENABLED
				modbusRTU.readDeviceIdentification(sensor3.slaveID, 0, 0x0E, 3); //(SLAVEID, OBJECT ID, MEI TYPE, READ DEVICE ID CODE)
				pollSequence++;
			}

			else { pollSequence += 3; } //skip sensor 1 poll if disabled

			break;

		case 28: //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

			if (modbusRTU.u8MBStatus == ku8MBSuccess) {

				if (bitRead(faultBITS, 14) == 1) { bitWrite(faultBITS, 14, 0); } //CLEAR sensor 1 COMM fault

				char dataParameters[64];
				modbusRTU.getCode2BResponse(0x80, dataParameters, 64);

				if (strstr(sensor3.serialNO, dataParameters) == NULL) {//UPDATE SENSOR SERIAL NUMBER TO SERVER IF IT HAS CHANGED.
					strncpy(sensor3.serialNO, dataParameters, 8);
					MQTT.queue_MQTT_update(sensor3.serialNO, "S3"); //add new value to MQTT send queue
				}

				modbusRTU.u8MBStatus = ku8MBDone;
			}

			else if (modbusRTU.u8MBStatus != ku8MBWaiting) {
				modbusRTU.print_the_error((const uint8_t)10);
				if (bitRead(faultBITS, 14) == 0 && bitRead(HardwareStatus, 4) == 1) { bitWrite(faultBITS, 14, 1); }//FLAG sensor 3 COMM fault	
				modbusRTU.u8MBStatus = ku8MBDone;
			}

			if (modbusRTU.u8MBStatus == ku8MBDone) {
				polltimeout = millis();
				pollSequence++;
			}

			break;

		case 29: //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

			if ((millis() - polltimeout) >= BETWEENPOLLTIME) { pollSequence++; } //TIME DELAY BETWEEN POLLS

			if (millis() < polltimeout) { polltimeout = millis(); } //reset in case ul_tickcount rolls over to 0

			break;

		case 30: //DPT145 FAULT STATUS, ONLINE STATUS AND ERROR CODE #####################################################################################################

			if (bitRead(HardwareStatus, 4) == 1 && IO.PWRstage == IO.COMPLETE) { //IF RS485 ENABLED
				modbusRTU.readHoldingRegisters(sensor3.slaveID, 0x200, 5);  // Poll modbus registers 0x200 to 0x204 (513 to 514)
				pollSequence++;
			}

			else { pollSequence += 3; } //skip sensor 1 poll if disabled

			break;

		case 31: //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

			if (modbusRTU.u8MBStatus == ku8MBSuccess) {

				if (bitRead(faultBITS, 14) == 1) { bitWrite(faultBITS, 14, 0); } //CLEAR sensor 1 COMM fault

				sensor3.faultstatus = modbusRTU.getResponseBuffer(0);  //0x200(513)
				if (sensor3.faultstatus == 1) { bitWrite(faultBITS, 20, 0); }
				else if (sensor3.faultstatus == 0) { bitWrite(faultBITS, 20, 1); }

				sensor3.onlinestatus = modbusRTU.getResponseBuffer(1);  //0x201(514)
				if (sensor3.onlinestatus == 1) { bitWrite(HardwareStatus, 7, 1); }
				else if (sensor3.onlinestatus == 0) { bitWrite(HardwareStatus, 7, 0); }

				uint32_t errcode = dataconversion.uint32t_from_two_uint16_t(modbusRTU.getResponseBuffer(4), modbusRTU.getResponseBuffer(3)); //0x203 & 0x204 (516 & 517)

				if (sensor3.errorcode != errcode) {
					sensor3.errorcode = errcode;
					MQTT.queue_MQTT_update(sensor3.errorcode, "E3"); //add new value to MQTT send queue
				}

				modbusRTU.u8MBStatus = ku8MBDone;
			}

			else if (modbusRTU.u8MBStatus != ku8MBWaiting) {
				modbusRTU.print_the_error((const uint8_t)11);
				if (bitRead(faultBITS, 14) == 0 && bitRead(HardwareStatus, 4) == 1) { bitWrite(faultBITS, 14, 1); }//FLAG sensor 3 COMM fault	
				bitWrite(HardwareStatus, 7, 0); //FLAG sensor 3 offline	
				modbusRTU.u8MBStatus = ku8MBDone;
			}

			if (modbusRTU.u8MBStatus == ku8MBDone) {
				polltimeout = millis();
				pollSequence++;
			}

			break;

		case 32: //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

			if ((millis() - polltimeout) >= BETWEENPOLLTIME) { pollSequence++; } //TIME DELAY BETWEEN POLLS

			if (millis() < polltimeout) { polltimeout = millis(); } //reset in case ul_tickcount rolls over to 0

			break;

		case 33: //DPT145 PURGE STATUS #####################################################################################################

			if (bitRead(HardwareStatus, 4) == 1 && IO.PWRstage == IO.COMPLETE) { //IF RS485 ENABLED
				modbusRTU.readHoldingRegisters(sensor3.slaveID, 0x502, 3);  // Poll modbus register 0x502 - 0x504 (1283 - 1285)
				pollSequence++;
			}

			else { pollSequence += 3; } //skip sensor 1 poll if disabled

			break;

		case 34: //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

			if (modbusRTU.u8MBStatus == ku8MBSuccess) {

				if (bitRead(faultBITS, 14) == 1) { bitWrite(faultBITS, 14, 0); } //CLEAR sensor 1 COMM fault

				sensor3.autopurge = modbusRTU.getResponseBuffer(0);  //0x502(1283)
				if (sensor3.autopurge == 0) { bitWrite(HardwareStatus, 29, 0); }
				else if (sensor3.autopurge == 1) { bitWrite(HardwareStatus, 29, 1); }

				sensor3.startuppurge = modbusRTU.getResponseBuffer(1);  //0x503(1284)
				if (sensor3.startuppurge == 0) { bitWrite(HardwareStatus, 30, 0); }
				else if (sensor3.startuppurge == 1) { bitWrite(HardwareStatus, 30, 1); }

				sensor3.purgestatus = modbusRTU.getResponseBuffer(2);  //0x504(1285)
				if (sensor3.purgestatus == 0) { bitWrite(HardwareStatus, 31, 0); }
				else if (sensor3.purgestatus == 1) { bitWrite(HardwareStatus, 31, 1); }

				modbusRTU.u8MBStatus = ku8MBDone;
			}

			else if (modbusRTU.u8MBStatus != ku8MBWaiting) {
				modbusRTU.print_the_error((const uint8_t)12);
				if (bitRead(faultBITS, 14) == 0 && bitRead(HardwareStatus, 4) == 1) { bitWrite(faultBITS, 14, 1); }//FLAG sensor 3 COMM fault	
				bitWrite(HardwareStatus, 31, 0); //FLAG sensor 3 purging status OFF
				modbusRTU.u8MBStatus = ku8MBDone;
			}

			if (modbusRTU.u8MBStatus == ku8MBDone) {
				polltimeout = millis();
				pollSequence++;
			}

			break;

		case 35: //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

			if ((millis() - polltimeout) >= BETWEENPOLLTIME) { pollSequence++; } //TIME DELAY BETWEEN POLLS

			if (millis() < polltimeout) { polltimeout = millis(); } //reset in case ul_tickcount rolls over to 0

			break;

		case 36: //END OF SEQUENCE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

			pollSequence = 0;
		}
	}

}

