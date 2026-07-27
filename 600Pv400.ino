#include "main.h"

uint32_t heartbeat_timer = 0;
uint32_t fivehundredms_timer = 0;
uint32_t onehundredms_timer = 0;
uint32_t MQTTsend_timer = 0;

uint32_t totaluptimeSECONDS = 0;
uint8_t currentflashbank = 0;
uint8_t debugEN = 0;
uint8_t modemdebugEN = 0;
uint32_t PCBserialno = 0;
uint32_t lastresetcause = 0;
uint32_t looptimestart = 0;
uint32_t looptime = 0;
uint32_t faultBITS = 0;
uint32_t faultBITS_1 = 0;
uint32_t HardwareStatus = 0;
uint32_t HardwareStatus_1 = 0;

void setup() {

	//IO setup *********************************************************************************************************************************
	
	pinMode(BLUE_LED, OUTPUT);
	pinMode(YELLOW_LED, OUTPUT);
	pinMode(RED_LED, OUTPUT);
	
	pinMode(FLGB1, INPUT); //FLGB CONNECTOR 1
	pinMode(FLGB2, INPUT); //FLGB CONNECTOR 2
	pinMode(FLGB3, INPUT); //FLGB CONNECTOR 3

	pinMode(sense1PWR, OUTPUT); //24v ON CONNECTOR 1
	digitalWrite(sense1PWR, HIGH);
	pinMode(sense2PWR, OUTPUT); //24v ON CONNECTOR 2
	digitalWrite(sense2PWR, HIGH);
	pinMode(sense3PWR, OUTPUT); //24v ON CONNECTOR 3
	digitalWrite(sense3PWR, HIGH);

	pinMode(A10, INPUT_PULLUP); //SD INSERTED DETECTION
	pinMode(4, OUTPUT); //SD chip select

	pinMode(rs4851REDE, OUTPUT); //RS485 1 RE/DE PIN
	digitalWrite(rs4851REDE, LOW);
	pinMode(rs4852REDE, OUTPUT); //RS485 2 RE/DE PIN
	digitalWrite(rs4852REDE, LOW);
	pinMode(rs4853REDE, OUTPUT); //RS485 3 RE/DE PIN
	digitalWrite(rs4853REDE, LOW);

	pinMode(A4, OUTPUT); //BT ENABLE
	pinMode(A2, INPUT); //BT CTS
	pinMode(A3, INPUT); //BT RTS

	//Watchdog timer initialise *********************************************************************************************************************************

	watchdogEnable(5000); //Enables Watchdog timer. Time in ms to reset controller if the process locks up.

	//Determine flashbank code is currently running from *********************************************************************************************************************************

	currentflashbank = EEPROM.getflashbank();

	//Serial Port setup *********************************************************************************************************************************

	Wire1.begin(); //INITIATE WIRE COMMUNICATION FOR EEPROM AND RTC
	DEBUG.begin(115200);	
	btUART.begin(115200); //open serial port for BT

	//EEPROM initialise *****************************************************************************************************************************

	EEPROM.init();

	//RTC setup ****************************************************************************************************************************************

	DS1338.init();

	//SD card setup ****************************************************************************************************************************************

	SDcard.init();

	//READ AND LOG THE CAUSE OF SYTEM RESET ****************************************************************************************************************************************

	lastresetcause = rstc_get_reset_cause(RSTC) >> RSTC_SR_RSTTYP_Pos;

	if (lastresetcause == GENERAL) { SDcard.logEVENT("GENERAL RESET"); }
	if (lastresetcause == BACKUP) { SDcard.logEVENT("BACKUP RESET"); }
	if (lastresetcause == SOFTWARE) { SDcard.logEVENT("SOFTWARE RESET"); }
	if (lastresetcause == USER) { SDcard.logEVENT("USER RESET"); }
	if (lastresetcause == WATCHDOG) { //DISABLE SERIAL OUTPUTS INCASE THEY WERE THE CAUSE OF THE WATCHDOG RESET!

		SDcard.logEVENT("WATCHDOG RESET");

		if (debugEN == 1) {
			debugEN = 0;
			EEPROM.write(EEPROM0, 0, debugEN);
		}

		if (modemdebugEN == 1) {
			modemdebugEN = 0;
			EEPROM.write(EEPROM0, 4, modemdebugEN);
		}

	}

	//wait for DEBUG port *****************************************************************************************************************************

	if (debugEN == 1) { while (!DEBUG); }

	//DPT145 setup ****************************************************************************************************************************************

	sensor.init();

	//MQTT setup ****************************************************************************************************************************************

	MQTT.init();

	MQTT.queue_MQTT_update((uint8_t)lastresetcause, "Rr"); //add new value to MQTT send queue
	MQTT.queue_MQTT_update(thisCODEversion, "Fw"); //add new value to MQTT send queue

	//Modem setup ****************************************************************************************************************************************

	HL7650.init();

}

