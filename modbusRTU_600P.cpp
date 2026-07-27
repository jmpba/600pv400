// 
// 
// 

#include "modbusRTU_600P.h"

ModbusRTUClass modbusRTU;

void ModbusRTUClass::init()
{


}

void ModbusRTUClass::readHoldingRegisters(uint8_t slaveID, uint16_t u16ReadAddress, uint16_t u16ReadQty) {

	_u8MBSlave = slaveID;
	_u16ReadAddress = u16ReadAddress;
	_u16ReadQty = u16ReadQty;
	u8MBFunction = ku8MBReadHoldingRegisters;
	//ModbusMasterTransaction();
	transactionstep = 1;
}

void ModbusRTUClass::readInputRegisters(uint8_t slaveID, uint16_t u16ReadAddress, uint16_t u16ReadQty) {

	_u8MBSlave = slaveID;
	_u16ReadAddress = u16ReadAddress;
	_u16ReadQty = u16ReadQty;
	u8MBFunction = ku8MBReadInputRegisters;
	//ModbusMasterTransaction();
	transactionstep = 1;
}

void ModbusRTUClass::writeSingleRegister(uint8_t slaveID, uint16_t u16WriteAddress, uint16_t u16WriteValue)
{
	_u8MBSlave = slaveID;
	_u16WriteAddress = u16WriteAddress;
	_u16WriteQty = 0;
	_u16TransmitBuffer[0] = u16WriteValue;
	u8MBFunction = ku8MBWriteSingleRegister;
	//ModbusMasterTransaction();
	transactionstep = 1;
}

uint8_t ModbusRTUClass::setTransmitBuffer(uint8_t u8Index, uint16_t u16Value)
{
	if (u8Index < ku8MaxBufferSize)
	{
		_u16TransmitBuffer[u8Index] = u16Value;
		return ku8MBSuccess;
	}
	else
	{
		return ku8MBIllegalDataAddress;
	}
}

void ModbusRTUClass::writeMultipleRegisters(uint8_t slaveID, uint16_t u16WriteAddress, uint16_t u16WriteQty) {

	_u8MBSlave = slaveID;
	_u16WriteAddress = u16WriteAddress;
	_u16WriteQty = u16WriteQty;
	u8MBFunction = ku8MBWriteMultipleRegisters;
	//ModbusMasterTransaction();
	transactionstep = 1;
}

void ModbusRTUClass::writeSingleCoil(uint8_t slaveID, uint16_t u16WriteAddress, uint8_t u8State)
{
	_u8MBSlave = slaveID;
	_u16WriteAddress = u16WriteAddress;
	_u16WriteQty = (u8State ? 0xFF00 : 0x0000);
	u8MBFunction = ku8MBWriteSingleCoil;
	//ModbusMasterTransaction();
	transactionstep = 1;
}

void ModbusRTUClass::readDeviceIdentification(uint8_t slaveID, uint16_t u16ReadAddress, uint16_t u16MEItype, uint16_t u16DEVICEidTYPE) {

	_u8MBSlave = slaveID;
	_u16ReadAddress = u16ReadAddress;
	_u16MEItype = u16MEItype;
	_u16DEVICEidTYPE = u16DEVICEidTYPE;
	u8MBFunction = ku8MBReadDeviceIdentification;
	//ModbusMasterTransaction();
	transactionstep = 1;
}

uint16_t ModbusRTUClass::getResponseBuffer(uint8_t u8Index)
{
	if (u8Index < ku8MaxBufferSize)
	{
		return _u16ResponseBuffer[u8Index];
	}
	else
	{
		return 0xFFFF;
	}
}

