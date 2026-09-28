// 
// 
// 

#include "HL7650_600P.h"

HL7650Class HL7650;

void HL7650Class::init()
{
	pinMode(11, OUTPUT); //MODEM REGULATOR ON/OFF
	pinMode(A7, OUTPUT); //MODEM RESET
	pinMode(12, OUTPUT); //MODEM ON/OFF
	pinMode(30, INPUT); //MODEM RI
	pinMode(31, OUTPUT); //MODEM RTS
	pinMode(MDM_CTS, INPUT); //MODEM CTS
	pinMode(34, INPUT); //MODEM 1.8V ON
	pinMode(50, INPUT); //MODEM GPIO4
	pinMode(MDM_DTR, OUTPUT); //MODEM DTR
	pinMode(35, INPUT); //MODEM DSR
	pinMode(40, INPUT); //MODEM DCD
	pinMode(51, INPUT); //MODEM GPIO10
	pinMode(41, INPUT); //MODEM GPIO6

	digitalWrite(11, HIGH); //ENABLE 3.7V REGULATOR

	modemUART.begin(115200); //open serial port for Modem

	sprintf(MODEM_EOF_PATTERN, "%s%s", MODEM_EOF_PATTERN_START, MODEM_EOF_PATTERN_END);

	snprintf(HL7650.APN_name, 13, "m2m");
	snprintf(HL7650.dns_1, 10, "0.0.0.0");
	snprintf(HL7650.dns_2, 10, "0.0.0.0");
	snprintf(HL7650.ip_address, 10, "0.0.0.0");
	HL7650.TCPport = 0;
	HL7650.Auth = NONE;

	HL7650.remoteTCPport = 1883;
	//changed the remote broker to kaaiot - jm 28.07.26
	snprintf(HL7650.remoteserver, 32, "mqtt.next.kaaiot.com");

	modem_powerON();
}

void HL7650Class::modem_powerON(void) {

	//gpio_set_pin_level(MDM_RESET,1);

	digitalWrite(12, HIGH); //turn on modem
	modemreadytimer = millis(); //reset timer
	//pin_pulse(12, 1, 3000); //pulse power pin for 3 seconds
	//modemsupervisiontimer = millis(); //reset timer
	modem_ready = 2; //initiate CTS timeout

}

void HL7650Class::modem_powerOFF(void) {


	modem_ready = 0;
	networkregistrationstatus = 0;
	modem_initial_response_seen = 0;
	ModemCommandStep = 0;
	modemfirstrun = 0;

	digitalWrite(12, LOW); //turn on modem
	if (debugEN == 1) { DEBUG.println(F("MODEM POWER IS OFF")); }

}

void HL7650Class::modem_reset(void) {

	if (debugEN == 1) {
		DEBUG.println(F("MODEM RESET INITIATED"));
	}
	//pin_pulse(A7, 1, 50); //pulse reset pin 100ms
	//pin_pulse(11, 0, 5000); //pulse power regulator pin for 5 seconds

	//modem_ready = 2; //initiate CTS timeout


	ModemCommandStep = 0;
	reset_required = 0;
	networkregistrationstatus = 0;
	modem_initial_response_seen = 0;
	modemfirstrun = 0;
	modemRESETactive = 1;
	modem_ready = 6;
}

