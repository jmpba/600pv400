// 
// 
// 

#include "IO_600P.h"

IOClass IO;

void IOClass::init()
{


}

void IOClass::read()
{

	//24V FAULT DETECTION FOR DPT145 SENSORS**********************************************************************************************************************************************

	if (digitalRead(FLGB1) == HIGH && bitRead(faultBITS,15) == 1) {
		bitWrite(faultBITS, 15, 0); //SENSOR A 24V FAULT RESET
		PWRcycletime = SENSORpowertime;
		if (debugEN == 1) { DEBUG.println(F("24V FAULT CONNECTOR 1 RESET")); }
	}

	if (digitalRead(FLGB2) == HIGH && bitRead(faultBITS, 16) == 1) {
		bitWrite(faultBITS, 16, 0); //SENSOR B 24V FAULT RESET
		PWRcycletime = SENSORpowertime;
		if (debugEN == 1) { DEBUG.println(F("24V FAULT CONNECTOR 2 RESET")); }
	}

	if (digitalRead(FLGB3) == HIGH && bitRead(faultBITS, 17) == 1) {
		bitWrite(faultBITS, 17, 0); //SENSOR C 24V FAULT RESET
		PWRcycletime = SENSORpowertime;
		if (debugEN == 1) { DEBUG.println(F("24V FAULT CONNECTOR 3 RESET")); }
	}

	if (digitalRead(FLGB1) == LOW && bitRead(faultBITS, 15) == 0 && numSensors > 0) {
		SDcard.logEVENT("24V FAULT 1");
		bitWrite(faultBITS, 15, 1); //SENSOR A 24V FAULT ACTIVE
		FAULTretrySENSOR1++;
		PWRcycletime = 120000;
		if (FAULTretrySENSOR1 == 3) { SDcard.logEVENT("24V FAULT 1 LOCKOUT"); }
		if (FAULTretrySENSOR1 <= 3) { PWRstage = OFF; powercycletimer = millis(); } //Try to power up sensor again if attempts are less than 3
		if (debugEN == 1) {
			DEBUG.println(F("24V FAULT DETECTED CONNECTOR 1"));
			if (FAULTretrySENSOR1 == 3) { DEBUG.println(F("LOCKOUT")); DEBUG.println(); }
		}
	}

	if (digitalRead(FLGB2) == LOW && bitRead(faultBITS, 16) == 0 && numSensors > 1) {
		SDcard.logEVENT("24V FAULT 2");
		bitWrite(faultBITS, 16, 1); //SENSOR B 24V FAULT ACTIVE
		FAULTretrySENSOR2++;
		PWRcycletime = 120000;
		if (FAULTretrySENSOR2 == 3) { SDcard.logEVENT("24V FAULT 2 LOCKOUT"); }
		if (FAULTretrySENSOR2 <= 3) { PWRstage = OFF; powercycletimer = millis(); } //Try to power up sensor again if attempts are less than 3
		if (debugEN == 1) {
			DEBUG.println(F("24V FAULT DETECTED CONNECTOR 2"));
			if (FAULTretrySENSOR2 == 3) { DEBUG.println(F("LOCKOUT")); DEBUG.println(); }
		}
	}

	if (digitalRead(FLGB3) == LOW && bitRead(faultBITS, 17) == 0 && numSensors > 2) {
		SDcard.logEVENT("24V FAULT 3");
		bitWrite(faultBITS, 17, 1); //SENSOR C 24V FAULT ACTIVE
		FAULTretrySENSOR3++;
		PWRcycletime = 120000;
		if (FAULTretrySENSOR3 == 3) { SDcard.logEVENT("24V FAULT 3 LOCKOUT"); }
		if (FAULTretrySENSOR3 <= 3) { PWRstage = OFF; powercycletimer = millis(); } //Try to power up sensor again if attempts are less than 3
		if (debugEN == 1) {
			DEBUG.println(F("24V FAULT DETECTED CONNECTOR 3"));
			if (FAULTretrySENSOR3 == 3) { DEBUG.println(F("LOCKOUT")); DEBUG.println(); }
		}
	}
/*
	if (Fault24V1 > 0 || Fault24V2 > 0 || Fault24V3 > 0 || S1modbusERR > 0 || S2modbusERR > 0 || S3modbusERR > 0) {
		if (digitalRead(RED_LED) == LOW) { digitalWrite(RED_LED, HIGH); } //RED LED ON
	}

	if (Fault24V1 == 0 && Fault24V2 == 0 && Fault24V3 == 0 && S1modbusERR == 0 && S2modbusERR == 0 && S3modbusERR == 0) {
		if (digitalRead(RED_LED) == HIGH) { digitalWrite(RED_LED, LOW); } //RED LED OFF AFTER FAULT RESET
	}
*/
	//DPT145 SENSOR POWER UP SEQUENCE. STARTUP STAGGERED TO PREVENT CURRENT INRUSH ISSUES. *********************************************************************************

	if ((millis() - powercycletimer) >= PWRcycletime) { //SENSOR POWERUP TIMER. EACH SENSOR IS POWERED UP 30 SECONDS APART.

		if (PWRstage < COMPLETE) {

			if (PWRstage == TIMEADJUST) {
				PWRstage = COMPLETE;
				if (debugEN == 1) { DEBUG.println(F("POWER UP SEQUENCE COMPLETE")); }
				PWRcycletime = SENSORpowertime;
			}

			//Sensor 3 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (digitalRead(sense3PWR) == LOW && bitRead(faultBITS, 17) == 1 && FAULTretrySENSOR3 < 3) {
				if (debugEN == 1) { DEBUG.println(F("RESSETTING OVERCURRENT IC 3")); }
				digitalWrite(sense3PWR, HIGH);
			}

			if (digitalRead(sense3PWR) == HIGH && PWRstage == SENSOR3) {
				if (numSensors > 2) {
					digitalWrite(sense3PWR, LOW);
					//if (responseTIMEOUTC != 0) { responseTIMEOUTC = 0; }
					if (debugEN == 1) { DEBUG.println(F("POWER STAGE THREE COMPLETE")); DEBUG.println(); }
				}
				PWRstage = TIMEADJUST;
			}

			else if (PWRstage == SENSOR3) { PWRstage = TIMEADJUST; }

			//Sensor 2 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (digitalRead(sense2PWR) == LOW && bitRead(faultBITS, 16) == 1 && FAULTretrySENSOR2 < 3) {
				if (debugEN == 1) { DEBUG.println(F("RESSETTING OVERCURRENT IC 2")); }
				digitalWrite(sense2PWR, HIGH);
			}

			if (digitalRead(sense2PWR) == HIGH && PWRstage == SENSOR2) {
				if (numSensors > 1) {
					digitalWrite(sense2PWR, LOW);
					//if (responseTIMEOUTB != 0) { responseTIMEOUTB = 0; }
					if (debugEN == 1) { DEBUG.println(F("POWER STAGE TWO COMPLETE")); DEBUG.println(); }
				}
				PWRstage = SENSOR3;
			}

			else if (PWRstage == SENSOR2) { PWRstage = SENSOR3; }

			//Sensor 1 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (digitalRead(sense1PWR) == LOW && bitRead(faultBITS, 15) == 1 && FAULTretrySENSOR1 < 3) {
				if (debugEN == 1) { DEBUG.println(F("RESSETTING OVERCURRENT IC 1")); }
				digitalWrite(sense1PWR, HIGH);
			}

			if (digitalRead(sense1PWR) == HIGH && PWRstage == SENSOR1) {
				if (numSensors > 0) {
					digitalWrite(sense1PWR, LOW);
					//if (responseTIMEOUTA != 0) { responseTIMEOUTA = 0; }
					if (debugEN == 1) { DEBUG.println(F("POWER STAGE ONE COMPLETE")); DEBUG.println(); }
				}
				PWRstage = SENSOR2;
			}

			else if (PWRstage == SENSOR1) { PWRstage = SENSOR2; }

			//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			if (PWRstage == OFF) { PWRstage = SENSOR1; } //inital delay between powerup and first sensor.

		}

		powercycletimer = millis();
	}
	
	//Detect SD card not inserted or removed **************************************************************************************************************************************************************************************

	if (digitalRead(A10) == HIGH && SDinserted == 1) {
		SDinserted = 0;
		bitWrite(faultBITS, 1, 1); //SD Fault
		if (debugEN == 1) { DEBUG.println(F("ERROR - NO SD Card inserted!")); }

	}

	//Sensor purge local indication**************************************************************************************************************************************************************************************

	if ((sensor1.purgestatus == 1 || sensor2.purgestatus == 1 || sensor3.purgestatus == 1) && digitalRead(YELLOW_LED) == LOW) {
		digitalWrite(YELLOW_LED, HIGH);
	}

	if (sensor1.purgestatus == 0 && sensor2.purgestatus == 0 && sensor3.purgestatus == 0 && digitalRead(YELLOW_LED) == HIGH) {
		digitalWrite(YELLOW_LED, LOW);
	}

}