void loop() {

	looptimestart = micros();

	WDT_Restart(WDT); //call this every code loop to prevent the watchdog resetting the microcontroller.
	serialcommands.scan();
	sensor.read();
	IO.read();
	HL7650.read();

	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	if ((millis() - heartbeat_timer) >= 1000) { //HEARTBEAT LED - dont change this from 1000!
		totaluptimeSECONDS++;

		digitalWrite(BLUE_LED, !digitalRead(BLUE_LED));
		MCP98244.PCBtemp = MCP98244.readtemp();

		if ((MCP98244.PCBtemp >= (MCP98244.PCBtemp_1 + 0.5)) || (MCP98244.PCBtemp <= (MCP98244.PCBtemp_1 - 0.5))) {
			//MQTT.queue_MQTT_update(MCP98244.PCBtemp, "Tp"); //add new value to MQTT send queue
			MCP98244.PCBtemp_1 = MCP98244.PCBtemp;
		}

		if (faultBITS_1 != faultBITS) {
			MQTT.queue_MQTT_update(faultBITS, "Ec"); //add new value to MQTT send queue
			faultBITS_1 = faultBITS;
		}

		if (HardwareStatus_1 != HardwareStatus) {
			MQTT.queue_MQTT_update(HardwareStatus, "Hs"); //add new value to MQTT send queue
			HardwareStatus_1 = HardwareStatus;
		}

		heartbeat_timer = millis();
	}

	if (millis() < heartbeat_timer) { //reset after rollover
		heartbeat_timer = millis();
	}

	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	if ((millis() - fivehundredms_timer) >= 500) { //500ms timer

		if (faultBITS > 0) { digitalWrite(RED_LED, !digitalRead(RED_LED)); }
		if (faultBITS == 0 && digitalRead(RED_LED) == HIGH) { digitalWrite(RED_LED, LOW); }
		DS1338.epoch = DS1338.readRTC(RTC0);

		fivehundredms_timer = millis();
	}

	if (millis() < fivehundredms_timer) { //reset after rollover
		fivehundredms_timer = millis();
	}

	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	if ((millis() - MQTTsend_timer) >= 300000) { //5 minute MQTT send timer 300000
	
		MQTT.queue_MQTT_update(totaluptimeSECONDS, "UT"); //add new value to MQTT send queue
		MQTT.queue_MQTT_update(MCP98244.PCBtemp, "Tp"); //add new value to MQTT send queue

		if (bitRead(HardwareStatus, 2) == 1) { //if sensor 1 enabled
			MQTT.queue_MQTT_update(sensor1.T, "T1"); //add new value to MQTT send queue
			MQTT.queue_MQTT_update(sensor1.Tdf, "D1"); //add new value to MQTT send queue
			MQTT.queue_MQTT_update(sensor1.Tdfa, "d1"); //add new value to MQTT send queue
			MQTT.queue_MQTT_update(sensor1.H2O, "M1"); //add new value to MQTT send queue
			MQTT.queue_MQTT_update(sensor1.P, "P1"); //add new value to MQTT send queue
			MQTT.queue_MQTT_update(sensor1.Pnorm, "p1"); //add new value to MQTT send queue
			MQTT.queue_MQTT_update(sensor1.Rhoo, "R1"); //add new value to MQTT send queue
		}

		if (bitRead(HardwareStatus, 3) == 1) { //if sensor 2 enabled
			MQTT.queue_MQTT_update(sensor2.T, "T2"); //add new value to MQTT send queue
			MQTT.queue_MQTT_update(sensor2.Tdf, "D2"); //add new value to MQTT send queue
			MQTT.queue_MQTT_update(sensor2.Tdfa, "d2"); //add new value to MQTT send queue
			MQTT.queue_MQTT_update(sensor2.H2O, "M2"); //add new value to MQTT send queue
			MQTT.queue_MQTT_update(sensor2.P, "P2"); //add new value to MQTT send queue
			MQTT.queue_MQTT_update(sensor2.Pnorm, "p2"); //add new value to MQTT send queue
			MQTT.queue_MQTT_update(sensor2.Rhoo, "R2"); //add new value to MQTT send queue
		}

		if (bitRead(HardwareStatus, 4) == 1) { //if sensor 3 enabled
			MQTT.queue_MQTT_update(sensor3.T, "T3"); //add new value to MQTT send queue
			MQTT.queue_MQTT_update(sensor3.Tdf, "D3"); //add new value to MQTT send queue
			MQTT.queue_MQTT_update(sensor3.Tdfa, "d3"); //add new value to MQTT send queue
			MQTT.queue_MQTT_update(sensor3.H2O, "M3"); //add new value to MQTT send queue
			MQTT.queue_MQTT_update(sensor3.P, "P3"); //add new value to MQTT send queue
			MQTT.queue_MQTT_update(sensor3.Pnorm, "p3"); //add new value to MQTT send queue
			MQTT.queue_MQTT_update(sensor3.Rhoo, "R3"); //add new value to MQTT send queue
		}

		MQTTsend_timer = millis();		
	}

	if (millis() < MQTTsend_timer) { //reset after rollover
		MQTTsend_timer = millis();
	}

	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	if ((millis() - onehundredms_timer) >= 100) { //100ms timer


		onehundredms_timer = millis();
	}

	if (millis() < onehundredms_timer) { //reset after rollover
		onehundredms_timer = millis();
	}

	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	if ((micros() - looptimestart) > looptime) { looptime = micros() - looptimestart; }

}

void watchdogSetup(void) { //REMOVING THIS FUNCTION WILL DISABLE WATCHDOG

}