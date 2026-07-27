
// 
// 
// 

#include "serialcommands_600P.h"

serialcommandsClass serialcommands;





void serialcommandsClass::init()
{


}

void serialcommandsClass::scan() {

	if (DEBUG.available() > 0) {
		DEBUGbuffer[count] = DEBUG.read();
		//if (ModemPassthrough == 1) { modemUART.write(DEBUGbuffer[count]); } //PASSES ALL DATA THROUGH TO MODEM
		//modemUART.write(DEBUGbuffer[count]);
		count++;
		if (count >= (127)) { count--; } //prevents overflow into unreserved memory spaces.
		DEBUGtimer = millis();
	}

	if (count > 0 && (millis() - DEBUGtimer >= 250)) { // 500 millisecond delay after last byte has arrived before processing.

		DEBUGbuffer[count] = '\0'; //TERMINATE STRING


		// BASIC PARAMETER LIST ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^	
		if (strcmp(DEBUGbuffer, basicPARA) == 0) {
			DEBUG.println();
			DEBUG.println(STARLINE);
			DEBUG.println();
			DEBUG.println(F("CURRENT CONFIGURATION..."));
			DEBUG.println();

			DEBUG.print(F("Model:			"));
			DEBUG.println(Model);

			DEBUG.print(F("PCB S/N:			"));
			DEBUG.println(PCBserialno);

			DEBUG.print(F("PCB Revision:		"));
			DEBUG.println(hardwareVersion);

			DEBUG.print(F("Firmware Version:		"));
			DEBUG.println(thisCODEversion);

			time_t rawtime = DS1338.epoch;
			struct tm  ts;
			char       buf[80];
		
			ts = *gmtime(&rawtime);
			strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S", &ts);
			DEBUG.print(F("System Clock GMT		"));
			DEBUG.println(buf);
			
			rawtime += 3600 * (DS1338.readnvRAMlong(RTC0,16) / 4); //add on timezone
			ts = *gmtime(&rawtime);

			strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S", &ts);
			DEBUG.print(F("System Clock Local	"));
			DEBUG.println(buf);

			DEBUG.print(F("Sensors Detected:	"));
			//DEBUG.println(numSensors);

			DEBUG.println();
			DEBUG.println(STARLINE);
			DEBUG.println();
			DEBUG.println(F("MODEM STATUS..."));
			DEBUG.println();

			DEBUG.print(F("Signal Strength:		"));
			if (HL7650.ntwkSIG != 0) { DEBUG.print(HL7650.ntwkSIG); }
			else { DEBUG.print(F("-")); }
			DEBUG.println(F(" dBm"));

			DEBUG.print(F("Network Band:		"));
			DEBUG.println(HL7650.netBands[HL7650.ntwkBAND]);

			DEBUG.print(F("IP Address:		"));
			DEBUG.println(HL7650.ip_address);

			DEBUG.print(F("Network Status:		"));
			DEBUG.print(HL7650.netStatus[HL7650.networkregistrationstatus]);
			if (HL7650.networkregistrationstatus == 1 || HL7650.networkregistrationstatus == 5) { DEBUG.println(HL7650.netType[HL7650.networkAcT]); }
			else { DEBUG.println(); }

			DEBUG.println();
			DEBUG.println(STARLINE);
			DEBUG.println();
			DEBUG.println(F("SENSORS..."));
			DEBUG.println();
			
						//if (numSensors > 0) {
							DEBUG.print(F("Sensor 1 S/N:		"));
							DEBUG.println(sensor1.serialNO);
						//}

						//if (numSensors > 1) {
							DEBUG.print(F("Sensor 2 S/N:		"));
							DEBUG.println(sensor2.serialNO);
						//}

						//if (numSensors > 2) {
							DEBUG.print(F("Sensor 3 S/N:		"));
							DEBUG.println(sensor3.serialNO);
						//}
			
			DEBUG.println();
			DEBUG.println(STARLINE);
			DEBUG.println();

			validCommand = true;
		}

		// ADVANCED PARAMETER LIST ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^	

		if (strcmp(DEBUGbuffer, advancedPARA) == 0) {
			DEBUG.println();
			DEBUG.println(STARLINE);
			DEBUG.println();
			DEBUG.println(F("ADVANCED CONFIGURATION..."));
			DEBUG.println();

			DEBUG.print(F("Network APN:	"));
			DEBUG.println();

			DEBUG.print(F("Active flashbank:	"));
			DEBUG.println(currentflashbank);

			DEBUG.print(F("RS485 enabled:	"));
			DEBUG.println(bitRead(HardwareStatus, 1));

			DEBUG.print(F("Last reset reason:	"));
			if (lastresetcause == GENERAL) { DEBUG.println(F("GENERAL")); }
			if (lastresetcause == BACKUP) { DEBUG.println(F("BACKUP")); }
			if (lastresetcause == WATCHDOG) { DEBUG.println(F("WATCHDOG")); }
			if (lastresetcause == SOFTWARE) { DEBUG.println(F("SOFTWARE")); }
			if (lastresetcause == USER) { DEBUG.println(F("USER")); }

			DEBUG.print(F("PCB temp:	"));
			DEBUG.print(MCP98244.PCBtemp);
			DEBUG.println(F(" C"));

			DEBUG.print(F("System uptime:	"));
			DEBUG.print((totaluptimeSECONDS % (86400 * 30)) / 86400);
			DEBUG.print(F("days "));
			DEBUG.print((totaluptimeSECONDS % 86400) / 3600);
			DEBUG.print(F("h "));
			DEBUG.print((totaluptimeSECONDS % 3600) / 60);
			DEBUG.print(F("m "));
			DEBUG.print(totaluptimeSECONDS % 60);
			DEBUG.println(F("s"));
			DEBUG.println();

			DEBUG.print(F("Program longest looptime:	"));
			DEBUG.print(looptime);
			DEBUG.println(F(" us"));

			DEBUG.print(F("faultBITS:	"));
			DEBUG.println(faultBITS, BIN);

			DEBUG.print(F("HardwareStatus:	"));
			DEBUG.println(HardwareStatus, BIN);

			DEBUG.print(F("response.ack_type:	"));
			DEBUG.println(response.ack_type);

			DEBUG.print(F("response.return_code:	"));
			DEBUG.println(response.return_code);

			DEBUG.print(F("HL7650.ModemCommandStep:	"));
			DEBUG.println(HL7650.ModemCommandStep);

			DEBUG.print(F("MQTT.queue_isEmpty():	"));
			DEBUG.println(MQTT.queue_isEmpty());

			DEBUG.println();
			DEBUG.println(STARLINE);
			DEBUG.println();

			validCommand = true;
		}

		// RS485 ON/OFF ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^	

		if (strcmp(DEBUGbuffer, RS485ON) == 0) {
			bitWrite(HardwareStatus, 1, 1);
			DEBUG.println(F("RS485 ENABLED"));
			EEPROM.write(EEPROM0, 2, bitRead(HardwareStatus, 1));
			validCommand = true;
		}

		if (strcmp(DEBUGbuffer, RS485OFF) == 0) {
			bitWrite(HardwareStatus, 1, 0);
			DEBUG.println(F("RS485 DISABLED"));
			EEPROM.write(EEPROM0, 2, bitRead(HardwareStatus, 1));
			digitalWrite(rs4851REDE, LOW);
			digitalWrite(rs4852REDE, LOW);
			digitalWrite(rs4853REDE, LOW);
			validCommand = true;
		}

		// DEBUG OUTPUT ON/OFF ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^	

		if (strcmp(DEBUGbuffer, debugEnablecommand) == 0) {
			debugEN = 1;
			DEBUG.println(F("SERIAL OUTPUT ENABLED"));
			DEBUG.println();
			EEPROM.write(EEPROM0, 0, debugEN);

			DEBUG.print(F("PCB SERIAL NO: "));
			DEBUG.println(EEPROM.readlong(EEPROM0, 13));
			DEBUG.println();

			//format_timestamp();
			//DEBUG.print(F("SYSTEM CLOCK: "));
			//DEBUG.println(timebuffer);
			//DEBUG.println();
			//DEBUG.println();
			validCommand = true;
		}

		if (strcmp(DEBUGbuffer, debugDisablecommand) == 0) {
			debugEN = 0;
			DEBUG.println(F("SERIAL OUTPUT DISABLED"));
			DEBUG.println();
			EEPROM.write(EEPROM0, 0, debugEN);
			validCommand = true;
		}

		// SHOW SENSOR VALUES ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^	

		if (strcmp(DEBUGbuffer, sensorVALUES) == 0) {

			DEBUG.println();
			DEBUG.println(F("Sensor 1"));
			DEBUG.println(F("---------------------------------------------------"));

			DEBUG.print(F("Temp: "));
			DEBUG.print(sensor1.T);
			DEBUG.println(F(" C"));
			DEBUG.print(F("Dewpoint: "));
			DEBUG.print(sensor1.Tdf);
			DEBUG.println(F(" C"));
			DEBUG.print(F("Dewpoint atm: "));
			DEBUG.print(sensor1.Tdfa);
			DEBUG.println(F(" C"));
			DEBUG.print(F("Moisture: "));
			DEBUG.print(sensor1.H2O);
			DEBUG.println(F(" ppm"));
			DEBUG.print(F("Pressure: "));
			DEBUG.print(sensor1.P);
			DEBUG.println(F(" mBara"));
			DEBUG.print(F("Pressure @20C: "));
			DEBUG.print(sensor1.Pnorm);
			DEBUG.println(F(" mBara"));
			DEBUG.print(F("Density: "));
			DEBUG.print(sensor1.Rhoo);
			DEBUG.println(F(" kg/m3"));
			DEBUG.print(F("Fault status: "));
			DEBUG.println(sensor1.faultstatus);
			DEBUG.print(F("Online status: "));
			DEBUG.println(sensor1.onlinestatus);
			DEBUG.print(F("Error code: "));
			DEBUG.println(sensor1.errorcode);
			DEBUG.print(F("Serial no: "));
			DEBUG.println(sensor1.serialNO);
			DEBUG.print(F("Auto purge: "));
			DEBUG.println(sensor1.autopurge);
			DEBUG.print(F("Startup purge: "));
			DEBUG.println(sensor1.startuppurge);
			DEBUG.print(F("Purge in progress: "));
			DEBUG.println(sensor1.purgestatus);
			DEBUG.println();

			DEBUG.println(F("Sensor 2"));
			DEBUG.println(F("---------------------------------------------------"));

			DEBUG.print(F("Temp: "));
			DEBUG.print(sensor2.T);
			DEBUG.println(F(" C"));
			DEBUG.print(F("Dewpoint: "));
			DEBUG.print(sensor2.Tdf);
			DEBUG.println(F(" C"));
			DEBUG.print(F("Dewpoint atm: "));
			DEBUG.print(sensor2.Tdfa);
			DEBUG.println(F(" C"));
			DEBUG.print(F("Moisture: "));
			DEBUG.print(sensor2.H2O);
			DEBUG.println(F(" ppm"));
			DEBUG.print(F("Pressure: "));
			DEBUG.print(sensor2.P);
			DEBUG.println(F(" mBara"));
			DEBUG.print(F("Pressure @20C: "));
			DEBUG.print(sensor2.Pnorm);
			DEBUG.println(F(" mBara"));
			DEBUG.print(F("Density: "));
			DEBUG.print(sensor2.Rhoo);
			DEBUG.println(F(" kg/m3"));
			DEBUG.print(F("Fault status: "));
			DEBUG.println(sensor2.faultstatus);
			DEBUG.print(F("Online status: "));
			DEBUG.println(sensor2.onlinestatus);
			DEBUG.print(F("Error code: "));
			DEBUG.println(sensor2.errorcode);
			DEBUG.print(F("Serial no: "));
			DEBUG.println(sensor2.serialNO);
			DEBUG.print(F("Auto purge: "));
			DEBUG.println(sensor2.autopurge);
			DEBUG.print(F("Startup purge: "));
			DEBUG.println(sensor2.startuppurge);
			DEBUG.print(F("Purge in progress: "));
			DEBUG.println(sensor2.purgestatus);
			DEBUG.println();

			DEBUG.println(F("Sensor 3"));
			DEBUG.println(F("---------------------------------------------------"));

			DEBUG.print(F("Temp: "));
			DEBUG.print(sensor3.T);
			DEBUG.println(F(" C"));
			DEBUG.print(F("Dewpoint: "));
			DEBUG.print(sensor3.Tdf);
			DEBUG.println(F(" C"));
			DEBUG.print(F("Dewpoint atm: "));
			DEBUG.print(sensor3.Tdfa);
			DEBUG.println(F(" C"));
			DEBUG.print(F("Moisture: "));
			DEBUG.print(sensor3.H2O);
			DEBUG.println(F(" ppm"));
			DEBUG.print(F("Pressure: "));
			DEBUG.print(sensor3.P);
			DEBUG.println(F(" mBara"));
			DEBUG.print(F("Pressure @20C: "));
			DEBUG.print(sensor3.Pnorm);
			DEBUG.println(F(" mBara"));
			DEBUG.print(F("Density: "));
			DEBUG.print(sensor3.Rhoo);
			DEBUG.println(F(" kg/m3"));
			DEBUG.print(F("Fault status: "));
			DEBUG.println(sensor3.faultstatus);
			DEBUG.print(F("Online status: "));
			DEBUG.println(sensor3.onlinestatus);
			DEBUG.print(F("Error code: "));
			DEBUG.println(sensor3.errorcode);
			DEBUG.print(F("Serial no: "));
			DEBUG.println(sensor3.serialNO);
			DEBUG.print(F("Auto purge: "));
			DEBUG.println(sensor3.autopurge);
			DEBUG.print(F("Startup purge: "));
			DEBUG.println(sensor3.startuppurge);
			DEBUG.print(F("Purge in progress: "));
			DEBUG.println(sensor3.purgestatus);
			DEBUG.println();
			DEBUG.println();

			validCommand = true;
		}
		



		// $$$$$$$$$$$$$ ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

		if (validCommand == false) {
			//if (ModemPassthrough == 0 && BTPassthrough == 0) {
				DEBUG.println(F("INVALID COMMAND!"));
				DEBUG.println();
			//}

		}
		count = 0;
		validCommand = 0;
	}

}