void ModbusRTUClass::ModbusMasterTransaction(void) {

	switch (transactionstep) {

	case 1:

		ModbusReadStatus = 255; //reset control register
		u8ModbusADUSize = 0;
		i = 0;
		u8Qty = 0;
		u16CRC = 0;
		u32StartTime = 0;
		u8BytesLeft = 8;
		u8MBStatus = ku8MBWaiting;
		regno = 0;

		// assemble Modbus Request Application Data Unit
		u8ModbusADU[u8ModbusADUSize++] = _u8MBSlave;
		u8ModbusADU[u8ModbusADUSize++] = u8MBFunction;

		switch (u8MBFunction)
		{
		case ku8MBReadCoils:
		case ku8MBReadDiscreteInputs:
		case ku8MBReadInputRegisters:
		case ku8MBReadHoldingRegisters:
		case ku8MBReadWriteMultipleRegisters:
			u8ModbusADU[u8ModbusADUSize++] = highByte(_u16ReadAddress);
			u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16ReadAddress);
			u8ModbusADU[u8ModbusADUSize++] = highByte(_u16ReadQty);
			u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16ReadQty);

			break;
		}

		switch (u8MBFunction)
		{
		case ku8MBReadDeviceIdentification:
			u8ModbusADU[u8ModbusADUSize++] = _u16MEItype;
			u8ModbusADU[u8ModbusADUSize++] = _u16DEVICEidTYPE;
			u8ModbusADU[u8ModbusADUSize++] = _u16ReadAddress;
			break;
		}

		switch (u8MBFunction)
		{
		case ku8MBWriteSingleCoil:
		case ku8MBMaskWriteRegister:
		case ku8MBWriteMultipleCoils:
		case ku8MBWriteSingleRegister:
		case ku8MBWriteMultipleRegisters:
		case ku8MBReadWriteMultipleRegisters:
			u8ModbusADU[u8ModbusADUSize++] = highByte(_u16WriteAddress);
			u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16WriteAddress);
			break;
		}

		switch (u8MBFunction)
		{
		case ku8MBWriteSingleCoil:
			u8ModbusADU[u8ModbusADUSize++] = highByte(_u16WriteQty);
			u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16WriteQty);
			break;

		case ku8MBWriteSingleRegister:
			u8ModbusADU[u8ModbusADUSize++] = highByte(_u16TransmitBuffer[0]);
			u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16TransmitBuffer[0]);
			break;

		case ku8MBWriteMultipleCoils:
			u8ModbusADU[u8ModbusADUSize++] = highByte(_u16WriteQty);
			u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16WriteQty);
			u8Qty = (_u16WriteQty % 8) ? ((_u16WriteQty >> 3) + 1) : (_u16WriteQty >> 3);
			u8ModbusADU[u8ModbusADUSize++] = u8Qty;
			for (i = 0; i < u8Qty; i++)
			{
				switch (i % 2)
				{
				case 0: // i is even
					u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16TransmitBuffer[i >> 1]);
					break;

				case 1: // i is odd
					u8ModbusADU[u8ModbusADUSize++] = highByte(_u16TransmitBuffer[i >> 1]);
					break;
				}
			}
			break;

		case ku8MBWriteMultipleRegisters:
		case ku8MBReadWriteMultipleRegisters:
			u8ModbusADU[u8ModbusADUSize++] = highByte(_u16WriteQty);
			u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16WriteQty);
			u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16WriteQty << 1);

			for (i = 0; i < lowByte(_u16WriteQty); i++)
			{
				u8ModbusADU[u8ModbusADUSize++] = highByte(_u16TransmitBuffer[i]);
				u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16TransmitBuffer[i]);
			}
			break;

		case ku8MBMaskWriteRegister:
			u8ModbusADU[u8ModbusADUSize++] = highByte(_u16TransmitBuffer[0]);
			u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16TransmitBuffer[0]);
			u8ModbusADU[u8ModbusADUSize++] = highByte(_u16TransmitBuffer[1]);
			u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16TransmitBuffer[1]);
			break;
		}

		// append CRC
		u16CRC = 0xFFFF;
		for (i = 0; i < u8ModbusADUSize; i++)
		{
			u16CRC = crc16_update(u16CRC, u8ModbusADU[i]);
		}

		u8ModbusADU[u8ModbusADUSize++] = lowByte(u16CRC);
		u8ModbusADU[u8ModbusADUSize++] = highByte(u16CRC);
		u8ModbusADU[u8ModbusADUSize] = 0;

		if (_u8MBSlave == sensor1.slaveID) { digitalWrite(rs4851REDE, HIGH); } //Set IL3585E transmit mode
		if (_u8MBSlave == sensor2.slaveID) { digitalWrite(rs4852REDE, HIGH); }//Set IL3585E transmit mode
		if (_u8MBSlave == sensor3.slaveID) { digitalWrite(rs4853REDE, HIGH); }//Set IL3585E transmit mode

		//digitalWrite(rs4851REDE, LOW);
		//digitalWrite(rs4852REDE, LOW);

		transactiontimer = millis();
		transactionstep++;

		break;

	case 2:

		if ((millis() - transactiontimer) >= 10) {

			/* 
			this 10ms delay is needed as switching all three IL3585E chips from recieve to transmit mode can put random bytes into the recieve 
			buffer which corrupts the sensor response and throws an invalid slave ID error. 10ms is requred to allow the data bus to settle 
			and clear after switching the REDE output.		
			*/

			while (RS485.read() != -1); // flush receive buffer before transmitting request

			/*
			if (RS485.available() > 0) {
				tempvaluee3 = 1;
				DEBUG.print(F("flush: "));
			}
			while (RS485.available() > 0) {
				DEBUG.print(RS485.read(), HEX);
				DEBUG.print(F(" "));
			}
			if (tempvaluee3 == 1) {
				DEBUG.println();

			}
			*/

			for (i = 0; i < u8ModbusADUSize; i++)
			{
				RS485.write(u8ModbusADU[i]);
			}

			u8ModbusADUSize = 0;

			RS485.flush(); // flush transmit buffer

			if (_u8MBSlave == sensor1.slaveID) {
				digitalWrite(rs4851REDE, LOW); //Set IL3585E Receive mode
				digitalWrite(rs4852REDE, HIGH); //Set IL3585E Transmit mode
				digitalWrite(rs4853REDE, HIGH); //Set IL3585E Transmit mode
			}
			if (_u8MBSlave == sensor2.slaveID) {
				digitalWrite(rs4852REDE, LOW); //Set IL3585E Receive mode
				digitalWrite(rs4851REDE, HIGH); //Set IL3585E Transmit mode
				digitalWrite(rs4853REDE, HIGH); //Set IL3585E Transmit mode
			}
			if (_u8MBSlave == sensor3.slaveID) {
				digitalWrite(rs4853REDE, LOW); //Set IL3585E Receive mode
				digitalWrite(rs4851REDE, HIGH); //Set IL3585E Transmit mode
				digitalWrite(rs4852REDE, HIGH); //Set IL3585E Transmit mode
			}

			bitClear(ModbusReadStatus, 0); //request sent bit
			transactionstep = 0;
		}

		if (millis() < transactiontimer) { transactiontimer = millis(); } //reset in case ul_tickcount rolls over to 0
	}
}


