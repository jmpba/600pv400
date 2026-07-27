// 
// 
// 

#include "SD_600P.h"

SDcardClass SDcard;

void SDcardClass::init()
{
	snprintf(filename, 16, "%s%lu%s", "L", PCBserialno, ".txt"); //assemble log filename

	if (SD.begin(4)) { //init OK

	}

	else { //initialization fail
		bitWrite(faultBITS, 1, 1); //SD Fault
	}
}

void SDcardClass::logEVENT(const char* data) {//SYSTEM EVENTS GET LOGGED TO SD CARD HERE.

	File File1;

	char recordbuffer[256];

	//Clock();//UPDATE TIME

	if (debugEN == 1) { 
		DEBUG.print(F("LOGGING EVENT - "));
		DEBUG.println(data);
	}

	if (IO.SDinserted == 1) {

		time_t rawtime = DS1338.epoch;
		struct tm  ts;
		char       TimeString[80];
		ts = *localtime(&rawtime);
		strftime(TimeString, sizeof(TimeString), "%a %Y-%m-%d %H:%M:%S %Z", &ts);

		snprintf(recordbuffer, 99, "%s %s", data, TimeString);


		if (SD.exists(filename)) {
			File1 = SD.open(filename, FILE_WRITE);
			if (File1) { File1.println(recordbuffer); }
			else if (debugEN == 1) { DEBUG.println(F("SD Error")); }
			File1.close();
		}

		if (!SD.exists(filename)) {
			File1 = SD.open(filename, FILE_WRITE); //tests if file exists. If no file found attempts to create it.
			if (debugEN == 1) {
				DEBUG.print(F("CREATING NEW LOGFILE... "));
				DEBUG.println(filename);
			}

			if (File1) {
				File1.print(F("PCB S/N "));
				File1.println(PCBserialno);
				File1.print(F("FILE CREATED ON "));
				File1.println(TimeString);
				File1.println();
				File1.println(recordbuffer);
			}

			File1.close();
		}

		if (!SD.exists(filename)) {
			if (debugEN == 1) { DEBUG.println(F("error reading SD card file1")); } // tests again to see if file exists or if file creation was succesful if file wasnt found in line above.
			return;
		}  //code stops here if file wasnt found or created.  

	}

	else if (debugEN == 1) { DEBUG.println(F("NO SD CARD INSERTED")); }
}



