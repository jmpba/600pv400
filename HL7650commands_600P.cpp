//
//
//

#include "HL7650commands_600P.h"

HL7650commandClass HL7650command;

void HL7650commandClass::init()
{
}

void HL7650commandClass::process(void)
{

	if (HL7650.modemreadyfornextcommand == 0)
	{

		if ((millis() - HL7650.modem_response_timeout) >= MODEM_RESPONSE_TIMOUT_MS)
		{
			if (debugEN == 1)
			{
				DEBUG.print(F("MODEM RESPONSE TIMEOUT\r\n"));
			}

			if (HL7650.modem_initial_response_seen == 0)
			{ // initiate URC report setting change if modem has not been configured for this.
				HL7650.ModemCommandStep = 1;
				HL7650.modem_response_timeout = millis(); // reset timeout counter
				// modem_initial_response_seen = 1;
			}

			HL7650.modemreadyfornextcommand = 1;
			HL7650.modemresponsereceived = 0;
		}

		if (millis() < HL7650.modem_response_timeout)
		{ // reset after rollover
			HL7650.modem_response_timeout = millis();
		}
	}

	if (HL7650.ERROR == true)
	{

		if ((millis() - HL7650.ERROR_timeout) >= 120000)
		{

			if (debugEN == 1)
			{
				DEBUG.print(F("ERROR TIMEOUT\r\n"));
			}

			// HL7650.ModemCommandStep = 34; //De-register from the network.
			// HL7650.ModemCommandStep = 9;

			HL7650.ERROR = false;
			HL7650.modem_reset();
		}

		if (millis() < HL7650.ERROR_timeout)
		{ // reset after rollover
			HL7650.ERROR_timeout = millis();
		}
	}

	if (HL7650.modem_ready == 1)
	{ // response from previous command must have be seen as-well as modem CTS pin low

		switch (HL7650.ModemCommandStep)
		{ // MODEM COMMANDS. SWITCH USED TO ALLOW PORCESSOR TO CARRY ON WITH OTHER TASKS WHILE WAITING FOR MODEM TO RESPOND.
			// COMMANDS ARE IN BLOCKS OF 10. IE FIRST COMMAND STARTS AT 10. THE NEXT 20. NUMBERS IN BETWEEN CAN BE USED TO CHANGE SETTINGS.

		case ATK3: // config check and URC enable %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("AT&K3");
			} // enable hardware flow control (RTS/CTS)

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATE0);
			}

			break;

		case ATE0: // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("ATE0");
			} // enable command echo

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATKSREP);
			}

			break;

		case ATKSREP: // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("AT+KSREP?");
			} // Start-up Reporting

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATCREG);
			}
			if (HL7650.modemresponsereceived == 2)
			{
				HL7650command.complete(ATKSREPmodify);
			}

			break;

		case ATKSREPmodify:

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("AT+KSREP=1");
			} // Modify Start-up Reporting
			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATKSREP);
			}

			break;

		case ATCREG: // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("AT+CREG?");
			} // query startup URC enabled

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATW0);
			}
			if (HL7650.modemresponsereceived == 2)
			{
				HL7650command.complete(ATCREGmodify);
			}

			break;

		case ATCREGmodify:

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("AT+CREG=2");
			} // Modify Start-up Reporting
			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(MODEMRESET);
			}

			break;

		case ATW0: // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("AT&W0");
			} // SAVE CONFIGURATION

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATWMANTSEL);
			}

			break;

		case ATWMANTSEL: // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("AT+WMANTSEL?");
			} // query startup URC enabled

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATCOPS);
			}
			if (HL7650.modemresponsereceived == 2)
			{
				HL7650command.complete(ATWMANTSEL_modify);
			}

			break;

		case ATWMANTSEL_modify:

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("AT+WMANTSEL=11");
			}
			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATWMANTSEL);
			}

			break;

		case ATCOPS: // COPS SELECTION %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("AT+COPS?");
			} // query COPS

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATCPIN);
			}
			if (HL7650.modemresponsereceived == 2)
			{
				HL7650command.complete(ATCOPSauto);
			}

			break;

		case ATCOPSauto:

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("AT+COPS=0");
			} // Automatic registration
			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATCOPS);
			}

			break;

		case ATCOPSderegister:

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("AT+COPS=2");
			} // De-register from network
			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATCOPS);
			}

			break;

		case ATCPIN: // CPIN (SIM CARD PIN) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("AT+CPIN?");
			}

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATCMEE);
			}

			break;

		case ATCMEE: // CMEE (ERROR REPORT FORMAT) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("AT+CMEE?");
			}

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATCGDCONT);
			}
			if (HL7650.modemresponsereceived == 2)
			{
				HL7650command.complete(ATCMEEmodify);
			}

			break;

		case ATCMEEmodify:

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("AT+CMEE=1");
			}
			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATCMEE);
			}

			break;

		case ATCGDCONT: // CGDCONT (DEFINE PDP CONTEXT) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (HL7650.networkregistrationstatus == 1 || HL7650.networkregistrationstatus == 5)
			{

				if (HL7650.modemreadyfornextcommand == 1)
				{
					sendcmd("AT+CGDCONT?");
				}

				if (HL7650.modemresponsereceived == 1)
				{
					HL7650command.complete(ATWPPP);
				}
				if (HL7650.modemresponsereceived == 2)
				{
					HL7650command.complete(ATCMEEmodify);
				}
			}

			break;

		case ATCGDCONTmodify:

			if (HL7650.modemreadyfornextcommand == 1)
			{
				snprintf(HL7650.modem_send_buff, 256, "AT+CGDCONT=%d,\"IP\",\"%s\",\"\",0,0,1,0,0,0", HL7650.cid, HL7650.APN_name);
				sendcmd(HL7650.modem_send_buff);
			}

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(MODEMRESET);
			}

			break;

		case ATCGDCONTdelete: // Delete CGDCONTEXT

			if (HL7650.modemreadyfornextcommand == 1)
			{
				snprintf(HL7650.modem_send_buff, 256, "AT+CGDCONT=%d", HL7650.cid2);
				sendcmd(HL7650.modem_send_buff);
			}

			// if (HL7650.modemresponsereceived == 1) { HL7650command.complete(MODEMRESET); }

			break;

		case ATWPPP: // WPPP (PDP CONTEXT AUTHENTICATION CONFIGURATION) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("AT+WPPP?");
			}

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATCGACT);
			}
			// if (HL7650.modemresponsereceived == 2) { HL7650command.complete(ATWPPPmodify); }

			/*
		_write_modem_command(&modem,"AT+CFUN?",8); //query network registration. if WPPP settings need to be changed then module needs to be de-registered from the network using at+cfun=4.
		ModemCommandStep++;
		break;

		case 82:
		printf("ENABLE network\r\n");
		_write_modem_command(&modem,"AT+CFUN=1,1",11); //RE-ENABLE NETWORK AND RESET MODULE
		modem_ready = 2; //RESET MODEM BOOT SEQUENCE
		ModemCommandStep = 10; //START AGAIN AFTER RESET
		 break;

		case 84:
		_write_modem_command(&modem,"AT+WPPP?",8);
		ModemCommandStep++;
		break;

		case 86:
		printf("disable network\r\n");
		_write_modem_command(&modem,"AT+CFUN=4",9); //DE-REGISTER FROM NETWORK TO CHANGE WPPP SETTINGS
		ModemCommandStep++;
		break;

		case 87:
		if (modemreadyfornextcommand == 1 && networkregistrationstatus == 0) {
		if (modem.Auth == NONE) { snprintf(modem_send_buff, 256, "AT+WPPP=%d", modem.Auth); }
		else {	snprintf(modem_send_buff, 256, "AT+WPPP=%d,%d,\"%s\",\"%s\"", modem.Auth, cid, modem.APN_login, modem.APN_password); }
		_write_modem_command(&modem,modem_send_buff,strlen(modem_send_buff));
		ModemCommandStep -= ModemCommandStep%10; //ROUND DOWN TO NEAREST 10.
		}
		*/
			break;

		case ATCGACT: // CGACT (ACTIVATE PDP CONTEXT) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("AT+CGACT?");
			}

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATKCNXCFG);
			}
			if (HL7650.modemresponsereceived == 2)
			{
				HL7650command.complete(ATCGACTactivate);
			}
			if (HL7650.modemresponsereceived == 3)
			{
				HL7650command.complete(ATCGACTdeactivate);
			}

			break;

		case ATCGACTactivate: // Activate PDP CONTEXT

			if (HL7650.modemreadyfornextcommand == 1)
			{
				snprintf(HL7650.modem_send_buff, 64, "AT+CGACT=1,%d", HL7650.cid);
				sendcmd(HL7650.modem_send_buff);
			}

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATCGACT);
			}

			break;

		case ATCGACTdeactivate: // De-activate PDP CONTEXT

			if (HL7650.modemreadyfornextcommand == 1)
			{
				snprintf(HL7650.modem_send_buff, 64, "AT+CGACT=0,%d", HL7650.cid);
				sendcmd(HL7650.modem_send_buff);
			}

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATCGACT);
			}

			break;

		case ATKCNXCFG: // KCNXCFG (GPRS CONNECTION CONFIGURATION) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("AT+KCNXCFG?");
			}

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATKSLEEP);
			}
			if (HL7650.modemresponsereceived == 2)
			{
				HL7650command.complete(ATKCNXCFGmodify);
			}

			break;

		case ATKCNXCFGmodify:

			if (HL7650.modemreadyfornextcommand == 1)
			{

				if (HL7650.Auth == NONE)
				{ // omit apn login and password if authentication is not needed.
					snprintf(HL7650.modem_send_buff, Modem_buffer_size, "AT+KCNXCFG=%d,\"GPRS\",\"%s\",\"\",\"\",\"%s\",\"%s\",\"%s\",\"%s\"",
							 HL7650.cid, HL7650.APN_name, "IPV4", HL7650.ip_address, HL7650.dns_1, HL7650.dns_2);
				}

				else
				{ // include apn login and password if authentication set.
					snprintf(HL7650.modem_send_buff, Modem_buffer_size, "AT+KCNXCFG=%d,\"GPRS\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"",
							 HL7650.cid, HL7650.APN_name, HL7650.APN_login, HL7650.APN_password, "IPV4", HL7650.ip_address, HL7650.dns_1, HL7650.dns_2);
				}

				sendcmd(HL7650.modem_send_buff);
			}

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATKCNXCFG);
			}

			break;

		case ATKSLEEP: // KSLEEP (POWER MANAGEMENT CONTROL) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("AT+KSLEEP?");
			}

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATCTZU);
			}
			if (HL7650.modemresponsereceived == 2)
			{
				HL7650command.complete(ATKSLEEPmodify);
			}

			break;

		case ATKSLEEPmodify:

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("AT+KSLEEP=2");
			}

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATKSLEEP);
			}

			break;

		case ATCTZU: // CTZU (AUTOMATIC TIME ZONE UPDATE) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("AT+CTZU?");
			}

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATCTZR);
			}
			if (HL7650.modemresponsereceived == 2)
			{
				HL7650command.complete(ATCTZUmodify);
			}

			break;

		case ATCTZUmodify:

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("AT+CTZU=1");
			}

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATCTZU);
			}

			break;

		case ATCTZR: // CTZR (TIME ZONE REPORTING) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("AT+CTZR?");
			}

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATCCLK);
			}
			if (HL7650.modemresponsereceived == 2)
			{
				HL7650command.complete(ATCTZRmodify);
			}

			break;

		case ATCTZRmodify:

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("AT+CTZR=1");
			}

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATCTZR);
			}

			break;

		case ATCCLK: // CCLK (NETWORK TIME) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("AT+CCLK?");
			}

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATKCNXTIMER);
			}

			break;

		case ATKCNXTIMER: // KCNXTIMER (CONNECTION TIMER CONFIGURATION) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("AT+KCNXTIMER?");
			}

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATKTCPCFG);
			}
			if (HL7650.modemresponsereceived == 2)
			{
				HL7650command.complete(ATKCNXTIMERmodify);
			}

			break;

		case ATKCNXTIMERmodify:

			if (HL7650.modemreadyfornextcommand == 1)
			{

				snprintf(HL7650.modem_send_buff, 256, "AT+KCNXTIMER=%d,30,3,60,600", HL7650.cid);
				sendcmd(HL7650.modem_send_buff);
			}

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATKCNXTIMER);
			}

			break;

		case ATKTCPCFG: // KTCPCFG (TCP CONNECTION CONFIGURATION) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (HL7650.modemreadyfornextcommand == 1)
			{
				sendcmd("AT+KTCPCFG?");
			}

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATKTCPCNX);
			}
			if (HL7650.modemresponsereceived == 2)
			{
				HL7650command.complete(ATKTCPCFGmodify);
			}
			if (HL7650.modemresponsereceived == 3)
			{
				HL7650command.complete(ATKTCPCFGwait);
			}

			break;

		case ATKTCPCFGmodify:

			if (HL7650.modemreadyfornextcommand == 1)
			{
				snprintf(HL7650.modem_send_buff, 256, "AT+KTCPCFG=%d,0,%s,%d,%d,0,1,0", HL7650.cid, HL7650.remoteserver, HL7650.remoteTCPport, HL7650.TCPport); // 1883 non secure port, 8883 secure port.
				sendcmd(HL7650.modem_send_buff);
			}
			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(ATKTCPCFG);
			}
			break;

		case ATKTCPCFGwait:

			if (MQTT.queue_size() > 0)
			{
				HL7650.ModemCommandStep = ATKTCPCNX;
			}

			break;

		case ATKTCPCNX: // KTCPCNX (OPEN TCP CONNECTION) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (HL7650.modemreadyfornextcommand == 1)
			{
				snprintf(HL7650.modem_send_buff, 256, "AT+KTCPCNX=%d", MQTT.mqtt_tcp_session_id);
				sendcmd(HL7650.modem_send_buff);
			}

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(MQTTconnect);
			}
			// waits for +KTCP_IND: 1,1 before proceeding!
			break;

		case MQTTconnect: // +KTCPSND (SEND DATA THROUGH TCP CONNECTION) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (HL7650.modemreadyfornextcommand == 1)
			{
				if (debugEN == 1)
				{
					DEBUG.print(F("MQTT connect...\r\n"));
				}

				msglen = MQTT.get_mqtt_connect_msg(MQTT.send_buff, Modem_buffer_size);
				// printf("MQTT connect message length: %d\r\n", msg_len);
				/*
						len = 0;
						while (len < msg_len) {
							if (MQTT_send_buff[len] < 16) { printf("0"); }
							printf("%x ",MQTT_send_buff[len]);
							len++;
						}
						printf("\r\n");
						printf("\r\n");
				*/
				memcpy(&MQTT.send_buff[msglen], HL7650.MODEM_EOF_PATTERN, strlen(HL7650.MODEM_EOF_PATTERN));

				sprintf(HL7650.modem_send_buff, "AT+KTCPSND=%d,%d", MQTT.mqtt_tcp_session_id, msglen);
				sendcmd(HL7650.modem_send_buff);
				msglen += 16;			   // ADD SIZE OF EOF MESSAGE
				response.ack_type = RESET; // reset MQTT response
				response.message_id = 0;
				response.return_code = NOT_AUTHORIZED;
				MQTT.mqtt_rx_length = 0;
			}

			if (HL7650.modemresponsereceived == 100)
			{
				HL7650command.complete(MQTTconnectSEND);
			}

			break;

		case MQTTconnectSEND: // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (HL7650.modemreadyfornextcommand == 1)
			{
				senddata((uint8_t *)MQTT.send_buff, msglen);
			}

			if (HL7650.modemresponsereceived == 1)
			{
				HL7650command.complete(MQTTconnectRECV);
			}

			break;

		case MQTTconnectRECV: // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (HL7650.modemreadyfornextcommand == 1 && HL7650.tcp_dataavailable > 0)
			{
				sprintf(HL7650.modem_send_buff, "AT+KTCPRCV=%d,%lu", MQTT.mqtt_tcp_session_id, HL7650.tcp_dataavailable);
				MQTT.response_expected = true;
				sendcmd(HL7650.modem_send_buff);
			}