void ModbusRTUClass::process_slave_response(void) {

	if (bitRead(ModbusReadStatus, 0) == 0) {
		timeout_timestamp = millis();
		bitSet(ModbusReadStatus, 0);
		bitClear(ModbusReadStatus, 1);
	}

	if (bitRead(ModbusReadStatus, 1) == 0) {
		if ((millis() - timeout_timestamp) > ku16MBResponseTimeout) { //Slave response timeout
			u8MBStatus = ku8MBResponseTimedOut;
			ModbusReadStatus = 255; //reset control register
		}
		if ((millis() - timeout_timestamp) < 0) { //reset after rollover
			timeout_timestamp = millis();
		}
	}

	if ((u8BytesLeft > 0) && (u8MBStatus == ku8MBWaiting) && (bitRead(ModbusReadStatus, 1) == 0)) {

		while (RS485.available() > 0) { //while there is unread data in the ring buffer
			//ringbuffer_get(&modbus.io_desc->rx, &u8ModbusADU[u8ModbusADUSize]);
			u8ModbusADU[u8ModbusADUSize] = RS485.read();

			if (tempvaluee2 == 0) { tempvaluee2 = 1; }
			//DEBUG.print(u8ModbusADU[u8ModbusADUSize], HEX);
			//DEBUG.print(F(" "));

			u8ModbusADUSize++;
			if (u8BytesLeft > 0) { u8BytesLeft--; }

			if (u8BytesLeft == 1 && u8ModbusADU[1] == ku8MBReadDeviceIdentification) {
				IDindex[regno] = u8ModbusADU[u8ModbusADUSize - 1];
			}

			if (u8BytesLeft == 0 && u8ModbusADU[1] == ku8MBReadDeviceIdentification) {
				Lengthindex[regno] = u8ModbusADU[u8ModbusADUSize - 1];
				if (regno < 9 && regno < u8ModbusADU[7]) { regno++; }

				if (u8Qty != u8ModbusADU[7]) {
					if (u8Qty == 0) { u8BytesLeft = u8ModbusADU[9] + 2; }
					else { u8BytesLeft = u8ModbusADU[u8ModbusADUSize - 1] + 2; }
					u8Qty++;
				}
			}

			// evaluate slave ID, function code once enough bytes have been read
			if (u8ModbusADUSize == 5)
			{

				// verify response is for correct Modbus slave
				if (u8ModbusADU[0] != _u8MBSlave)
				{
					u8MBStatus = ku8MBInvalidSlaveID;
					break;
				}

				// verify response is for correct Modbus function code (mask exception bit 7)
				if ((u8ModbusADU[1] & 0x7F) != u8MBFunction)
				{
					u8MBStatus = ku8MBInvalidFunction;
					break;
				}

				// check whether Modbus exception occurred; return Modbus Exception Code
				if (bitRead(u8ModbusADU[1], 7))
				{
					u8MBStatus = u8ModbusADU[2];
					break;
				}

				// evaluate returned Modbus function code
				switch (u8ModbusADU[1])
				{
				case ku8MBReadCoils:
				case ku8MBReadDiscreteInputs:
				case ku8MBReadInputRegisters:
				case ku8MBReadHoldingRegisters:
				case ku8MBReadWriteMultipleRegisters:
					u8BytesLeft = u8ModbusADU[2];
					break;

				case ku8MBWriteSingleCoil:
				case ku8MBWriteMultipleCoils:
				case ku8MBWriteSingleRegister:
				case ku8MBWriteMultipleRegisters:
					u8BytesLeft = 3;
					break;

				case ku8MBMaskWriteRegister:
					u8BytesLeft = 5;
					break;

				case ku8MBReadDeviceIdentification:

					u8BytesLeft = 5;
					break;
				}
			}
		}


	}

	else {

		while (RS485.read() != -1); // flush receive buffer of unwanted data
	}

	if (u8BytesLeft == 0) {
		bitSet(ModbusReadStatus, 1);
		bitClear(ModbusReadStatus, 2);
	}

	// verify response is large enough to inspect further
	if ((u8MBStatus == ku8MBWaiting) && (u8ModbusADUSize >= 5) && (bitRead(ModbusReadStatus, 2) == 0))
	{
		// calculate CRC
		u16CRC = 0xFFFF;

		for (i = 0; i < (u8ModbusADUSize - 2); i++)
		{
			u16CRC = crc16_update(u16CRC, u8ModbusADU[i]);
		}

		// verify CRC
		if ((u8MBStatus == ku8MBWaiting) && (lowByte(u16CRC) != u8ModbusADU[u8ModbusADUSize - 2] ||
			highByte(u16CRC) != u8ModbusADU[u8ModbusADUSize - 1]))
		{
			u8MBStatus = ku8MBInvalidCRC;
		}
	}

	// disassemble ADU into words
	if ((u8MBStatus == ku8MBWaiting) && (bitRead(ModbusReadStatus, 2) == 0))
	{
		// evaluate returned Modbus function code

		switch (u8ModbusADU[1])
		{
		case ku8MBReadCoils:
		case ku8MBReadDiscreteInputs:
			// load bytes into word; response bytes are ordered L, H, L, H, ...
			for (i = 0; i < (u8ModbusADU[2] >> 1); i++)
			{
				if (i < ku8MaxBufferSize)
				{
					_u16ResponseBuffer[i] = ((u8ModbusADU[2 * i + 4]) << 8 | (u8ModbusADU[2 * i + 3]));
				}

				_u8ResponseBufferLength = i;
			}

			// in the event of an odd number of bytes, load last byte into zero-padded word
			if (u8ModbusADU[2] % 2)
			{
				if (i < ku8MaxBufferSize)
				{
					_u16ResponseBuffer[i] = ((0) << 8 | (u8ModbusADU[2 * i + 3]));
				}

				_u8ResponseBufferLength = i + 1;
			}
			break;

		case ku8MBReadInputRegisters:
		case ku8MBReadHoldingRegisters:
		case ku8MBReadWriteMultipleRegisters:
			// load bytes into word; response bytes are ordered H, L, H, L, ...

			for (i = 0; i < (u8ModbusADU[2] >> 1); i++)
			{
				if (i < ku8MaxBufferSize)
				{
					_u16ResponseBuffer[i] = ((u8ModbusADU[2 * i + 3]) << 8 | (u8ModbusADU[2 * i + 4]));
				}

				_u8ResponseBufferLength = i;
			}

			break;

		case ku8MBReadDeviceIdentification:
			for (i = 0; i < (u8ModbusADUSize - 2); i++)
			{
				_u16ResponseBuffer[i] = u8ModbusADU[i];
			}

			break;
		}
	}

	if (u8MBStatus == ku8MBWaiting && bitRead(ModbusReadStatus, 2) == 0) { u8MBStatus = ku8MBSuccess; }

	if (u8MBStatus != ku8MBWaiting) { 
		ModbusReadStatus = 255;  //reset control register
		if (digitalRead(rs4851REDE) == HIGH) { digitalWrite(rs4851REDE, LOW); }//Set IL3585E Receive mode (LOW power)
		if (digitalRead(rs4852REDE) == HIGH) { digitalWrite(rs4852REDE, LOW); }//Set IL3585E Receive mode (LOW power)
		if (digitalRead(rs4853REDE) == HIGH) { digitalWrite(rs4853REDE, LOW); }//Set IL3585E Receive mode (LOW power)
		//DONT LEAVE REDE PINS HIGH AS THEY OVERHEAT THE 24/5V REGULATOR!!!!	
	}	  

}