void HL7650Class::read() {

	if (modem_ready == 1 && (networkregistrationstatus == 1 || networkregistrationstatus == 5)) { //only if registered or registered roaming and modem booted.
		if (modemfirstrun == 0) {
			ModemCommandStep = 1; //initiate config check
			modemfirstrun++;
		}
	}

	ModemReady();
	HL7650command.process();

	while (modemUART.available() > 0) {  // while there is unread data in the ring buffer

		if (DataMode == false) {

			Modem_buffer[recv_count] = modemUART.read();

			timeout_read = millis();

			switch (connectSTAT) {

			case 0:
				if (Modem_buffer[recv_count] == 'C') { connectSTAT++; }
				else { connectSTAT = 0; }
				break;

			case 1:
				if (Modem_buffer[recv_count] == 'O') { connectSTAT++; }
				else { connectSTAT = 0; }
				break;

			case 2:
				if (Modem_buffer[recv_count] == 'N') { connectSTAT++; }
				else { connectSTAT = 0; }
				break;

			case 3:
				if (Modem_buffer[recv_count] == 'N') { connectSTAT++; }
				else { connectSTAT = 0; }
				break;

			case 4:
				if (Modem_buffer[recv_count] == 'E') { connectSTAT++; }
				else { connectSTAT = 0; }
				break;

			case 5:
				if (Modem_buffer[recv_count] == 'C') { connectSTAT++; }
				else { connectSTAT = 0; }
				break;

			case 6:
				if (Modem_buffer[recv_count] == 'T') { connectSTAT++; }
				else { connectSTAT = 0; }
				break;

			case 7:
				if (Modem_buffer[recv_count] == '\r') { connectSTAT++; }
				else { connectSTAT = 0; }
				break;

			case 8:
				if (Modem_buffer[recv_count] == '\n') {
					if (MQTT.response_expected == true || HL7650.command_response_expected == true) {
						DataMode = true;
						data_count = 0;
						if (debugEN == 1) {
							DEBUG.println(F("DATAMODE ON!"));
						}
					}

					connectSTAT = 0;
				}
				else { connectSTAT = 0; }
				break;

			}
			recv_count++;
		}

		if (DataMode == true) {
			Modem_Data_Buffer[data_count] = modemUART.read();

			timeout_read = millis();;

			switch (eofSTAT) {

			case 0:
				if (Modem_Data_Buffer[data_count] == '-') { eofSTAT++; }
				else { eofSTAT = 0; }
				break;

			case 1:
				if (Modem_Data_Buffer[data_count] == '-') { eofSTAT++; }
				else { eofSTAT = 0; }
				break;

			case 2:
				if (Modem_Data_Buffer[data_count] == 'E') { eofSTAT++; }
				else { eofSTAT = 0; }
				break;

			case 3:
				if (Modem_Data_Buffer[data_count] == 'O') { eofSTAT++; }
				else { eofSTAT = 0; }
				break;

			case 4:
				if (Modem_Data_Buffer[data_count] == 'F') { eofSTAT++; }
				else { eofSTAT = 0; }
				break;

			case 5:
				if (Modem_Data_Buffer[data_count] == '-') { eofSTAT++; }
				else { eofSTAT = 0; }
				break;

			case 6:
				if (Modem_Data_Buffer[data_count] == '-') { eofSTAT++; }
				else { eofSTAT = 0; }
				break;

			case 7:
				if (Modem_Data_Buffer[data_count] == 'P') { eofSTAT++; }
				else { eofSTAT = 0; }
				break;

			case 8:
				if (Modem_Data_Buffer[data_count] == 'a') { eofSTAT++; }
				else { eofSTAT = 0; }
				break;

			case 9:
				if (Modem_Data_Buffer[data_count] == 't') { eofSTAT++; }
				else { eofSTAT = 0; }
				break;

			case 10:
				if (Modem_Data_Buffer[data_count] == 't') { eofSTAT++; }
				else { eofSTAT = 0; }
				break;

			case 11:
				if (Modem_Data_Buffer[data_count] == 'e') { eofSTAT++; }
				else { eofSTAT = 0; }
				break;

			case 12:
				if (Modem_Data_Buffer[data_count] == 'r') { eofSTAT++; }
				else { eofSTAT = 0; }
				break;

			case 13:
				if (Modem_Data_Buffer[data_count] == 'n') { eofSTAT++; }
				else { eofSTAT = 0; }
				break;

			case 14:
				if (Modem_Data_Buffer[data_count] == '-') { eofSTAT++; }
				else { eofSTAT = 0; }
				break;

			case 15:
				if (Modem_Data_Buffer[data_count] == '-') {
					DataMode = false;
					eofSTAT = 0;
					if (debugEN == 1) {
						DEBUG.println(F("DATAMODE OFF!"));
					}
				}
				break;

			}

			if (Modem_Data_Buffer[0] != 255) { data_count++; }

		}

		if ((millis() - timeout_read) >= 25) { //break from loop if read has taken too long
			if (debugEN == 1) { DEBUG.println("recieve timeout break"); }
			break;
		}

		if (recv_count > 255) { //Prevent Overflow
			if (debugEN == 1) { DEBUG.println("overflow break"); }
			timeout_read -= 1000; //force processing
			break;
		}

		if (data_count > 200 && eofSTAT == 0) { //Prevent Overflow (packet size 64 bytes) unless EOF pattern has started in which case let it run on. buffer has space for 128 bytes. occasionally this may create packets a few bytes bigger than the 64 byte packet.
			if (debugEN == 1) { DEBUG.println("datapacket break"); }
			timeout_read -= 1000; //force processing
			break;
		}


	}

	if ((recv_count > 0 || data_count > 0) && (millis() - timeout_read) >= 20) {

		processDATA();
		recv_count = 0;
		data_count = 0;
		connectSTAT = 0;
		eofSTAT = 0;
	}

}