//COMFIRM THE RESPONSE IS A CONACK AND RETURN CODE IS ACCEPTED BEFORE PROCEEDING TO NEXT STEP - JM 28.7
			if (response.ack_type == CONACK && response.return_code == ACCEPTED) {
				if (MQTT.queue_isEmpty() == 0) { HL7650command.complete(MQTTpublish); }
				else { HL7650command.complete(ATKCGPADDR); }
			}
			else if (HL7650.modemresponsereceived == 101) {
				HL7650.modemresponsereceived = 0;
				HL7650.modemreadyfornextcommand = 1;
			}
			
				
					break;

					/*		case 210: // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

								if (response.ack_type == CONACK && response.return_code == ACCEPTED) {
									if (debugEN == 1) { DEBUG.print(F("MCS: "));DEBUG.println(HL7650.ModemCommandStep); }
									if (debugEN == 1) { DEBUG.print(F("MQTT connect OK. Sending subscribe message\r\n")); }
									printMQTTresponse();

									//char topic[] = "v1/devices/me/rpc/request/+";
									MQTT.message_id++;
									//msglen = MQTT.get_mqtt_subscribe_message(MQTT.send_buff, buffer_size, topic, 0, MQTT.message_id);
									msglen = MQTT.get_mqtt_subscribe_message(MQTT.send_buff, buffer_size, MQTT.MQTT_TOPIC, 0, MQTT.message_id);

									//if (debugEN == 1) {
									//	uint16_t gg = 0;
										//DEBUG.println(MQTT.send_buff);
										//while (gg < msglen) {
										//	DEBUG.print(MQTT.send_buff[gg], HEX);
										//	DEBUG.print(F(" "));
										//	gg++;
										//}
										//DEBUG.println();
									//}
									memcpy(&MQTT.send_buff[msglen], HL7650.MODEM_EOF_PATTERN, strlen(HL7650.MODEM_EOF_PATTERN));
									cmdlen = sprintf(HL7650.modem_send_buff, "AT+KTCPSND=%d,%d", MQTT.mqtt_tcp_session_id, msglen);

									sendcmd(HL7650.modem_send_buff);
									msglen += 16; //ADD SIZE OF EOF MESSAGE

									MQTT.published_ID = MQTT.message_id;

									HL7650.ModemCommandStep++;
									if (debugEN == 1) { DEBUG.print(F("MCS: "));DEBUG.println(HL7650.ModemCommandStep); }
								}
								break;

								break;

							case 220: // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

								senddata((uint8_t*)MQTT.send_buff, msglen);
								HL7650.ModemCommandStep++;
								if (debugEN == 1) { DEBUG.print(F("MCS: "));DEBUG.println(HL7650.ModemCommandStep); }
								break;

							case 230: // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

								cmdlen = sprintf(HL7650.modem_send_buff, "AT+KTCPRCV=%d,%lu", MQTT.mqtt_tcp_session_id, HL7650.tcp_dataavailable);
								sendcmd(HL7650.modem_send_buff);
								MQTT.response_expected = true;
								HL7650.ModemCommandStep++;
								if (debugEN == 1) { DEBUG.print(F("MCS: "));DEBUG.println(HL7650.ModemCommandStep); }
								break;

					*/
				case ATKCGPADDR: // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

					if (HL7650.modemreadyfornextcommand == 1)
					{
						sendcmd("AT+KCGPADDR");
					}

					if (HL7650.modemresponsereceived == 1)
					{
						HL7650command.complete(ATCSQ);
					}

					break;

				case ATCSQ: // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

					if (HL7650.modemreadyfornextcommand == 1)
					{
						sendcmd("AT+CSQ");
					}

					if (HL7650.modemresponsereceived == 1)
					{
						HL7650command.complete(ATKBND);
					}

					break;

				case ATKBND: // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

					if (HL7650.modemreadyfornextcommand == 1)
					{
						sendcmd("AT+KBND?");
					}

					if (HL7650.modemresponsereceived == 1)
					{
						HL7650command.complete(MQTTpublish);
					}

					break;

				case MQTTpublish: // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

					if ((MQTT.publish_pending || MQTT.queue_isEmpty() == 0) && HL7650.modemreadyfornextcommand == 1)
					{

						if (debugEN == 1)
						{
							DEBUG.print(F("MQTT connect OK. Publishing...\r\n"));
						}
						printMQTTresponse();

						if (!MQTT.publish_pending) {
						char msg_payload_buf[Modem_buffer_size] = {'\0'};
						char recovered_name[3] = {'\0'};

						snprintf(msg_payload_buf, 50, "{\"deviceID\":\"%s\",\"ts\":\"%lu\"", MQTT.MQTT_CLIENT_ID, DS1338.epoch);

						while (MQTT.queue_size() > 0)
						{

							MQTT.dequeue_MQTT_update();
							if (MQTT.dequeued_data[0] == MQTT_char)
							{
								memcpy(recovered_name, MQTT.dequeued_data + 14, 2);
							}
							else
							{
								memcpy(recovered_name, MQTT.dequeued_data + 6, 2);
							}

							recovered_name[2] = '\0';

							if (MQTT.dequeued_data[0] == MQTT_float)
							{ // float data type
								float recovered_float = dataconversion.float32_from_four_uint8(MQTT.dequeued_data[3], MQTT.dequeued_data[2], MQTT.dequeued_data[5], MQTT.dequeued_data[4]);
								snprintf(&msg_payload_buf[strlen(msg_payload_buf)], 20, ",\"%s\":\"%3.1f\"", recovered_name, recovered_float);
							}

							if (MQTT.dequeued_data[0] == MQTT_uint32_t)
							{ // uint32_t data type
								uint32_t recovered_int32t = dataconversion.uint32_t_from_four_uint8(MQTT.dequeued_data[3], MQTT.dequeued_data[2], MQTT.dequeued_data[5], MQTT.dequeued_data[4]);
								snprintf(&msg_payload_buf[strlen(msg_payload_buf)], 20, ",\"%s\":\"%lu\"", recovered_name, recovered_int32t);
							}

							if (MQTT.dequeued_data[0] == MQTT_uint16_t)
							{ // uint16_t data type
								uint16_t recovered_int16t = ((uint16_t)MQTT.dequeued_data[3] << 8) | MQTT.dequeued_data[2];
								snprintf(&msg_payload_buf[strlen(msg_payload_buf)], 20, ",\"%s\":\"%hu\"", recovered_name, recovered_int16t);
							}

							if (MQTT.dequeued_data[0] == MQTT_uint8_t)
							{ // uint8_t data type
								snprintf(&msg_payload_buf[strlen(msg_payload_buf)], 20, ",\"%s\":\"%u\"", recovered_name, MQTT.dequeued_data[2]);
							}

							if (MQTT.dequeued_data[0] == MQTT_int8_t)
							{ // int8_t data type
								snprintf(&msg_payload_buf[strlen(msg_payload_buf)], 20, ",\"%s\":\"%i\"", recovered_name, (int8_t)MQTT.dequeued_data[2]);
							}

							if (MQTT.dequeued_data[0] == MQTT_char)
							{ // char data type
								snprintf(&msg_payload_buf[strlen(msg_payload_buf)], 20, ",\"%s\":\"%s\"", recovered_name, MQTT.dequeued_data + 2);
							}

							if (strlen(msg_payload_buf) >= Modem_buffer_size - 20)
							{ // break from loop if there isnt enough room for another data point
								break;
							}
						}

						snprintf(&msg_payload_buf[strlen(msg_payload_buf)], 2, "}");
						strncpy(MQTT.pending_payload, msg_payload_buf, sizeof(MQTT.pending_payload) - 1);
						MQTT.pending_payload[sizeof(MQTT.pending_payload) - 1] = '\0';
						MQTT.message_id++;
						MQTT.published_ID = MQTT.message_id;
						MQTT.publish_pending = true;
						MQTT.send_attempt = 0;
						}

						response.ack_type = RESET;
						response.message_id = 0;
						response.return_code = NOT_AUTHORIZED;
						/*
						if (debugEN == 1) {
							DEBUG.print(F("message size: "));
							DEBUG.println(strlen(msg_payload_buf));
							DEBUG.println(msg_payload_buf);
						}
						*/
						uint8_t duplicate = MQTT.send_attempt == 0 ? 0 : 1;
						MQTT.send_attempt++;
						msglen = MQTT.get_mqtt_pub_message(MQTT.send_buff, Modem_buffer_size, MQTT.MQTT_TOPIC, MQTT.pending_payload, 1, duplicate, MQTT.published_ID);
						memcpy(&MQTT.send_buff[msglen], HL7650.MODEM_EOF_PATTERN, strlen(HL7650.MODEM_EOF_PATTERN));
						/*
						if (debugEN == 1) {
							DEBUG.print(F("MQTT.send_buff: "));DEBUG.println();
							uint16_t rew = 0;
							while (rew < msglen) {
								if (MQTT.send_buff[rew] < 16) { DEBUG.print(F("0")); }
								DEBUG.print(MQTT.send_buff[rew], HEX);
								DEBUG.print(F(" "));
								rew++;
							}

							DEBUG.println();
						}
						*/
						// ENDTEXTFORMATTING;
						sprintf(HL7650.modem_send_buff, "AT+KTCPSND=%d,%d", MQTT.mqtt_tcp_session_id, msglen);
						sendcmd(HL7650.modem_send_buff);
						msglen += 16; // ADD SIZE OF EOF MESSAGE
												MQTT.publish_sent_at = millis();
						// printf("Publish message ID: %x\r\n", MQTT.message_id);
						// printf("Publish message ID: %x\r\n", MQTT_published_ID);
						// HL7650.ModemCommandStep++;
					}

					if (HL7650.modemresponsereceived == 100)
					{
						HL7650command.complete(MQTTpublishSEND);
					}

					break;

				case MQTTpublishSEND: // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

					if (HL7650.modemreadyfornextcommand == 1)
					{
						senddata((uint8_t *)MQTT.send_buff, msglen);
					}

					if (HL7650.modemresponsereceived == 1)
					{
						HL7650command.complete(MQTTpublishRECV);
					}

					break;

				case MQTTpublishRECV: // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
					if (response.ack_type == PUBACK && response.message_id == MQTT.published_ID)
					{
						HL7650command.complete(MQTTpublishRESULT);
						break;
					}
					if ((uint32_t)(millis() - MQTT.publish_sent_at) >= 15000)
					{
						response.ack_type = RESET;
						response.message_id = 0;
						response.return_code = NOT_AUTHORIZED;
						HL7650command.complete(MQTTpublish);
						break;
					}

					if (HL7650.modemreadyfornextcommand == 1 && HL7650.tcp_dataavailable > 0)
					{
						sprintf(HL7650.modem_send_buff, "AT+KTCPRCV=%d,%lu", MQTT.mqtt_tcp_session_id, HL7650.tcp_dataavailable);
						MQTT.response_expected = true;
						sendcmd(HL7650.modem_send_buff);
					}

					if (HL7650.modemresponsereceived == 101 &&
						!(response.ack_type == PUBACK && response.message_id == MQTT.published_ID))
					{
						HL7650.modemresponsereceived = 0;
						HL7650.modemreadyfornextcommand = 1;
					}

					break;

				case MQTTpublishRESULT: // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

					if (response.ack_type == PUBACK && response.message_id == MQTT.published_ID)
					{
						if (debugEN == 1)
						{
							DEBUG.println(F("MQTT PUBACK (PUBLISH) OK %"));
						}

						MQTT.modem_SERVER_timeout = millis(); // RESET TIMER
												MQTT.publish_pending = false;
												MQTT.send_attempt = 0;
						HL7650.set_last_send_time(DS1338.epoch);
						HL7650command.complete(MQTTpublish);
					}

					else
					{
						if (debugEN == 1)
						{
							DEBUG.println(F("MQTT PUBLISH FAILED!!!!"));
						}
						response.ack_type = RESET;
						response.message_id = 0;
						response.return_code = NOT_AUTHORIZED;
						HL7650command.complete(MQTTpublish);
					}

					break;

				case MQTTdisconnect: // +KTCPSND (SEND DATA THROUGH TCP CONNECTION) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

					if (HL7650.modemreadyfornextcommand == 1)
					{
						if (debugEN == 1)
						{
							DEBUG.print(F("MQTT disconnect...\r\n"));
						}

						msglen = MQTT.get_mqtt_disconnect_msg(MQTT.send_buff, Modem_buffer_size);

						memcpy(&MQTT.send_buff[msglen], HL7650.MODEM_EOF_PATTERN, strlen(HL7650.MODEM_EOF_PATTERN));

						sprintf(HL7650.modem_send_buff, "AT+KTCPSND=%d,%d", MQTT.mqtt_tcp_session_id, msglen);
						sendcmd(HL7650.modem_send_buff);
						msglen += 16; // ADD SIZE OF EOF MESSAGE
					}

					if (HL7650.modemresponsereceived == 100)
					{
						HL7650command.complete(MQTTdisconnectSEND);
					}

					break;

				case MQTTdisconnectSEND: // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

					if (HL7650.modemreadyfornextcommand == 1)
					{
						senddata((uint8_t *)MQTT.send_buff, msglen);
					}

					if (HL7650.modemresponsereceived == 1)
					{
						HL7650command.complete(MQTTdisconnectRECV);
					}

					break;

				case MQTTdisconnectRECV: // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

					if (HL7650.modemreadyfornextcommand == 1)
					{
						sprintf(HL7650.modem_send_buff, "AT+KTCPRCV=%d,%lu", MQTT.mqtt_tcp_session_id, HL7650.tcp_dataavailable);
						MQTT.response_expected = true;
						sendcmd(HL7650.modem_send_buff);
					}

					if (HL7650.modemresponsereceived == 101)
					{
						HL7650command.complete(ATKTCPCFGwait);
					}

					break;
					/*
							case 310: // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
								//WAS 280
								if (debugEN == 1) { DEBUG.print(F("READY FOR NEXT COMMAND&\r\n")); }
								//_write_modem_command(&modem,"AT+CPIN?",8);
								HL7650.ModemCommandStep = 230;
								break;
					*/
				case TCPdisconnect: // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

					if (HL7650.modemreadyfornextcommand == 1)
					{
						sprintf(HL7650.modem_send_buff, "AT+KTCPSTAT=%d", MQTT.mqtt_tcp_session_id);
						sendcmd(HL7650.modem_send_buff);
					}

					if (HL7650.modemresponsereceived == 1 && MQTT.TCPsocketSTATUS == SOCKETCLOSED)
					{
						HL7650command.complete(ATKTCPCFG);
					}
					if (HL7650.modemresponsereceived == 1 && MQTT.TCPsocketSTATUS == SOCKETREADY)
					{
						HL7650command.complete(TCPsocketCLOSE);
					}

					break;

				case TCPsocketCLOSE: // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

					if (HL7650.modemreadyfornextcommand == 1)
					{
						sprintf(HL7650.modem_send_buff, "AT+KTCPCLOSE=%d", MQTT.mqtt_tcp_session_id);
						sendcmd(HL7650.modem_send_buff);
					}

					if (HL7650.modemresponsereceived == 1)
					{
						HL7650command.complete(ATKTCPCFG);
					}

					break;

				case TCPdelete: // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

					if (HL7650.modemreadyfornextcommand == 1)
					{
						sprintf(HL7650.modem_send_buff, "AT+KTCPDEL=%d", MQTT.mqtt_tcp_session_id);
						sendcmd(HL7650.modem_send_buff);
					}

					if (HL7650.modemresponsereceived == 1)
					{
						HL7650command.complete(ATKTCPCFG);
					} // REVIEW ACTION

					break;

				case POWERoff: // Power off %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

					if (HL7650.modemreadyfornextcommand == 1)
					{
						sendcmd("AT+CPOF");
					}

					if (HL7650.modemresponsereceived == 1)
					{
						HL7650command.complete(ATKTCPCFG);
					} // REVIEW ACTION

					break;

				case CMEERROR:

					HL7650command.complete(TCPdisconnect);

					break;
				}
		}
	}

	void HL7650commandClass::printMQTTresponse(void)
	{

		if (debugEN == 1)
		{

			if (response.return_code == ACCEPTED)
			{
				DEBUG.println(F("MQTT Connection accepted\r\n"));
			}
			if (response.return_code == UNACCEPTABLE_PROTOCOL)
			{
				DEBUG.println(F("MQTT Connection refused, unacceptable protocol version\r\n"));
			}
			if (response.return_code == IDENTIFIER_REJECTED)
			{
				DEBUG.println(F("MQTT Connection refused, identifier rejected\r\n"));
			}
			if (response.return_code == SERVER_UNAVAILABLE)
			{
				DEBUG.println(F("MQTT Connection refused, server unavailable\r\n"));
			}
			if (response.return_code == BAD_USER_PASSWORD)
			{
				DEBUG.println(F("MQTT Connection refused, bad user name or password\r\n"));
			}
			if (response.return_code == NOT_AUTHORIZED)
			{
				DEBUG.println(F("MQTT Connection refused, not authorized\r\n"));
			}
		}
	}

	uint8_t HL7650commandClass::sendcmd(const char *cmd)
	{

		if (digitalRead(MDM_CTS) == LOW && HL7650.modemreadyfornextcommand >= 1)
		{ // ensure modem is ready for command

			HL7650.modemreadyfornextcommand = 0;
			HL7650.modemresponsereceived = 0;
			HL7650.modem_response_timeout = millis(); // reset timeout counter

			modemUART.println(cmd);
			modemUART.flush();

			if (debugEN == 1)
			{	
				DEBUG.print(F(">>cmd: "));
				DEBUG.println();
				DEBUG.println(cmd);
				DEBUG.println();
			}

			return 1;
		}

		else
		{
			return 0;
		}
	}

	void HL7650commandClass::senddata(uint8_t *sendingbuffer, uint16_t message_len)
	{

		if (digitalRead(MDM_CTS) == LOW)
		{ // ensure modem is ready for command

			HL7650.modemreadyfornextcommand = 0;
			HL7650.modemresponsereceived = 0;
			HL7650.modem_response_timeout = millis(); // reset timeout counter

			if (debugEN == 1)
			{

				DEBUG.println();
				DEBUG.print(F("sending data bytes: ")); // REMOVE 16 BYTES FOR EOF-PATTERN
				DEBUG.print(message_len - 16);
				DEBUG.println();
			}

			for (unsigned int i = 0; i < message_len; i++)
			{
				/*
				if (debugEN == 1) {
					DEBUG.print(sendingbuffer[i], HEX);
					DEBUG.print(F(" "));
				}
				*/
				modemUART.write(sendingbuffer[i]);
			}

			// if (debugEN == 1) { DEBUG.println();DEBUG.println(); }
		}

		// else if (debugEN == 1) { DEBUG.println(F("DATA SEND FAIL. CTS NOT LOW")); }
	}

	void HL7650commandClass::complete(uint16_t nextstep)
	{

		HL7650.modemresponsereceived = 0;
		HL7650.modemreadyfornextcommand = 1;
		HL7650.ModemCommandStep = nextstep;
		if (nextstep == MODEMRESET)
		{
			HL7650.reset_required = 1;
		} // reboot module after command given
	}