uint16_t ModbusRTUClass::getCode2BResponse(uint8_t u8Index, char* theDATAbuffer, uint8_t u8Length)
{
	uint8_t i = 0;

	if (u8Index == IDindex[0])
	{
		for (i = 0; i < Lengthindex[0]; i++)
		{
			if (i < u8Length) {
				theDATAbuffer[i] = _u16ResponseBuffer[(i + 10)];
			}
		}
		theDATAbuffer[i] = 0;
		return 0;
	}

	if (_u16ResponseBuffer[7] > 1) { //if there is more than 1 register

		if (u8Index == IDindex[1])
		{
			for (i = 0; i < Lengthindex[1]; i++)
			{
				if (i < u8Length) {
					theDATAbuffer[i] = _u16ResponseBuffer[i + 12 + Lengthindex[0]];
				}
			}
			theDATAbuffer[i] = 0;
			return 0;
		}
	}

	if (_u16ResponseBuffer[7] > 2) { //if there is more than 1 register

		if (u8Index == IDindex[2])
		{
			for (i = 0; i < Lengthindex[2]; i++)
			{
				if (i < u8Length) {
					theDATAbuffer[i] = _u16ResponseBuffer[i + 14 + Lengthindex[1] + Lengthindex[0]];
				}
			}
			theDATAbuffer[i] = 0;
			return 0;
		}
	}

	if (_u16ResponseBuffer[7] > 3) { //if there is more than 1 register

		if (u8Index == IDindex[3])
		{
			for (i = 0; i < Lengthindex[3]; i++)
			{
				if (i < u8Length) {
					theDATAbuffer[i] = _u16ResponseBuffer[i + 16 + Lengthindex[2] + Lengthindex[1] + Lengthindex[0]];
				}
			}
			theDATAbuffer[i] = 0;
			return 0;
		}
	}

	if (_u16ResponseBuffer[7] > 4) { //if there is more than 1 register

		if (u8Index == IDindex[4])
		{
			for (i = 0; i < Lengthindex[4]; i++)
			{
				if (i < u8Length) {
					theDATAbuffer[i] = _u16ResponseBuffer[i + 18 + Lengthindex[3] + Lengthindex[2] + Lengthindex[1] + Lengthindex[0]];
				}
			}
			theDATAbuffer[i] = 0;
			return 0;
		}
	}

	if (_u16ResponseBuffer[7] > 5) { //if there is more than 1 register

		if (u8Index == IDindex[5])
		{
			for (i = 0; i < Lengthindex[5]; i++)
			{
				if (i < u8Length) {
					theDATAbuffer[i] = _u16ResponseBuffer[i + 20 + Lengthindex[4] + Lengthindex[3] + Lengthindex[2] + Lengthindex[1] + Lengthindex[0]];
				}
			}
			theDATAbuffer[i] = 0;
			return 0;
		}
	}

	if (_u16ResponseBuffer[7] > 6) { //if there is more than 1 register

		if (u8Index == IDindex[6])
		{
			for (i = 0; i < Lengthindex[6]; i++)
			{
				if (i < u8Length) {
					theDATAbuffer[i] = _u16ResponseBuffer[i + 22 + Lengthindex[5] + Lengthindex[4] + Lengthindex[3] + Lengthindex[2] + Lengthindex[1] + Lengthindex[0]];
				}
			}
			theDATAbuffer[i] = 0;
			return 0;
		}
	}

	if (_u16ResponseBuffer[7] > 7) { //if there is more than 1 register

		if (u8Index == IDindex[7])
		{
			for (i = 0; i < Lengthindex[7]; i++)
			{
				if (i < u8Length) {
					theDATAbuffer[i] = _u16ResponseBuffer[i + 24 + Lengthindex[6] + Lengthindex[5] + Lengthindex[4] + Lengthindex[3] + Lengthindex[2] + Lengthindex[1] + Lengthindex[0]];
				}
			}
			theDATAbuffer[i] = 0;
			return 0;
		}
	}

	if (_u16ResponseBuffer[7] > 8) { //if there is more than 1 register

		if (u8Index == IDindex[8])
		{
			for (i = 0; i < Lengthindex[8]; i++)
			{
				if (i < u8Length) {
					theDATAbuffer[i] = _u16ResponseBuffer[i + 26 + Lengthindex[7] + Lengthindex[6] + Lengthindex[5] + Lengthindex[4] + Lengthindex[3] + Lengthindex[2] + Lengthindex[1] + Lengthindex[0]];
				}
			}
			theDATAbuffer[i] = 0;
			return 0;
		}
	}

	return 0xFFFF;
}