void HL7650Class::processDATA() {

	if (data_count > 0) {

		static const uint8_t modem_eof[] = "--EOF--Pattern--";
		uint32_t received_bytes = data_count;
		if (received_bytes >= sizeof(modem_eof) - 1 &&
			memcmp(Modem_Data_Buffer + received_bytes - (sizeof(modem_eof) - 1),
				modem_eof, sizeof(modem_eof) - 1) == 0) {
			received_bytes -= sizeof(modem_eof) - 1;
		}
		if (tcp_dataavailable >= received_bytes) { tcp_dataavailable -= received_bytes; }
		else { tcp_dataavailable = 0; }
		if (debugEN == 1) {
			DEBUG.print(F("MQTT.response_expected: "));
			DEBUG.println(MQTT.response_expected);
		}
		if (MQTT.response_expected == true && received_bytes > 0) {


			MQTT.parse_mqtt_response(received_bytes, (char*)Modem_Data_Buffer);

			MQTT.response_expected = false;

		}

		if (debugEN == 1) {
			DEBUG.print("DATA RECEIVED BYTES ");
			DEBUG.println(received_bytes);

			uint8_t lengthd = 0;
			while (lengthd < received_bytes) {
				if (Modem_Data_Buffer[lengthd] < 16) { DEBUG.print(F("0")); }
				DEBUG.print(Modem_Data_Buffer[lengthd], HEX);
				DEBUG.print(F(" "));
				lengthd++;
			}
			DEBUG.println();
			DEBUG.println();

		}
		DataMode = false;
		
		if (HL7650.command_response_expected == true) {
/*
			//char copybuffer[256];
			//memcpy(copybuffer, Modem_Data_Buffer, 256);

			if (strstr((char*)Modem_Data_Buffer + 40, "heater_on")) {
				if (debugEN == 1) { DEBUG.println("HEATER ON COMMAND SEEN!"); }
				bitWrite(HardwareStatus, 22, 1);
				writeEEPROM(EEPROM0, 3, 1);
				DisplayWrite_OBJ(display_Winbutton, 33, 0xFFFF); //HIDE DISABLED BUTTON
				DisplayWrite_OBJ(display_Winbutton, 32, 0x00); //SHOW ENABLED BUTTON
			}


			if (strstr((char*)Modem_Data_Buffer + 40, "heater_off")) {
				if (debugEN == 1) { DEBUG.println("HEATER OFF COMMAND SEEN!"); }
				bitWrite(HardwareStatus, 22, 0);
				writeEEPROM(EEPROM0, 3, 0);
				DisplayWrite_OBJ(display_Winbutton, 32, 0xFFFF); //hide ENABLED BUTTON
				DisplayWrite_OBJ(display_Winbutton, 33, 0x00); //SHOW DISABLED BUTTON
			}

			if (strstr((char*)Modem_Data_Buffer + 40, "vacuum_on")) {
				if (debugEN == 1) { DEBUG.println("VACUUM ON COMMAND SEEN!"); }
				bitWrite(HardwareStatus, 24, 1);
				writeEEPROM(EEPROM0, 6, 1);
				startSEQUENCE = 1; //Enable pump start sequence
				DisplayWrite_OBJ(display_Winbutton, 35, 0xFFFF); //HIDE DISABLED BUTTON
				DisplayWrite_OBJ(display_Winbutton, 34, 0x00); //SHOW ENABLED BUTTON
			}

			if (strstr((char*)Modem_Data_Buffer + 40, "vacuum_off")) {
				if (debugEN == 1) { DEBUG.println("VACUUM OFF COMMAND SEEN!"); }
				bitWrite(HardwareStatus, 24, 0);
				writeEEPROM(EEPROM0, 6, 0);
				DisplayWrite_OBJ(display_Winbutton, 34, 0xFFFF); //hide ENABLED BUTTON
				DisplayWrite_OBJ(display_Winbutton, 35, 0x00); //SHOW DISABLED BUTTON
			}

			if (strstr((char*)Modem_Data_Buffer + 40, "reset_modem")) {
				reset_modem();
				if (debugEN == 1) { DEBUG.println("MODEM RESET!"); }
			}

			if (strstr((char*)Modem_Data_Buffer + 40, "main_reset")) {
				if (debugEN == 1) { DEBUG.println("CONTROLLER RESET!"); }
				rstc_start_software_reset(RSTC);
			}

			if (strstr((char*)Modem_Data_Buffer + 40, "setTemp")) {
				if (debugEN == 1) { DEBUG.println("NEW TEMP SETPOINT!"); }
				uint8_t NEW_TEMP = 0;
				uint16_t char_offset = strstr((char*)Modem_Data_Buffer + 20, "setTemp") - (char*)Modem_Data_Buffer;

				strtok((char*)Modem_Data_Buffer + char_offset, ":");
				NEW_TEMP = atoi(strtok(NULL, ":}"));

				if (NEW_TEMP < 101 && NEW_TEMP > 0) { //CHANGE THE SETPOINT
					oven_setpoint = NEW_TEMP;
					writeEEPROM(EEPROM0, 4, NEW_TEMP);
				}

				if (debugEN == 1) {
					DEBUG.print("NEW TEMP READ: ");
					DEBUG.println(NEW_TEMP);
				}

				if (currentFORM == 12) {
					snprintf(displayformattingstring, 6, "%d", oven_setpoint);
					DisplayWrite_STR(84, displayformattingstring);
				}
			}
*/
		}


		HL7650.command_response_expected = false;
	}

	if (recv_count > 0) {
		Modem_buffer[recv_count] = '\0';

		if (debugEN == 1) {
			//uint32_t PCNTER = recv_count + 14;
			DEBUG.print(F(">>response: "));
			DEBUG.println(Modem_buffer);

			strcat(Modem_buffer, "\r\n********************************\r\n");
			DEBUG.println(Modem_buffer);

		}

		HL7650response.ProcessModemResponse();
	}

}

