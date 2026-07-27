// HL7650_600P.h

#ifndef _HL7650_600P_h
#define _HL7650_600P_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
	#include "main.h"

#else
	#include "WProgram.h"
#endif

//MODEM
#define MDM_CTS 32
#define MDM_DTR 33

#define MODEM_RESPONSE_TIMOUT_MS 65000

#define NONE 0
#define PAP 1
#define CHAP 2

#define APN_BUFF_SIZE 64
#define APN_LOGIN_BUFF_SIZE 64
#define APN_PASSWORD_BUFF_SIZE 64
#define IP_ADDRESS_BUFF_SIZE 32

#define Modem_buffer_size 384

class HL7650Class
{
 protected:

	 
	 bool DataMode = false;
	 
	 uint8_t Modem_Data_Buffer[256] = { 0 };
	 uint32_t recv_count = 0;
	 uint32_t data_count = 0;
	 volatile uint32_t timeout_read = 0;
	 uint8_t connectSTAT = 0;
	 uint8_t eofSTAT = 0;
	 uint8_t modemfirstrun = 0;
	 
	 uint32_t modemreadytimer = 0;
	 
	 
	 uint8_t modemRESETactive = 0;
	 const char* MODEM_EOF_PATTERN_START = "--EOF";  // Pattern used to show end of data when modem is in data mode
	 const char* MODEM_EOF_PATTERN_END = "--Pattern--";

 public:
	
	 const char* netBands[17] = { "NOT AVAILABLE", "GSM 900 MHz", "DCS 1800 MHz","UTMS Band I (2100 MHz)","UTMS Band II (1900 MHz)","UTMS Band V (850 MHz)","UTMS Band VIII (900 MHz)","LTE Band 2 (1900 MHz)"
								,"LTE Band 3 (1800 MHz)","LTE Band 4 (1700 MHz)","LTE Band 5 (850 MHz)","LTE Band 13 (700 MHz)","LTE Band 17 (700 MHz)","LTE Band 28 (700 MHz)","LTE Band 8 (900 MHz)","LTE Band 20 (800 MHz)","LTE Band 12 (700 MHz)" };

	 const char* netStatus[6] = { "Not registered to network, NOT searching!","Registered to network, Home network - ","Not registered to network, searching..."
								,"Network registration denied","Unknown network registration error","Registered to network, Roaming - " };

	 const char* netType[8] = { "GSM","","UTRAN","GSM with EGPRS","UTRAN with HSDPA","UTRAN with HSUPA","UTRAN with HSDPA and HSUPA","E-UTRAN" };

	 // APN parameters 
	 char APN_name[APN_BUFF_SIZE] = { '\0' };
	 char APN_login[APN_LOGIN_BUFF_SIZE] = { '\0' };
	 char APN_password[APN_PASSWORD_BUFF_SIZE] = { '\0' };
	 char static_ip[IP_ADDRESS_BUFF_SIZE] = { '\0' };
	 char ip_address[IP_ADDRESS_BUFF_SIZE] = { '\0' };  //In dotted decimal notation
	 char dns_1[IP_ADDRESS_BUFF_SIZE] = { '\0' };      //In dotted decimal notation
	 char dns_2[IP_ADDRESS_BUFF_SIZE] = { '\0' };       //In dotted decimal notation

	 // Diversity antenna enable 
	 int8_t divEN = 0;

	 // Network authentication. 0 for none, 1 for PAP, 2 for CHAP 
	 int8_t Auth = NONE;

	 //local port number.
	 uint16_t TCPport = 0;

	 //remote port number.
	 uint16_t remoteTCPport = 0;

	 //remote server address.
	 char remoteserver[32] = { '\0' };

	 // Set to enable verbose output to terminal e.g. printing AT command responses 
	 uint8_t debug_EN = 0;

	 uint8_t ntwkBAND = 0;
	 int8_t ntwkSIG = 0;
	 volatile uint8_t modem_ready; //0 - not ready. 1 - ready for commands, >2 - boot sequence.
	 char MODEM_EOF_PATTERN[20] = { '\0' };
	 uint32_t tcp_dataavailable = 0;
	 volatile uint8_t modemreadyfornextcommand = 0;
	 volatile uint8_t modemresponsereceived = 0;
	 volatile bool ERROR = false;
	 volatile uint32_t modem_response_timeout = 0;
	 volatile uint32_t ERROR_timeout = 0;
	 uint16_t ModemCommandStep = 0;
	 uint8_t modem_initial_response_seen = 0;
	 char modem_send_buff[Modem_buffer_size] = { '\0' };
	 uint8_t cid = 2;
	 uint8_t cid2 = 0;
	 bool command_response_expected = false;
	 char Modem_buffer[Modem_buffer_size] = { '\0' };
	 uint8_t networkregistrationstatus = 0;
	 uint32_t networklocationcode = 0;
	 uint32_t networkcellid = 0;
	 uint16_t networkAcT = 0;
	 int16_t timezone = 0; //difference between local time and GMT expressed in 15 min intervals. (range -48 to +56)
	 uint8_t daylightsavingtime = 0; // 0 - no DST adjustment, 1 - +1 hour , 2 - +2 hours
	 uint8_t reset_required = 0;

	void init();
	void modem_powerON(void);
	void modem_powerOFF(void);
	void modem_reset(void);
	void read(void);
	void processDATA();
	void ModemReady();
	void set_last_send_time(uint32_t timestamp);
};

extern HL7650Class HL7650;

#endif

