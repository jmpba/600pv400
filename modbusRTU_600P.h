// modbusRTU_600P.h

#ifndef _MODBUSRTU_600P_h
#define _MODBUSRTU_600P_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
	#include "main.h"
#else
	#include "WProgram.h"
#endif

#define ku8MaxBufferSize                 192   ///< size of response/transmit buffers    

// Modbus function codes for bit access
#define ku8MBReadCoils 0x01 // Modbus function 0x01 Read Coils
#define ku8MBReadDiscreteInputs 0x02 //< Modbus function 0x02 Read Discrete Inputs
#define ku8MBWriteSingleCoil 0x05 //< Modbus function 0x05 Write Single Coil
#define ku8MBWriteMultipleCoils 0x0F //< Modbus function 0x0F Write Multiple Coils

// Modbus function codes for 16 bit access
#define ku8MBReadHoldingRegisters 0x03 //< Modbus function 0x03 Read Holding Registers
#define ku8MBReadInputRegisters 0x04 //< Modbus function 0x04 Read Input Registers
#define ku8MBWriteSingleRegister 0x06 //< Modbus function 0x06 Write Single Register
#define ku8MBWriteMultipleRegisters 0x10 //< Modbus function 0x10 Write Multiple Registers
#define ku8MBMaskWriteRegister 0x16 //< Modbus function 0x16 Mask Write Register
#define ku8MBReadWriteMultipleRegisters 0x17 //< Modbus function 0x17 Read Write Multiple Registers
#define ku8MBReadDeviceIdentification 0x2B //< Modbus function 0x2B Read Device Identification

// Modbus timeout [milliseconds]
#define ku16MBResponseTimeout 500  //< Modbus timeout [milliseconds]

static const uint8_t ku8MBIllegalFunction = 0x01;
static const uint8_t ku8MBIllegalDataAddress = 0x02;
static const uint8_t ku8MBIllegalDataValue = 0x03;
static const uint8_t ku8MBSlaveDeviceFailure = 0x04;
static const uint8_t ku8MBSuccess = 0x00;
static const uint8_t ku8MBInvalidSlaveID = 0xE0;
static const uint8_t ku8MBInvalidFunction = 0xE1;
static const uint8_t ku8MBResponseTimedOut = 0xE2;
static const uint8_t ku8MBInvalidCRC = 0xE3;
static const uint8_t ku8MBWaiting = 0x32;
static const uint8_t ku8MBDone = 0x33;

class ModbusRTUClass
{
 protected:

	 uint16_t _u16ReadAddress = 0;                      ///< slave register from which to read
	 uint16_t _u16ReadQty = 0;                          ///< quantity of words to read
	 uint8_t  _u8MBSlave = 0;                           ///< Modbus slave (1..255)
	 uint8_t u8MBFunction = 0;
	 uint16_t _u16WriteQty = 0;                         ///< quantity of words to write
	 uint16_t _u16WriteAddress = 0;                     ///< slave register to which to write
	 uint16_t _u16TransmitBuffer[ku8MaxBufferSize]; ///< buffer containing data to transmit to Modbus slave; set via SetTransmitBuffer()
	 uint16_t _u16MEItype = 0;                          ///< MEI type for device info
	 uint16_t _u16DEVICEidTYPE = 0;                     ///< Device ID type for device info
	 uint16_t _u16ResponseBuffer[ku8MaxBufferSize]; ///< buffer to store Modbus slave response; read via GetResponseBuffer()
	 uint8_t transactionstep = 0;
	 uint32_t transactiontimer = 0;
	 uint8_t tempvaluee = 0;
	 uint8_t tempvaluee2 = 0;
	 uint8_t tempvaluee3 = 0;

	 uint8_t ModbusReadStatus = 255;
	 /* ModbusReadStatus
	 bit0 - Request sent to slave
	 bit1 - Slave response timeout
	 bit2 - dataProcess
	 bit3 -
	 bit4 -
	 bit5 -
	 bit6 -
	 bit7 -
	 */

	 uint8_t u8ModbusADU[256];
	 uint8_t u8ModbusADUSize = 0;
	 uint8_t i, u8Qty = 0;
	 uint16_t u16CRC = 0;
	 uint32_t u32StartTime = 0;
	 uint8_t u8BytesLeft = 0;
	 
	 uint8_t regno = 0;
	 uint32_t timeout_timestamp = 0;
	 uint16_t IDindex[10];
	 uint16_t Lengthindex[10];
	 uint8_t _u8ResponseBufferLength;

 public:

	 uint8_t u8MBStatus = 0;

	void init();
	void readHoldingRegisters(uint8_t slaveID, uint16_t u16ReadAddress, uint16_t u16ReadQty);
	void readInputRegisters(uint8_t slaveID, uint16_t u16ReadAddress, uint16_t u16ReadQty);
	void writeSingleRegister(uint8_t slaveID, uint16_t u16WriteAddress, uint16_t u16WriteValue);
	uint8_t setTransmitBuffer(uint8_t u8Index, uint16_t u16Value);
	void writeMultipleRegisters(uint8_t slaveID, uint16_t u16WriteAddress, uint16_t u16WriteQty);
	void writeSingleCoil(uint8_t slaveID, uint16_t u16WriteAddress, uint8_t u8State);
	void readDeviceIdentification(uint8_t slaveID, uint16_t u16ReadAddress, uint16_t u16MEItype, uint16_t u16DEVICEidTYPE);
	uint16_t getResponseBuffer(uint8_t u8Index);
	void ModbusMasterTransaction(void);
	void process_slave_response(void);
	uint16_t getCode2BResponse(uint8_t u8Index, char* theDATAbuffer, uint8_t u8Length);
	void print_the_error(uint8_t locationID);
	uint16_t crc16_update(uint16_t crc, uint8_t a);

};

extern ModbusRTUClass modbusRTU;

#endif