void HL7650Class::ModemReady() { //MODEM BOOT DETECTION

	if (modem_ready == 2) {

		if ((millis() - modemreadytimer) >= 3000) { //Power pin timeout

			digitalWrite(12, LOW);
			modem_ready = 3;
		}

	}

	if (modem_ready == 3) { //RESET TIMER AND INITIATE FAIL COUNTDOWN
		modemreadytimer = millis(); //reset timer

		modem_ready = 4;
		if (debugEN == 1) {
			DEBUG.println(F("\r\nWAITING FOR CTS TO GO HIGH"));
		}
	}

	if (modem_ready == 4) {

		if ((millis() - modemreadytimer) >= 33000) { //WAIT FOR CTS PIN TO GO HIGH. 30 SECOND TIMEOUT

			if (debugEN == 1) {
				DEBUG.println(F("MODEM FAILED TO BOOT3"));
			}
			modem_reset();
		}

		if ((millis() - modemreadytimer) < 0) { //reset in case ul_tickcount rolls over to 0
			modemreadytimer = millis();
		}

		if (digitalRead(MDM_CTS) == 1) {
			if (debugEN == 1) {
				DEBUG.println(F("CTS IS HIGH!"));
			}
			modem_ready = 5;
		}
	}

	if (modem_ready == 5) { //WHEN CTS PIN GOES HIGH WAIT FOR IT TO GO LOW AGAIN. CONTINUATION OF 10 SECOND TIMEOUT ABOVE. SPEC SHEET SAYS MAX 9 SECOND BOOT TIME.

		if ((millis() - modemreadytimer) >= 15000) { //continue 15 second timeout

			if (debugEN == 1) {
				DEBUG.println(F("MODEM FAILED TO BOOT4"));
			}
			modem_reset();
		}

		if ((millis() - modemreadytimer) < 0) { //reset in case ul_tickcount rolls over to 0
			modemreadytimer = millis();
		}

		if (digitalRead(MDM_CTS) == 0) {
			if (debugEN == 1) {
				DEBUG.println(F("CTS IS LOW!\r\nMODEM READY"));
			}
			modem_ready = 1; //FLAG SET TO 1 TO INDICATE MODEM READY FOR AT COMMANDS
			HL7650.modemreadyfornextcommand = 1;
			HL7650.modem_response_timeout = millis(); //reset timeout counter
		}
	}

	if (modem_ready == 6) { //modem reset

		digitalWrite(11, LOW); //turn off modem regulator
		modem_ready = 7;
		modemreadytimer = millis();
	}

	if (modem_ready == 7) { //modem reset

		if ((millis() - modemreadytimer) >= 5000) {

			digitalWrite(11, HIGH); //turn off modem regulator
			modem_ready = 8;
		}

	}

	if (modem_ready == 8) { //modem reset

		if ((millis() - modemreadytimer) >= 1000) {

			if (modemRESETactive == 1) {
				modemRESETactive = 0;
			}

			modem_powerON();

		}

	}

}

void HL7650Class::set_last_send_time(uint32_t timestamp) {

	DS1338.writenvRAMlong(RTC0, 20, timestamp);

}
