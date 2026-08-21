//
//
//

#include "HL7650response_600P.h"

HL7650responseClass HL7650response;

void HL7650responseClass::init()
{
}

void HL7650responseClass::ProcessModemResponse()
{

	if (HL7650.modem_initial_response_seen == 0)
	{
		HL7650.modem_initial_response_seen = 1;
	} // flag modem URC startup seen

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if (strstr(HL7650.Modem_buffer, "ERROR\r"))
	{

		HL7650.modemreadyfornextcommand = 1;
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if (strstr(HL7650.Modem_buffer, "OK\r"))
	{

		if (HL7650.reset_required == 1)
		{
			HL7650.modem_reset();
		}

		if (HL7650.ModemCommandStep == ATK3)
		{
			HL7650.modemresponsereceived = 1;
		} // ATK3
		if (HL7650.ModemCommandStep == ATE0)
		{
			HL7650.modemresponsereceived = 1;
		} // ATE0
		if (HL7650.ModemCommandStep == ATKSREPmodify)
		{
			HL7650.modemresponsereceived = 1;
		} // ATKSREP
		if (HL7650.ModemCommandStep == ATCREGmodify)
		{
			HL7650.modemresponsereceived = 1;
		} // ATCREG
		if (HL7650.ModemCommandStep == ATW0)
		{
			HL7650.modemresponsereceived = 1;
		} // ATW0
		if (HL7650.ModemCommandStep == ATWMANTSEL_modify)
		{
			HL7650.modemresponsereceived = 1;
		} // ATWMANTSEL
		if (HL7650.ModemCommandStep == ATCMEEmodify)
		{
			HL7650.modemresponsereceived = 1;
		} // ATCMEEmodify
		if (HL7650.ModemCommandStep == ATCGDCONTmodify)
		{
			HL7650.modemresponsereceived = 1;
		} // ATCGDCONTmodify
		if (HL7650.ModemCommandStep == ATCGACTactivate)
		{
			HL7650.modemresponsereceived = 1;
		} // ATCGACTactivate
		if (HL7650.ModemCommandStep == ATKCNXCFGmodify)
		{
			HL7650.modemresponsereceived = 1;
		} // ATCGACTactivate
		if (HL7650.ModemCommandStep == ATKCNXTIMERmodify)
		{
			HL7650.modemresponsereceived = 1;
		} // ATKCNXTIMERmodify
	}

	////%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// if (strstr(HL7650.Modem_buffer, "AT&K3\r")) {
	//	//if (ModemCommandStep%10) { ModemCommandStep += 10 - ModemCommandStep%10; } //proceed to next command
	//	HL7650.ModemCommandStep++;
	// }

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// if (strstr(HL7650.Modem_buffer, "ATE0\r")) {
	// if (ModemCommandStep%10) { ModemCommandStep += 10 - ModemCommandStep%10; } //proceed to next command
	//	HL7650.ModemCommandStep++;
	//}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if (strstr(HL7650.Modem_buffer, "+KSREP:"))
	{
		strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)

		/* crop off the echoed part of modem response */
		uint16_t char_offset = strstr(PROCESSING_buffer, "+KSREP:") - PROCESSING_buffer;

		strtok(PROCESSING_buffer + char_offset, ":,");

		uint8_t act = 0;
		act = atoi(strtok(NULL, ":,"));

		uint8_t stat = 0;
		stat = atoi(strtok(NULL, ":,"));

		if (act == 1)
		{
			HL7650.modemresponsereceived = 1;
		}

		else
		{
			HL7650.modemresponsereceived = 2;
		} // ENABLE URC NOTIFICATION

		printstartupURCstatus(stat);
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if (strstr(HL7650.Modem_buffer, "+CREG:") != NULL)
	{
		if (debugEN == 1)
		{
			DEBUG.println(F("CREG DETECTED"));
		}
		strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)

		int16_t location[10] = {0};
		uint8_t noofmessages = 0;

		while (location[noofmessages] >= 0)
		{
			location[noofmessages] = 0;
			if (noofmessages == 0)
			{
				location[noofmessages] = strstr(PROCESSING_buffer + (location[noofmessages]), "+CREG:") - PROCESSING_buffer;
			}
			else
			{
				location[noofmessages] = strstr(PROCESSING_buffer + (location[noofmessages - 1] + 1), "+CREG:") - PROCESSING_buffer;
			}
			if (location[noofmessages] > 0)
			{
				noofmessages++;
			}
		}

		if (debugEN == 1)
		{
			DEBUG.print(F("NUMBER OF CREG MESSAGES: "));
			DEBUG.println(noofmessages);
		}

		if (strstr(PROCESSING_buffer, "OK\r") != NULL && HL7650.ModemCommandStep == ATCREG)
		{ // this message is not unsolicited.
			if (debugEN == 1)
			{
				DEBUG.println(F("REQUESTED CREG SEEN"));
			}
			strtok(PROCESSING_buffer + location[noofmessages - 1], ":");
			uint8_t cgregmode = 0;
			cgregmode = atoi(strtok(NULL, ":,"));
			HL7650.networkregistrationstatus = atoi(strtok(NULL, ":,"));

			if (HL7650.networkregistrationstatus == 1 || HL7650.networkregistrationstatus == 5)
			{ // only if registered or registered roaming.
				HL7650.networklocationcode = strtol(strtok(NULL, "\","), NULL, 16);
				HL7650.networkcellid = strtol(strtok(NULL, "\","), NULL, 16);
				HL7650.networkAcT = atoi(strtok(NULL, ":,"));
			}

			if (cgregmode == 2)
			{
				HL7650.modemresponsereceived = 1;
				if (debugEN == 1)
				{
					DEBUG.println(F("NETWORK REGISTRATION URC ENABLED"));
				}
			}

			else
			{
				HL7650.modemresponsereceived = 2;
			} // ENABLE URC NOTIFICATION
		}

		else
		{ // message is unsolicited.

			if (debugEN == 1)
			{
				DEBUG.println(F("UNSOLICITED CREG SEEN"));
			}

			strtok(PROCESSING_buffer + location[noofmessages - 1], ":");

			uint8_t stat = atoi(strtok(NULL, ":,"));

			if (debugEN == 1)
			{
				DEBUG.print(F("networkregistrationstatus "));
				DEBUG.println(stat);
			}

			if (stat == 1 || stat == 5)
			{ // only if registered or registered roaming.
				HL7650.networklocationcode = strtol(strtok(NULL, "\","), NULL, 16);
				HL7650.networkcellid = strtol(strtok(NULL, "\","), NULL, 16);
				HL7650.networkAcT = atoi(strtok(NULL, ":,"));
			}

			else if (HL7650.networkregistrationstatus == 1 || HL7650.networkregistrationstatus == 5)
			{ // if modem was previously registered to the network
				if (debugEN == 1)
				{
					DEBUG.println(F("Network DISCONNECT!"));
				}
				SDcard.logEVENT("Network DISCONNECT!");
			}

			// if (stat == 0) { //check COPS config
			//	HL7650.ModemCommandStep = 30;
			// }

			// if (networkregistrationstatus == 0 && ModemCommandStep > 800) { poweroff_modem(); }
			HL7650.networkregistrationstatus = stat;
			printnetworkstatus();
		}
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if (strstr(HL7650.Modem_buffer, "+WMANTSEL: 11"))
	{
		HL7650.modemresponsereceived = 1;
	}

	if (strstr(HL7650.Modem_buffer, "+WMANTSEL: 00"))
	{
		HL7650.modemresponsereceived = 2;
	}

	// unsolicited notification %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if (strstr(HL7650.Modem_buffer, "+KSUP:"))
	{
		strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)

		/* crop off the echoed part of modem response */
		uint16_t char_offset = strstr(PROCESSING_buffer, "+KSUP:") - PROCESSING_buffer;

		strtok(PROCESSING_buffer + char_offset, ":,");

		uint8_t stat = 0;
		stat = atoi(strtok(NULL, ":,"));

		printstartupURCstatus(stat);

		/*	stat
			0 The module is ready to receive commands for the TE.No access code is required
			1 The module is waiting for an access code.Use AT + CPIN ? to determine the code
			2 The SIM card is not present
			3 The module is in �SIM lock� state
			4 Unrecoverable error
			5 Unknown state
		*/
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if (strstr(HL7650.Modem_buffer, "+COPS:"))
	{
		strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)

		/* crop off the echoed part of modem response */
		uint16_t char_offset = strstr(PROCESSING_buffer, "+COPS:") - PROCESSING_buffer;

		strtok(PROCESSING_buffer + char_offset, ":,");

		uint8_t copsmode = 0;
		copsmode = atoi(strtok(NULL, ":,"));

		if (copsmode == 0)
		{
			HL7650.modemresponsereceived = 1;
		}

		else
		{
			HL7650.modemresponsereceived = 2;
		} // change cops value
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if (strstr(HL7650.Modem_buffer, "+CPIN:"))
	{

		if (strstr(HL7650.Modem_buffer, "READY"))
		{
			HL7650.modemresponsereceived = 1;
		} // proceed to next command.

		if (debugEN == 1)
		{
			if (strstr(HL7650.Modem_buffer, "SIM PIN"))
			{
				DEBUG.println(F("SIM PIN required\r\n"));
			}
			if (strstr(HL7650.Modem_buffer, "SIM PUK"))
			{
				DEBUG.println(F("SIM PUK required\r\n"));
			}
			if (strstr(HL7650.Modem_buffer, "SIM PIN2"))
			{
				DEBUG.println(F("SIM PIN2 required\r\n"));
			}
			if (strstr(HL7650.Modem_buffer, "SIM PUK2"))
			{
				DEBUG.println(F("SIM PUK2 required\r\n"));
			}
			if (strstr(HL7650.Modem_buffer, "PH-SIM PIN"))
			{
				DEBUG.println(F("phone-to-SIM password required\r\n"));
			}
			if (strstr(HL7650.Modem_buffer, "PH-NET PIN"))
			{
				DEBUG.println(F("network personalization password required\r\n"));
			}
		}
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if (strstr(HL7650.Modem_buffer, "+CMEE:"))
	{

		strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)

		/* crop off the echoed part of modem response */
		uint16_t char_offset = strstr(PROCESSING_buffer, "+CMEE:") - PROCESSING_buffer;

		strtok(PROCESSING_buffer + char_offset, ":,");

		uint8_t cmeemode = 0;
		cmeemode = atoi(strtok(NULL, ","));

		if (cmeemode == 1)
		{
			HL7650.modemresponsereceived = 1;
		}

		else
		{
			HL7650.modemresponsereceived = 2;
		} // change cops value
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if (strstr(HL7650.Modem_buffer, "+CGDCONT:") != NULL)
	{

		if (debugEN == 1)
		{
			DEBUG.println(F("+CGDCONT: DETECTED"));
		}
		strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)

		int16_t location[10] = {0};
		uint8_t noofmessages = 0;
		uint8_t check = 0;

		while (location[noofmessages] >= 0)
		{
			if (noofmessages == 0)
			{
				location[noofmessages] = strstr(PROCESSING_buffer + (location[noofmessages]), "+CGDCONT:") - PROCESSING_buffer;
			}
			else
			{
				location[noofmessages] = strstr(PROCESSING_buffer + (location[noofmessages - 1] + 1), "+CGDCONT:") - PROCESSING_buffer;
			}
			if (location[noofmessages] > 0)
			{
				noofmessages++;
			}
		}

		if (debugEN == 1)
		{
			DEBUG.print(F("NUMBER OF +CGDCONT: MESSAGES: "));
			DEBUG.println(noofmessages);
		}

		uint8_t cnx_cfg = 0;
		char apn[64] = {'\0'};
		char ip[16] = {'\0'};
		uint8_t IPv4AddrAlloc = 0;

		while (check < noofmessages)
		{

			strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)
			strtok(PROCESSING_buffer + location[check], ":");
			cnx_cfg = atoi(strtok(NULL, ","));

			if (cnx_cfg == HL7650.cid)
			{

				strtok(NULL, ","); // PDP_type>
				strncpy(apn, strtok(NULL, ","), sizeof(apn));
				strncpy(ip, strtok(NULL, ","), sizeof(ip));
				strtok(NULL, ",");						 // d_comp
				strtok(NULL, ",");						 // h_comp
				IPv4AddrAlloc = atoi(strtok(NULL, ",")); // IPv4AddrAlloc

				strncpy(apn, strtok(apn, "\""), sizeof(apn)); // remove ""
				strncpy(ip, strtok(ip, "\""), sizeof(ip));	  // remove ""

				if (strstr("0.0.0.0", "0.0.0.0") != NULL)
				{ // dynamic IP

					if (strstr(HL7650.APN_name, apn) != NULL)
					{
						HL7650.modemresponsereceived = 1;
					}

					else
					{
						HL7650.modemresponsereceived = 2;
					} // change parameters
				}

				else
				{ // fixed IP

					if (strstr(HL7650.APN_name, apn) != NULL &&
						strstr(HL7650.ip_address, ip) != NULL)
					{
						HL7650.modemresponsereceived = 1;
					}

					else
					{
						HL7650.modemresponsereceived = 2;
					} // change parameters
				}

				if (debugEN == 1)
				{

					DEBUG.print(F("cnxcfg: "));
					DEBUG.println(cnx_cfg);
					DEBUG.print(F("apn: "));
					DEBUG.println(apn);
					DEBUG.print(F("ip: "));
					DEBUG.println(ip);
					DEBUG.print(F("IPv4AddrAlloc: "));
					DEBUG.println(IPv4AddrAlloc);
				}
			}

			check++;
		}
	}

	/*
	if (strstr(HL7650.Modem_buffer, "+CGDCONT:")) {

		char expected_response[128] = { '\0' };
		snprintf(expected_response, 128, "+CGDCONT: %d,\"IP\",\"%s\"", HL7650.cid, HL7650.APN_name);

		// crop off the echoed part of modem response
		uint16_t char_offset = strstr(HL7650.Modem_buffer, "+CGDCONT:") - HL7650.Modem_buffer;

		if (strncmp(HL7650.Modem_buffer + char_offset, expected_response, strlen(expected_response)) == 0) {
			//if (HL7650.ModemCommandStep % 10) { HL7650.ModemCommandStep += 10 - HL7650.ModemCommandStep % 10; } //proceed to next command. (roundup to nearest 10)
			//HL7650.ModemCommandStep += 10; //bypass CFUN & WPPPP
			HL7650.ModemCommandStep = 90;
			if (debugEN == 1) { DEBUG.println(F("+CGDCONT: MATCH")); }
		}

		else {
			HL7650.ModemCommandStep++; //change +CGDCONT parameters
		}

	}
*/
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if (strstr(HL7650.Modem_buffer, "+CFUN:"))
	{ // check cfun is back to normal after changing +WPPP settings

		strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)

		/* crop off the echoed part of modem response */
		uint16_t char_offset = strstr(PROCESSING_buffer, "+CFUN:") - PROCESSING_buffer;

		strtok(PROCESSING_buffer + char_offset, ":,");

		uint8_t cfun = 0;
		cfun = atoi(strtok(NULL, ":,"));

		if (cfun == 1)
		{
			// HL7650.ModemCommandStep += 3;  //proceed to +WPPP
		}

		// else { HL7650.ModemCommandStep++; } //change cfun value
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if (strstr(HL7650.Modem_buffer, "+WPPP:"))
	{

		// strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); //copy response so there is an intact message to process below. (strtok breaks the message up)

		/* crop off the echoed part of modem response */
		// uint16_t char_offset = strstr(PROCESSING_buffer, "+CMEE:") - PROCESSING_buffer;

		HL7650.modemresponsereceived = 1;

		// printf("modemres: %s\r\n",Modem_buffer+char_offset);
		/*
		if (modem.Auth == NONE) {

			snprintf(expected_response, 256, "+WPPP: %d",NONE);

			if(strncmp(Modem_buffer+char_offset,expected_response,strlen(expected_response)) == 0) {
				if (ModemCommandStep%10) { ModemCommandStep += 10 - ModemCommandStep%10; } //proceed to next command. (roundup to nearest 10)
			}

			else { ModemCommandStep++; } //change +WPPP parameters

			//printf("expecres: %s\r\n",expected_response);
		}

		if (modem.Auth == PAP) {

			snprintf(expected_response, 256, "+WPPP: %d,%d,\"%s\",\"%s\"",PAP,cid,modem.APN_login,modem.APN_password);

			if(strncmp(Modem_buffer+char_offset,expected_response,strlen(expected_response)) == 0) {
				if (ModemCommandStep%10) { ModemCommandStep += 10 - ModemCommandStep%10; } //proceed to next command. (roundup to nearest 10)
			}

			else { ModemCommandStep++; } //change +WPPP parameters

				//printf("expecres: %s\r\n",expected_response);
		}

		if (modem.Auth == CHAP) {

			snprintf(expected_response, 256, "+WPPP: %d,%d,\"%s\",\"%s\"",CHAP,cid,modem.APN_login,modem.APN_password);

			if(strncmp(Modem_buffer+char_offset,expected_response,strlen(expected_response)) == 0) {
			if (ModemCommandStep%10) { ModemCommandStep += 10 - ModemCommandStep%10; } //proceed to next command. (roundup to nearest 10)
			}

			else { ModemCommandStep++; } //change +WPPP parameters

				//printf("expecres: %s\r\n",expected_response);
		}
		*/
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if (strstr(HL7650.Modem_buffer, "+CGACT:") != NULL)
	{

		if (debugEN == 1)
		{
			DEBUG.println(F("+CGACT: DETECTED"));
		}
		strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)

		int16_t location[10] = {0};
		uint8_t noofmessages = 0;
		uint8_t check = 0;

		while (location[noofmessages] >= 0)
		{
			if (noofmessages == 0)
			{
				location[noofmessages] = strstr(PROCESSING_buffer + (location[noofmessages]), "+CGACT:") - PROCESSING_buffer;
			}
			else
			{
				location[noofmessages] = strstr(PROCESSING_buffer + (location[noofmessages - 1] + 1), "+CGACT:") - PROCESSING_buffer;
			}
			if (location[noofmessages] > 0)
			{
				noofmessages++;
			}
		}

		if (debugEN == 1)
		{
			DEBUG.print(F("NUMBER OF +CGACT MESSAGES: "));
			DEBUG.println(noofmessages);
		}

		while (check < noofmessages)
		{

			strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)
			strtok(PROCESSING_buffer + location[check], ":");

			HL7650.cid2 = atoi(strtok(NULL, ","));
			uint8_t newSTATE = atoi(strtok(NULL, ","));

			if (HL7650.cid2 == HL7650.cid)
			{

				HL7650.cid = HL7650.cid2;

				if (debugEN == 1)
				{
					DEBUG.println("cid match\r\n");
				}

				if (newSTATE == 1)
				{
					if (debugEN == 1)
					{
						DEBUG.println("cid is already connected\r\n");
					}
					HL7650.modemresponsereceived = 1;
				}

				else
				{
					if (debugEN == 1)
					{
						DEBUG.println("cid requires connection\r\n");
					}
					// if (HL7650.ModemCommandStep % 10) { HL7650.ModemCommandStep += 10 - HL7650.ModemCommandStep % 10; } //proceed to next command. (roundup to nearest 10)
					HL7650.modemresponsereceived = 2;
				}
			}

			else if (newSTATE == 0 && HL7650.cid2 != 5 && HL7650.cid2 != 1)
			{ // delete the CID if disconnected (but not CID 5 which is used for sierra wireless  OTA)

				if (debugEN == 1)
				{
					DEBUG.print("Unwanted PDP context ");
					DEBUG.print(HL7650.cid2);
					DEBUG.println(" DELETING...");
				}

				HL7650.ModemCommandStep = ATCGDCONTdelete;
				check = noofmessages; // break from the loop to delete this PDP CID
			}

			check++;
		}
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if (strstr(HL7650.Modem_buffer, "+KCNXCFG:") != NULL || (HL7650.ModemCommandStep == ATKCNXCFG && strstr(HL7650.Modem_buffer, "OK")))
	{

		if (strstr(HL7650.Modem_buffer, "+KCNXCFG:") != NULL)
		{

			strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)

			int16_t location[10] = {0};
			uint8_t noofmessages = 0;
			uint8_t check = 0;

			while (location[noofmessages] >= 0)
			{
				if (noofmessages == 0)
				{
					location[noofmessages] = strstr(PROCESSING_buffer + (location[noofmessages]), "+KCNXCFG:") - PROCESSING_buffer;
				}
				else
				{
					location[noofmessages] = strstr(PROCESSING_buffer + (location[noofmessages - 1] + 1), "+KCNXCFG:") - PROCESSING_buffer;
				}
				if (location[noofmessages] > 0)
				{
					noofmessages++;
				}
			}

			uint8_t cnx_cfg = 0;
			char apn[64] = {'\0'};
			char login[64] = {'\0'};
			char password[64] = {'\0'};
			char af[8] = {'\0'};
			char ip[16] = {'\0'};
			char dns1[16] = {'\0'};
			char dns2[16] = {'\0'};
			uint8_t state = 0;

			/*	state
				0 Disconnected
				1 Connecting
				2 Connected
				3 Idle, down counting for disconnection
				4 Disconnecting
			*/

			while (check < noofmessages)
			{

				strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)
				strtok(PROCESSING_buffer + location[check], ":");
				cnx_cfg = atoi(strtok(NULL, ","));

				if (cnx_cfg == HL7650.cid)
				{

					strtok(NULL, ",");
					strncpy(apn, strtok(NULL, ","), sizeof(apn));
					strncpy(login, strtok(NULL, ","), sizeof(login));
					strncpy(password, strtok(NULL, ","), sizeof(password));
					strncpy(af, strtok(NULL, ","), sizeof(af));
					strncpy(ip, strtok(NULL, ","), sizeof(ip));
					strncpy(dns1, strtok(NULL, ","), sizeof(dns1));
					strncpy(dns2, strtok(NULL, ","), sizeof(dns2));
					state = atoi(strtok(NULL, ","));

					strncpy(apn, strtok(apn, "\""), sizeof(apn));				 // remove ""
					strncpy(login, strtok(login, "\""), sizeof(login));			 // remove ""
					strncpy(password, strtok(password, "\""), sizeof(password)); // remove ""
					strncpy(af, strtok(af, "\""), sizeof(af));					 // remove ""
					strncpy(ip, strtok(ip, "\""), sizeof(ip));					 // remove ""
					strncpy(dns1, strtok(dns1, "\""), sizeof(dns1));			 // remove ""
					strncpy(dns2, strtok(dns2, "\""), sizeof(dns2));			 // remove ""

					if (state == 0)
					{ // only attempt to change settings if disconnected!

						if (HL7650.Auth == NONE)
						{ // omit apn login and password if authentication is not needed.

							if (strstr(HL7650.APN_name, apn) != NULL &&
								strstr(HL7650.ip_address, ip) != NULL &&
								strstr(HL7650.dns_1, dns1) != NULL &&
								strstr(HL7650.dns_2, dns2) != NULL)
							{

								HL7650.modemresponsereceived = 1;
							}

							else
							{
								HL7650.modemresponsereceived = 2;
							} // change +KCNXCFG parameters
						}

						else
						{ // include apn login and password if authentication set.

							if (strstr(HL7650.APN_name, apn) != NULL &&
								strstr(HL7650.ip_address, ip) != NULL &&
								strstr(HL7650.dns_1, dns1) != NULL &&
								strstr(HL7650.dns_2, dns2) != NULL &&
								strstr(HL7650.APN_login, login) != NULL &&
								strstr(HL7650.APN_password, password) != NULL)
							{

								HL7650.modemresponsereceived = 1;
							}

							else
							{
								HL7650.modemresponsereceived = 2;
							} // change +KCNXCFG parameters
						}
					}

					else
					{
						HL7650.modemresponsereceived = 1;
					} // proceed to next command.

					if (debugEN == 1)
					{

						DEBUG.print(F("cnxcfg: "));
						DEBUG.println(cnx_cfg);
						DEBUG.print(F("apn: "));
						DEBUG.println(apn);
						DEBUG.print(F("login: "));
						DEBUG.println(login);

						DEBUG.print(F("password: "));
						DEBUG.println(password);
						DEBUG.print(F("af: "));
						DEBUG.println(af);
						DEBUG.print(F("ip: "));
						DEBUG.println(ip);

						DEBUG.print(F("dns1: "));
						DEBUG.println(dns1);
						DEBUG.print(F("dns2: "));
						DEBUG.println(dns2);
						DEBUG.print(F("state: "));
						DEBUG.println(state);
					}

					check = noofmessages;
				}

				else
				{
					check++;
				}
			}
		}

		else
		{ // configuration required!

			HL7650.modemresponsereceived = 2; // change +KCNXCFG parameters

			/*
			if (HL7650.Auth == NONE) { //omit apn login and password if authentication is not needed.
				snprintf(PROCESSING_buffer, 256, "+KCNXCFG: %d,\"GPRS\",\"%s\",\"\",\"\",\"%s\",\"%s\",\"%s\",\"%s\"",
					HL7650.cid, HL7650.APN_name, "IPV4", HL7650.ip_address, HL7650.dns_1, HL7650.dns_2);
			}

			else { //include apn login and password if authentication set.
				snprintf(PROCESSING_buffer, 256, "+KCNXCFG: %d,\"GPRS\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"",
					HL7650.cid, HL7650.APN_name, HL7650.APN_login, HL7650.APN_password, "IPV4", HL7650.ip_address, HL7650.dns_1, HL7650.dns_2);
			}
			*/
		}
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if (strstr(HL7650.Modem_buffer, "+KSLEEP:"))
	{

		strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)

		/* crop off the echoed part of modem response */
		uint16_t char_offset = strstr(PROCESSING_buffer, "+KSLEEP:") - PROCESSING_buffer;

		strtok(PROCESSING_buffer + char_offset, ":,");

		uint8_t sleepmode = 0;
		sleepmode = atoi(strtok(NULL, ":,"));

		if (sleepmode == 2)
		{
			HL7650.modemresponsereceived = 1; // proceed to next command.
		}

		else
		{
			HL7650.modemresponsereceived = 2;
		} // change cops value
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if (strstr(HL7650.Modem_buffer, "+CTZU:"))
	{

		strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)

		/* crop off the echoed part of modem response */
		uint16_t char_offset = strstr(PROCESSING_buffer, "+CTZU:") - PROCESSING_buffer;

		strtok(PROCESSING_buffer + char_offset, ":,");

		uint8_t ctzumode = 0;
		ctzumode = atoi(strtok(NULL, ":,"));

		if (ctzumode == 1)
		{
			HL7650.modemresponsereceived = 1; // proceed to next command.
		}

		else
		{
			HL7650.modemresponsereceived = 2;
		} // change cops value
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if (strstr(HL7650.Modem_buffer, "+CTZR:"))
	{

		strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)

		/* crop off the echoed part of modem response */
		uint16_t char_offset = strstr(PROCESSING_buffer, "+CTZR:") - PROCESSING_buffer;

		strtok(PROCESSING_buffer + char_offset, ":,");

		uint8_t timemode = 0;
		timemode = atoi(strtok(NULL, ":,"));

		if (timemode == 1)
		{
			HL7650.modemresponsereceived = 1; // proceed to next command.
		}

		else
		{
			HL7650.modemresponsereceived = 2;
		} // change cops value
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if (strstr(HL7650.Modem_buffer, "+CCLK:"))
	{

		int time_zone_offset;
		struct tm ti = {0};
		uint16_t char_offset = 0;

		strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)

		if (strstr(PROCESSING_buffer, "AT+CCLK?"))
		{ // take "AT+CCLK?" out of message if its not an unsolicited notification.
			char_offset = strstr(PROCESSING_buffer, "+CCLK:") - PROCESSING_buffer;
		}

		sscanf(PROCESSING_buffer + char_offset, "\r+CCLK: \"%d/%d/%d,%d:%d:%d+%d", &ti.tm_year, &ti.tm_mon, &ti.tm_mday, &ti.tm_hour, &ti.tm_min, &ti.tm_sec, &time_zone_offset);
		/*
				if (debugEN == 1) {
					DEBUG.print(F("YEAR: "));
					DEBUG.println(ti.tm_year);
					DEBUG.print(F("MONTH: "));
					DEBUG.println(ti.tm_mon);
					DEBUG.print(F("DAY: "));
					DEBUG.println(ti.tm_mday);
					DEBUG.print(F("HOUR: "));
					DEBUG.println(ti.tm_hour);
					DEBUG.print(F("MIN: "));
					DEBUG.println(ti.tm_min);
					DEBUG.print(F("SEC: "));
					DEBUG.println(ti.tm_sec);
					DEBUG.print(F("TZ: "));
					DEBUG.println(time_zone_offset);
				}
		*/
		/* Convert into Unix epoch timestamp */
		ti.tm_year += (2000 - 1900); // HL7650 gives year as YY, therefore we can just add 2000 to get the actual number of years.
		// final value is expressed in years since 1900.
		// printf("YEAR CHANGE: %d\r\n", ti.tm_year);
		ti.tm_mon -= 1; // Month range is 0 - 11 so need to subtract one.
		// printf("MONTH CHANGE: %d\r\n", ti.tm_mon);
		time_t timestamp = mktime(&ti);

		// eeram_long_write(TZ_addr, time_zone_offset); //store time zone offset in EERAM for local time display
		DS1338.writenvRAMlong(RTC0, 16, (int)time_zone_offset);
		// timezone is given as 15 minute intervals past UTC ie offset = 4 ==> UTC+1 (1 hour ahead)
		timestamp -= 3600 * (time_zone_offset / 4);

		// printf("timestamp: %ld\r\n", timestamp);

		// time_t current_unix_timestamp = hri_rtcmode0_read_COUNT_reg(RTC);  // Note that this variable is incremented in the systick ISR every second

		if (DS1338.epoch != (uint32_t)timestamp)
		{
			if (debugEN == 1)
			{
				DEBUG.println(F("UPDATING RTC WITH NETWORK TIME\r\n"));
			}
			// hri_rtcmode0_write_COUNT_reg(RTC, timestamp); //UPDATE RTC
			DS1338.setRTC(RTC0, timestamp);
			// current_unix_timestamp = timestamp;
		}

		else if (debugEN == 1)
		{
			DEBUG.println(F("RTC == NETWORK TIME. NO UPDATE REQUIRED.\r\n"));
		}

		if (HL7650.ModemCommandStep == ATCCLK)
		{
			HL7650.modemresponsereceived = 1; // proceed to next command.
		}
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if (strstr(HL7650.Modem_buffer, "+CTZE:"))
	{

		strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)

		/* crop off the echoed part of modem response */
		uint16_t char_offset = strstr(PROCESSING_buffer, "+CTZE:") - PROCESSING_buffer;

		strtok(PROCESSING_buffer + char_offset, ":,");

		HL7650.timezone = atoi(strtok(NULL, ":,"));
		HL7650.daylightsavingtime = atoi(strtok(NULL, ":,"));
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if (strstr(HL7650.Modem_buffer, "+KCNXTIMER:"))
	{

		char expected_response[256] = {'\0'};

		const uint16_t tim1 = 30;	   // 1 � 120 s (30 s by default). If the module fails to activate the PDP context, a timer of <tim1> will be started. When this timer expires, it will try to activate the PDP context again.
		const uint16_t nbtrial = 3;	   // Attempt times from1 � 4 (2 by default). The module will try to activate the PDP context for a maximum of <nbtrial> times.
		const uint16_t tim2 = 60;	   // 0 � 300s (60 s by default). 0 Deactivated (connection will not close by itself) For client sockets, module will try to connect to the server within <tim2>s; if <tim2> expires, it will give up the connection.
		const uint16_t idletime = 600; // 0 � 1800 s (30 s by default) When all sessions are closed, the idle timer starts with the idle time. When this timer expires, it will try to deactivate the PDP context. Before the timer expires, connecting any session will stop this timer and the PDP context is reused.

		snprintf(expected_response, 256, "+KCNXTIMER: %d,%d,%d,%d,%d", HL7650.cid, tim1, nbtrial, tim2, idletime);

		uint16_t char_offset = 0;
		strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)

		/* crop off the echoed part of modem response */
		if (strstr(PROCESSING_buffer, "+KCNXTIMER:"))
		{
			char_offset = strstr(PROCESSING_buffer, "+KCNXTIMER:") - PROCESSING_buffer;
		}

		if (strncmp(PROCESSING_buffer + char_offset, expected_response, strlen(expected_response)) == 0)
		{
			HL7650.modemresponsereceived = 1; // proceed to next command.
		}

		else
		{
			HL7650.modemresponsereceived = 2; // change +KCNXCFG parameters
		}
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if (strstr(HL7650.Modem_buffer, "+CTZV:"))
	{

		int time_zone_offset;
		struct tm ti = {0};
		uint16_t char_offset = 0;

		strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)

		char_offset = strstr(PROCESSING_buffer, "+CTZV:") - PROCESSING_buffer;

		sscanf(PROCESSING_buffer + char_offset, "\r+CTZV: +%d,\"%d/%d/%d,%d:%d:%d", &time_zone_offset, &ti.tm_year, &ti.tm_mon, &ti.tm_mday, &ti.tm_hour, &ti.tm_min, &ti.tm_sec);
		/*
		if (debugEN == 1) {
			DEBUG.print(F("YEAR: "));
			DEBUG.println(ti.tm_year);
			DEBUG.print(F("MONTH: "));
			DEBUG.println(ti.tm_mon);
			DEBUG.print(F("DAY: "));
			DEBUG.println(ti.tm_mday);
			DEBUG.print(F("HOUR: "));
			DEBUG.println(ti.tm_hour);
			DEBUG.print(F("MIN: "));
			DEBUG.println(ti.tm_min);
			DEBUG.print(F("SEC: "));
			DEBUG.println(ti.tm_sec);
			DEBUG.print(F("TZ: "));
			DEBUG.println(time_zone_offset);
		}
*/
		/* Convert into Unix epoch timestamp */
		ti.tm_year += (2000 - 1900); // HL7650 gives year as YY, therefore we can just add 2000 to get the actual number of years.
		// final value is expressed in years since 1900.
		// printf("YEAR CHANGE: %d\r\n", ti.tm_year);
		ti.tm_mon -= 1; // Month range is 0 - 11 so need to subtract one.
		// printf("MONTH CHANGE: %d\r\n", ti.tm_mon);
		time_t timestamp = mktime(&ti);

		// eeram_long_write(TZ_addr, time_zone_offset); //store time zone offset in EERAM for local time display
		DS1338.writenvRAMlong(RTC0, 16, (int)time_zone_offset);
		// timezone is given as 15 minute intervals past UTC ie offset = 4 ==> UTC+1 (1 hour ahead)
		timestamp -= 3600 * (time_zone_offset / 4);

		// printf("timestamp: %ld\r\n", timestamp);

		// time_t current_unix_timestamp = hri_rtcmode0_read_COUNT_reg(RTC);  // Note that this variable is incremented in the systick ISR every second

		if (DS1338.epoch != (uint32_t)timestamp)
		{
			if (debugEN == 1)
			{
				DEBUG.println(F("UPDATING RTC WITH NETWORK TIME\r\n"));
			}
			// hri_rtcmode0_write_COUNT_reg(RTC, timestamp); //UPDATE RTC
			DS1338.setRTC(RTC0, timestamp);
		}

		else if (debugEN == 1)
		{
			DEBUG.println(F("RTC == NETWORK TIME. NO UPDATE REQUIRED.\r\n"));
		}
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if (strstr(HL7650.Modem_buffer, "+KTCPCFG:") || (HL7650.ModemCommandStep == ATKTCPCFG && strstr(HL7650.Modem_buffer, "OK")))
	{

		if (strstr(HL7650.Modem_buffer, "+KTCPCFG:"))
		{

			strtok(HL7650.Modem_buffer, ":,");
			MQTT.mqtt_tcp_session_id = atoi(strtok(NULL, ":,"));

			if (MQTT.queue_size() > 0)
			{
				HL7650.modemresponsereceived = 1; // proceed to next command.
			}

			else
			{
				HL7650.modemresponsereceived = 3;
			}
		}

		else
		{
			HL7650.modemresponsereceived = 2;
		} // change +KTCPCFG: parameters
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if (strstr(HL7650.Modem_buffer, "+KCNX_IND:") != NULL)
	{

		strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)

		int16_t location[10] = {0};
		uint8_t noofmessages = 0;
		uint8_t check = 0;

		while (location[noofmessages] >= 0)
		{
			if (noofmessages == 0)
			{
				location[noofmessages] = strstr(PROCESSING_buffer + (location[noofmessages]), "+KCNX_IND:") - PROCESSING_buffer;
			}
			else
			{
				location[noofmessages] = strstr(PROCESSING_buffer + (location[noofmessages - 1] + 1), "+KCNX_IND:") - PROCESSING_buffer;
			}
			if (location[noofmessages] > 0)
			{
				noofmessages++;
			}
		}

		uint8_t cnx_cfg = 0;
		uint8_t status = 0;
		uint8_t attempt = 0;
		uint8_t idletime = 0;

		while (check < noofmessages)
		{

			strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)
			strtok(PROCESSING_buffer + location[check], ":");
			cnx_cfg = atoi(strtok(NULL, ","));
			status = atoi(strtok(NULL, ","));
			attempt = 0;
			idletime = 0;

			/*	status
				0 Disconnected due to network
				1 Connected
				2 Failed to connect, <tim1> timer is started if <attempt> is less than < nbtrail>
				3 Closed
				4 Connecting
				5 Idle time down counting started for disconnection
				6 Idle time down counting canceled
			*/

			if (status == 2 || status == 4)
			{
				attempt = atoi(strtok(NULL, ","));
				if (attempt >= 3)
				{
				}
			}

			if (status == 5)
			{
				idletime = atoi(strtok(NULL, ","));
			}

			if (debugEN == 1)
			{

				if (status == 0)
				{
					DEBUG.println(F("Disconnected due to network."));
				}
				if (status == 1)
				{
					DEBUG.println(F("Connected."));
				}
				if (status == 2)
				{
					DEBUG.println(F("Failed to connect, <tim1> timer is started if <attempt> is less than <nbtrail>."));
				}
				if (status == 3)
				{
					DEBUG.println(F("Closed."));
				}
				if (status == 4)
				{
					DEBUG.println(F("Connecting"));
				}
				if (status == 5)
				{
					DEBUG.println(F("Idle time down counting started for disconnection."));
				}
				if (status == 6)
				{
					DEBUG.println(F("Idle time down counting canceled."));
				}

				if (status == 2 || status == 4)
				{
					DEBUG.print(F("attempt "));
					DEBUG.println(attempt);
				}

				if (status == 5)
				{
					DEBUG.print(F("idletime "));
					DEBUG.println(idletime);
				}
			}

			check++;
		}
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if (strstr(HL7650.Modem_buffer, "+KTCP_IND:"))
	{

		strtok(HL7650.Modem_buffer, ":,");
		uint8_t tcp_session = atoi(strtok(NULL, ":,"));
		uint8_t tcp_status = atoi(strtok(NULL, ":,"));

		/*	tcp_status
			1 session is set up and ready for operation
		*/

		if ((tcp_session == MQTT.mqtt_tcp_session_id) && tcp_status == 1)
		{
			HL7650.modemresponsereceived = 1;
		}
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if (strstr(HL7650.Modem_buffer, "+KCGPADDR:") != NULL)
	{
		strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)

		/* crop off the echoed part of modem response */
		uint16_t char_offset = strstr(PROCESSING_buffer, "+KCGPADDR:") - PROCESSING_buffer;
		char IPaddress[32] = "\0";

		strtok(PROCESSING_buffer + char_offset, ":,\"");
		strtok(NULL, ":,\"");
		// strncpy(HL7650.ip_address, strtok(NULL, ":,"), 16);
		strncpy(IPaddress, strtok(NULL, ":,\""), 16);

		if (debugEN == 1)
		{
			DEBUG.print(F("IP: "));
			DEBUG.println(IPaddress);
		}

		if (strncmp(IPaddress, HL7650.ip_address, 16) != 0)
		{ // if IP's arent the same

			char tempstr[16];
			strncpy(tempstr, IPaddress, 16);
			uint8_t ip1 = atoi(strtok(tempstr, "."));
			uint8_t ip2 = atoi(strtok(NULL, "."));
			uint8_t ip3 = atoi(strtok(NULL, "."));
			uint8_t ip4 = atoi(strtok(NULL, "."));

			strncpy(HL7650.ip_address, IPaddress, 16);

			MQTT.queue_MQTT_update(ip1, "I1"); // add new value to MQTT send queue
			MQTT.queue_MQTT_update(ip2, "I2"); // add new value to MQTT send queue
			MQTT.queue_MQTT_update(ip3, "I3"); // add new value to MQTT send queue
			MQTT.queue_MQTT_update(ip4, "I4"); // add new value to MQTT send queue

			if (debugEN == 1)
			{
				DEBUG.println(F("IP has changed!"));
			}
		}

		HL7650.modemresponsereceived = 1; // proceed to next command.
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if (strstr(HL7650.Modem_buffer, "+CSQ:") != NULL)
	{
		strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)

		/* crop off the echoed part of modem response */
		uint16_t char_offset = strstr(PROCESSING_buffer, "+CSQ:") - PROCESSING_buffer;

		strtok(PROCESSING_buffer + char_offset, ":,");
		int8_t rssi = atoi(strtok(NULL, ":,"));
		uint8_t ber = atoi(strtok(NULL, ":,"));

		/*Parameters
		<rssi> Received signal strength indication
		0 -113 dBm or less
		1 � 30 -111 to -53 dBm
		31 -51 dBm or greater
		99 Not known or not detectable
		<ber> Integer type; channel bit error rate (in percent)
		0 � 7 As RXQUAL values in the table in 3GPP TS 45.008 [20] subclause 8.2.4
		99 Not known or not detectable
		*/

		if (rssi != 99)
		{
			if (rssi == 0)
			{
				rssi = -113;
			}
			else
			{
				rssi = -113 + (rssi * 2);
			}
		}

		if (debugEN == 1)
		{
			DEBUG.print(F("Signal strength "));
			DEBUG.println(rssi);
			DEBUG.print(F("Channel bit error rate "));
			DEBUG.println(ber);
		}

		if (rssi != HL7650.ntwkSIG)
		{
			if (debugEN == 1)
			{
				DEBUG.println(F("Signal strength has changed!"));
			}
			HL7650.ntwkSIG = rssi;
			MQTT.queue_MQTT_update(HL7650.ntwkSIG, "Sg"); // add new value to MQTT send queue
		}

		HL7650.modemresponsereceived = 1; // proceed to next command.
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if (strstr(HL7650.Modem_buffer, "+KBND:") != NULL)
	{
		strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)
		uint8_t simplifiedBAND = 0;

		/* crop off the echoed part of modem response */
		uint16_t char_offset = strstr(PROCESSING_buffer, "+KBND:") - PROCESSING_buffer;

		strtok(PROCESSING_buffer + char_offset, ":,");
		uint32_t BAND = atol(strtok(NULL, ":,"));

		if (BAND == 0)
		{
			simplifiedBAND = 0;
		}
		if (BAND == 2)
		{
			simplifiedBAND = 1;
		}
		if (BAND == 4)
		{
			simplifiedBAND = 2;
		}
		if (BAND == 10)
		{
			simplifiedBAND = 3;
		}
		if (BAND == 20)
		{
			simplifiedBAND = 4;
		}
		if (BAND == 40)
		{
			simplifiedBAND = 5;
		}
		if (BAND == 100)
		{
			simplifiedBAND = 6;
		}
		if (BAND == 800)
		{
			simplifiedBAND = 7;
		}
		if (BAND == 1000)
		{
			simplifiedBAND = 8;
		}
		if (BAND == 2000)
		{
			simplifiedBAND = 9;
		}
		if (BAND == 4000)
		{
			simplifiedBAND = 10;
		}
		if (BAND == 10000)
		{
			simplifiedBAND = 11;
		}
		if (BAND == 20000)
		{
			simplifiedBAND = 12;
		}
		if (BAND == 40000)
		{
			simplifiedBAND = 13;
		}
		if (BAND == 800000)
		{
			simplifiedBAND = 14;
		}
		if (BAND == 1000000)
		{
			simplifiedBAND = 15;
		}
		if (BAND == 2000000)
		{
			simplifiedBAND = 16;
		}

		if (debugEN == 1)
		{
			DEBUG.print(F("CURRENT NETWORK "));
			DEBUG.println(HL7650.netBands[simplifiedBAND]);
		}

		if (simplifiedBAND != HL7650.ntwkBAND)
		{
			if (debugEN == 1)
			{
				DEBUG.println(F("Network has changed!"));
			}
			HL7650.ntwkBAND = simplifiedBAND;
			MQTT.queue_MQTT_update(HL7650.ntwkBAND, "Bd"); // add new value to MQTT send queue
		}

		HL7650.modemresponsereceived = 1; // proceed to next command.
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if (strstr(HL7650.Modem_buffer, "+KTCP_DATA:"))
	{

		strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)

		/* crop off the echoed part of modem response */
		uint16_t char_offset = strstr(PROCESSING_buffer, "+KTCP_DATA:") - PROCESSING_buffer;

		strtok(PROCESSING_buffer + char_offset, ":,");
		uint8_t tcp_session = atoi(strtok(NULL, ":,"));
		HL7650.tcp_dataavailable = atoi(strtok(NULL, ":,"));

		if (tcp_session == MQTT.mqtt_tcp_session_id)
		{

			if (HL7650.ModemCommandStep == MQTTconnectSEND)
			{
				HL7650.modemresponsereceived = 1;
			}
			if (HL7650.ModemCommandStep == MQTTpublishSEND)
			{
				HL7650.modemresponsereceived = 1;
			}
			if (HL7650.ModemCommandStep == MQTTdisconnectSEND)
			{
				HL7650.modemresponsereceived = 1;
			}

			/*
			if (HL7650.ModemCommandStep == 191 || HL7650.ModemCommandStep == 221 || HL7650.ModemCommandStep == 251) {
				// 191 MQTT CONNECT
				if (HL7650.ModemCommandStep % 10) { HL7650.ModemCommandStep += 10 - HL7650.ModemCommandStep % 10; } //proceed to next command. (roundup to nearest 10)
			}

			else {
				if (debugEN == 1) { DEBUG.println(F("setting 300")); }
				HL7650.ModemCommandStep = 300;
			}
			*/
		}
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if (strstr(HL7650.Modem_buffer, "+KTCP_ACK:"))
	{

		strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)

		/* crop off the echoed part of modem response */
		uint16_t char_offset = strstr(PROCESSING_buffer, "+KTCP_ACK:") - PROCESSING_buffer;

		strtok(PROCESSING_buffer + char_offset, ":,");

		uint8_t tcp_session = atoi(strtok(NULL, ","));
		uint32_t ackedBytes = atoi(strtok(NULL, ","));
		uint32_t sentBytes = atoi(strtok(NULL, ","));

		if (tcp_session == MQTT.mqtt_tcp_session_id)
		{
			/*
			if (debugEN == 1) {
				DEBUG.print(F("Bytes sent to server: "));
				DEBUG.println(sentBytes);
				DEBUG.print(F("Bytes received by server: "));
				DEBUG.println(ackedBytes);
			}
			*/
		}

		if (ackedBytes == sentBytes)
		{
			// if (debugEN == 1) {
			//	DEBUG.println(F("DATA SEND OK"));
			// }
		}

		if (HL7650.ModemCommandStep == MQTTconnectSEND)
		{
			HL7650.modemresponsereceived = 1;
		} // advance to MQTTconnectRECV
		if (HL7650.ModemCommandStep == MQTTconnectRECV)
		{
			HL7650.modemresponsereceived = 101;
		}
		if (HL7650.ModemCommandStep == MQTTpublishSEND)
		{
			HL7650.modemresponsereceived = 1;
		} // advance to MQTTpublishRECV
		if (HL7650.ModemCommandStep == MQTTpublishRECV)
		{
			HL7650.modemresponsereceived = 101;
		}
		if (HL7650.ModemCommandStep == MQTTdisconnectSEND)
		{
			HL7650.modemresponsereceived = 1;
		} // advance to MQTTdisconnectRECV
		if (HL7650.ModemCommandStep == MQTTdisconnectRECV)
		{
			HL7650.modemresponsereceived = 101;
		}

		// if (HL7650.ModemCommandStep == 201 || HL7650.ModemCommandStep == 231 || HL7650.ModemCommandStep == 261) { HL7650.modemreadyfornextcommand = 1; }// ack data recieve complete
	}

	// NEW +KTCP_NOTIF: %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if (strstr(HL7650.Modem_buffer, "+KTCP_NOTIF:") != NULL)
	{

		strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)

		int16_t location[10] = {0};
		uint8_t noofmessages = 0;

		while (location[noofmessages] >= 0)
		{
			location[noofmessages] = 0;
			if (noofmessages == 0)
			{
				location[noofmessages] = strstr(PROCESSING_buffer + (location[noofmessages]), "+KTCP_NOTIF:") - PROCESSING_buffer;
			}
			else
			{
				location[noofmessages] = strstr(PROCESSING_buffer + (location[noofmessages - 1] + 1), "+KTCP_NOTIF:") - PROCESSING_buffer;
			}
			if (location[noofmessages] > 0)
			{
				noofmessages++;
			}
		}

		if (debugEN == 1)
		{
			DEBUG.print(F("NUMBER OF +KTCP_NOTIF MESSAGES: "));
			DEBUG.println(noofmessages);
		}

		strtok(PROCESSING_buffer + location[noofmessages - 1], ":");

		// HL7650.networkregistrationstatus = atoi(strtok(NULL, ":,"));
		uint8_t tcp_session = atoi(strtok(NULL, ","));
		uint8_t tcp_notif = atoi(strtok(NULL, ","));

		if (tcp_session == MQTT.mqtt_tcp_session_id)
		{
			printTCPerror(tcp_notif);
		}

		// HL7650.modemreadyfornextcommand = 1;
		if (HL7650.tcp_dataavailable == 0)
		{
			HL7650.ModemCommandStep = TCPdisconnect;
		} // initiate disconnect if no incoming data to be read.

		HL7650.modemresponsereceived = 1; // proceed to next command.

		/*	tcp_notif

			0 Network error
			1 No more sockets available; max.number already reached
			2 Memory problem
			3 DNS error
			4 TCP disconnection by the server or remote client
			5 TCP connection error
			6 Generic error
			7 Fail to accept client request�s
			8 Data sending is OK but KTCPSND was waiting more or less characters
			9 Bad session ID
			10 Session is already running
			11 All sessions are used
			12 Socket connection timeout error
			13 SSL connection error
			14 SSL initialization error

		*/
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if (strstr(HL7650.Modem_buffer, "+KTCPSTAT:"))
	{

		strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)

		/* crop off the echoed part of modem response */
		uint16_t char_offset = strstr(PROCESSING_buffer, "+KTCPSTAT:") - PROCESSING_buffer;

		strtok(PROCESSING_buffer + char_offset, ":,");

		MQTT.TCPsocketSTATUS = atoi(strtok(NULL, ","));
		int8_t tcp_notif = atoi(strtok(NULL, ","));
		uint32_t tcp_remaining_send_bytes = atoi(strtok(NULL, ","));
		uint32_t tcp_remaining_receive_bytes = atoi(strtok(NULL, ","));

		printsocketstatus(MQTT.TCPsocketSTATUS);
		if (tcp_notif != -1)
		{
			printTCPerror(tcp_notif);
		}

		if (debugEN == 1)
		{

			DEBUG.print(F("Remaining bytes in the socket buffer, waiting to be sent:"));
			DEBUG.println(tcp_remaining_send_bytes);
			DEBUG.print(F("Remaining bytes in the socket buffer, waiting to be recieved:"));
			DEBUG.println(tcp_remaining_receive_bytes);
		}

		if (MQTT.TCPsocketSTATUS == 0)
		{ // Socket not defined, use +KTCPCFG to create a TCP socket
			// if (debugEN == 1) { DEBUG.print(F("ERROR no action configured 0")); }
			SDcard.logEVENT("+KTCPSTAT: error 0");
		}

		if (MQTT.TCPsocketSTATUS == 1)
		{ // Socket is only defined but not used
			// if (debugEN == 1) { DEBUG.print(F("Action configured 1")); }
			// HL7650.ModemCommandStep = TCPdelete; //DELETE THE SESSION IF SOCKET IS CLOSED
		}

		if (MQTT.TCPsocketSTATUS == 2)
		{ // Socket is opening and connecting to the server, cannot be used
			// if (debugEN == 1) { DEBUG.print(F("ERROR no action configured 2")); }
			SDcard.logEVENT("+KTCPSTAT: error 2");
		}

		if (MQTT.TCPsocketSTATUS == 3)
		{ // Connection is up, socket can be used to send/receive data
			// HL7650.ModemCommandStep = TCPsocketCLOSE; //CLOSE THE SOCKET IF IT IS ACTIVE
			SDcard.logEVENT("+KTCPSTAT: error 3");
		}

		if (MQTT.TCPsocketSTATUS == 4)
		{ // Connection is closing, it cannot be used, wait for status 5
			// if (debugEN == 1) { DEBUG.print(F("ERROR no action configured 4")); }
			SDcard.logEVENT("+KTCPSTAT: error 4");
		}

		if (MQTT.TCPsocketSTATUS == 5)
		{ // Socket is closed
			// HL7650.ModemCommandStep = 820; //DELETE THE SESSION IF SOCKET IS CLOSED
			// if (debugEN == 1) { DEBUG.print(F("Action configured 5")); }
			// HL7650.ModemCommandStep = ATKTCPCFG;
		}

		HL7650.modemresponsereceived = 1; // proceed to next command.

		/* MQTT.TCPsocketSTATUS
		0 Socket not defined, use + KTCPCFG to create a TCP socket
		1 Socket is only defined but not used
		2 Socket is opening and connecting to the server, cannot be used
		3 Connection is up, socket can be used to send / receive data
		4 Connection is closing, it cannot be used, wait for status 5
		5 Socket is closed
		*/
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if (strstr(HL7650.Modem_buffer, "CONNECT"))
	{

		if (HL7650.ModemCommandStep == MQTTconnect)
		{
			HL7650.modemresponsereceived = 100;
		}
		if (HL7650.ModemCommandStep == MQTTpublish)
		{
			HL7650.modemresponsereceived = 100;
		}
		if (HL7650.ModemCommandStep == MQTTdisconnect)
		{
			HL7650.modemresponsereceived = 100;
		}
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	if (strstr(HL7650.Modem_buffer, "+CME ERROR:"))
	{

		strncpy(PROCESSING_buffer, HL7650.Modem_buffer, Modem_buffer_size); // copy response so there is an intact message to process below. (strtok breaks the message up)

		/* crop off the echoed part of modem response */
		uint16_t char_offset = strstr(PROCESSING_buffer, "+CME ERROR:") - PROCESSING_buffer;
		strtok(PROCESSING_buffer + char_offset, ":");
		uint16_t CMEcode = atoi(strtok(NULL, ":"));

		char tempbuffer[48] = {'\0'};
		snprintf(tempbuffer, 48, "%s:%d %s:%d", "CME ERROR", CMEcode, "ModemCommandStep", HL7650.ModemCommandStep);
		SDcard.logEVENT(tempbuffer);

		HL7650.ERROR_timeout = millis();
		HL7650.ERROR = true;

		HL7650command.complete(TCPdisconnect);
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
}

void HL7650responseClass::printTCPerror(uint8_t tcp_notif)
{
	if (debugEN == 1)
	{
		if (tcp_notif == 0)
		{
			DEBUG.println(F("Network error"));
		}
		if (tcp_notif == 1)
		{
			DEBUG.println(F("No more sockets available; max. number already reached"));
		}
		if (tcp_notif == 2)
		{
			DEBUG.println(F("Memory problem"));
		}
		if (tcp_notif == 3)
		{
			DEBUG.println(F("DNS error"));
		}
		if (tcp_notif == 4)
		{
			DEBUG.println(F("TCP disconnection by the server or remote client"));
		}
		if (tcp_notif == 5)
		{
			DEBUG.println(F("TCP connection error"));
		}
		if (tcp_notif == 6)
		{
			DEBUG.println(F("Generic error"));
		}
		if (tcp_notif == 7)
		{
			DEBUG.println(F("Fail to accept client requests"));
		}
		if (tcp_notif == 8)
		{
			DEBUG.println(F("Data sending is OK but +KTCPSND was waiting for more or less characters"));
		}
		if (tcp_notif == 9)
		{
			DEBUG.println(F("Bad session ID"));
		}
		if (tcp_notif == 10)
		{
			DEBUG.println(F("Session is already running"));
		}
		if (tcp_notif == 11)
		{
			DEBUG.println(F("All sessions are used"));
		}
		if (tcp_notif == 12)
		{
			DEBUG.println(F("Socket connection timeout error"));
		}
		if (tcp_notif == 13)
		{
			DEBUG.println(F("SSL connection error"));
		}
		if (tcp_notif == 14)
		{
			DEBUG.println(F("SSL initialization error"));
		}
	}
}

void HL7650responseClass::printnetworkstatus()
{
	if (debugEN == 1)
	{
		if (HL7650.networkregistrationstatus == 0)
		{
			DEBUG.println(F("Not registered, ME is not currently searching a new operator to register to."));
		}
		if (HL7650.networkregistrationstatus == 1)
		{
			DEBUG.println(F("Registered, home network."));
		}
		if (HL7650.networkregistrationstatus == 2)
		{
			DEBUG.println(F("Not registered, but ME is currently searching a new operator to register to."));
		}
		if (HL7650.networkregistrationstatus == 3)
		{
			DEBUG.println(F("Registration denied."));
		}
		if (HL7650.networkregistrationstatus == 4)
		{
			DEBUG.println(F("Unknown."));
		}
		if (HL7650.networkregistrationstatus == 5)
		{
			DEBUG.println(F("Registered, roaming."));
		}

		if (HL7650.networkregistrationstatus == 1 || HL7650.networkregistrationstatus == 5)
		{ // only if registered or registered roaming.
			if (HL7650.networkAcT == 0)
			{
				DEBUG.println(F(" GSM"));
			}
			if (HL7650.networkAcT == 7)
			{
				DEBUG.println(F(" E-UTRAN"));
			}
			if (HL7650.networkAcT == 9)
			{
				DEBUG.println(F(" E-UTRAN (NB-S1 mode)"));
			}
		}
	}
}

void HL7650responseClass::printsocketstatus(uint8_t statussocket)
{
	if (debugEN == 1)
	{
		if (statussocket == 0)
		{
			DEBUG.println(F("Socket not defined, use +KTCPCFG to create a TCP socket"));
		}
		if (statussocket == 1)
		{
			DEBUG.println(F("Socket is only defined but not used"));
		}
		if (statussocket == 2)
		{
			DEBUG.println(F("Socket is opening and connecting to the server, cannot be used"));
		}
		if (statussocket == 3)
		{
			DEBUG.println(F("Connection is up, socket can be used to send/receive data"));
		}
		if (statussocket == 4)
		{
			DEBUG.println(F("Connection is closing, it cannot be used, wait for status 5 (Socket is closed)"));
		}
		if (statussocket == 5)
		{
			DEBUG.println(F("Socket is closed"));
		}
	}
}

void HL7650responseClass::printstartupURCstatus(uint8_t URCstat)
{
	if (debugEN == 1)
	{
		if (URCstat == 0)
		{
			DEBUG.println(F("Module is ready to receive commands for the TE. No access code is required."));
		}
		if (URCstat == 1)
		{
			DEBUG.println(F("Module is waiting for an access code. (The AT+CPIN? command can be used used to determine it.)"));
		}
		if (URCstat == 2)
		{
			DEBUG.println(F("SIM card is not present"));
		}
		if (URCstat == 3)
		{
			DEBUG.println(F("Module is in �SIMlock� state"));
		}
		if (URCstat == 4)
		{
			DEBUG.println(F("Unrecoverable error"));
		}
		if (URCstat == 5)
		{
			DEBUG.println(F("Unknown state"));
		}
		if (URCstat == 6)
		{
			DEBUG.println(F("Inactive SIM"));
		}
	}
}