void ModbusRTUClass::print_the_error(uint8_t locationID) {

	if (debugEN == 1) {
		if (u8MBStatus == ku8MBIllegalFunction) {
			DEBUG.print(F("Illegal Function "));
			DEBUG.print(locationID);
			DEBUG.print(F(" "));
			DEBUG.println(_u8MBSlave);
		}
		if (u8MBStatus == ku8MBIllegalDataAddress) {
			DEBUG.print(F("IllegalData Address "));
			DEBUG.print(locationID);
			DEBUG.print(F(" "));
			DEBUG.println(_u8MBSlave);
		}
		if (u8MBStatus == ku8MBIllegalDataValue) {
			DEBUG.print(F("Illegal Data Value "));
			DEBUG.print(locationID);
			DEBUG.print(F(" "));
			DEBUG.println(_u8MBSlave);
		}
		if (u8MBStatus == ku8MBSlaveDeviceFailure) {
			DEBUG.print(F("Slave Device Failure "));
			DEBUG.print(locationID);
			DEBUG.print(F(" "));
			DEBUG.println(_u8MBSlave);
		}
		if (u8MBStatus == ku8MBInvalidSlaveID) {
			DEBUG.print(F("Invalid Slave ID "));
			DEBUG.print(locationID);
			DEBUG.print(F(" "));
			DEBUG.println(_u8MBSlave);
		}
		if (u8MBStatus == ku8MBInvalidFunction) {
			DEBUG.print(F("Invalid Function "));
			DEBUG.print(locationID);
			DEBUG.print(F(" "));
			DEBUG.println(_u8MBSlave);
		}
		if (u8MBStatus == ku8MBResponseTimedOut) {
			DEBUG.print(F("Response Timed Out "));
			DEBUG.print(locationID);
			DEBUG.print(F(" "));
			DEBUG.println(_u8MBSlave);
		}
		if (u8MBStatus == ku8MBInvalidCRC) {
			DEBUG.print(F("Invalid CRC "));
			DEBUG.print(locationID);
			DEBUG.print(F(" "));
			DEBUG.println(_u8MBSlave);
		}
	}
}

uint16_t ModbusRTUClass::crc16_update(uint16_t crc, uint8_t a)
{
	int i;

	crc ^= a;
	for (i = 0; i < 8; ++i)
	{
		if (crc & 1) {
			crc = (crc >> 1) ^ 0xA001;
		}
		else {
			crc = (crc >> 1);
		}
	}

	return crc;
}