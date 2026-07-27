// MQTT_600P.h

#ifndef _MQTT_600P_h
#define _MQTT_600P_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
	#include "main.h"

#else
	#include "WProgram.h"
#endif
/* Packet types */
#define CONNECT_PACKET_TYPE			1 << 4 //(0x10)
#define CONNACK_PACKET_TYPE			2 << 4 //(0x20)
#define PUBLISH_PACKET_TYPE			3 << 4 //(0x30)
#define PUBLISH_ACK_PACKET_TYPE		4 << 4 //(0x40)
#define SUBSCRIBE_PACKET_TYPE		0x82 //(0x82)
#define SUBACK_PACKET_TYPE			9 << 4 //(0x90)
#define PING_REQUEST_PACKET_TYPE	12 << 4 //(0x120)
#define PING_RESPONSE_PACKET_TYPE	13 << 4 //(0x130)
#define DISCONNECT_PACKET_TYPE		14 << 4 //(0x140)
#define sizeofpage 64

/* MQTT Header Control Flags */
#define USER_NAME_SET 1 << 7
#define PASSWORD_SET 1 << 6
#define WILL_RETAIN_SET 1 << 5
#define QOS_ZERO 0
#define QOS_ONE 1 << 1
#define QOS_TWO 1 << 2
#define WILL_FLAG 1 << 2
#define CLEAN_SESSION 1 << 1
#define DUP_FLAG 1 << 3

#define MQTT_DATA_STRING_BUFF_SIZE 128
#define MQTT_KEEP_ALIVE 60  // (15mins) seconds to keep TCP socket open before declaring client dead
#define MQTT_VERSION 4 // 4 = v3.1.1

#define buffer_size 512

#define SOCKETNOTDEFINED 0
#define SOCKETDEFINEDNOTUSED 1
#define SOCKETOPENING 2
#define SOCKETREADY 3
#define SOCKETCLOSING 4
#define SOCKETCLOSED 5

const uint8_t MQTT_float = 1;
const uint8_t MQTT_uint32_t = 2;
const uint8_t MQTT_uint16_t = 3;
const uint8_t MQTT_char = 4;
const uint8_t MQTT_uint8_t = 5;
const uint8_t MQTT_int8_t = 6;

typedef enum MQTT_ACK_Type_t {
	RESET, CONACK, PUBACK, ACK_ERR, SUBACK
} MQTT_ACK_Type;

typedef enum MQTT_RETURN_Code_t {
	ACCEPTED, UNACCEPTABLE_PROTOCOL, IDENTIFIER_REJECTED, SERVER_UNAVAILABLE, BAD_USER_PASSWORD, NOT_AUTHORIZED
} MQTT_RETURN_Code;

class MQTTClass
{
 protected:


 public:

	 volatile uint8_t mqtt_tcp_session_id = 0;
	 volatile uint8_t TCPsocketSTATUS = 0;
	 char send_buff[buffer_size] = { '\0' };
	 bool response_expected = false;
	 uint8_t data_valid = 0;
	 uint16_t message_id = 0;
	 uint8_t send_attempt = 0;
	 uint16_t published_ID = 0;
	 volatile uint32_t modem_SERVER_timeout = 0;
	 char MQTT_USERNAME[sizeofpage] = { '\0' };
	 char MQTT_TOPIC[sizeofpage] = { '\0' };
	 char MQTT_CLIENT_ID[16] = { '\0' };
	 uint8_t dequeued_data[16] = { '\0' };

	 void init();
	 int get_mqtt_connect_msg(char* connect_msg_buff, int buff_size);
	 int get_mqtt_pub_message(char* msg_buffer, int buff_size, const char* topic, const char* message, int qos, int duplicate, uint16_t new_message_id);
	 int get_mqtt_subscribe_message(char* msg_buffer, int buff_size, const char* topic, int qos, uint16_t packet_identifier);
	 int get_mqtt_disconnect_msg(char* connect_msg_buff, int buff_size);
	 void uint16_t_to_two_chars(uint16_t value, char* upper_byte, char* lower_byte);
	 int random_num(int min, int max);
	 void parse_mqtt_response(int buff_size, char* buff);
	 void queue_MQTT_update(uint32_t data, const char* data_name);
	 void queue_MQTT_update(uint16_t data, const char* data_name);
	 void queue_MQTT_update(uint8_t data, const char* data_name);
	 void queue_MQTT_update(int8_t data, const char* data_name);
	 void queue_MQTT_update(float data, const char* data_name);
	 void queue_MQTT_update(char* data, const char* data_name);
	 void queue_MQTT_TIMED_update(void);
	 void dequeue_MQTT_update();
	 void queue_peek();
	 uint8_t queue_isEmpty();
	 uint8_t queue_isFull();
	 uint16_t queue_size();
	 
};

extern MQTTClass MQTT;


// Response type for parsing function return value 
typedef struct MQTT_Response_t {

	MQTT_ACK_Type ack_type = RESET;
	uint16_t message_id = 0;
	uint16_t bytes_read = 0;
	MQTT_RETURN_Code return_code = NOT_AUTHORIZED;

} MQTT_Response;

extern MQTT_Response response;
#define QUEUE_SIZE 128

typedef struct MQTT_Queue_t {
	/*MQTT queue - each value placed into the queue takes 8 bytes of the buffer defined below.
	*/
	char buffer[1024] = { '\0' };
	uint16_t front = 0;
	uint16_t rear = 0;
	uint16_t itemCount = 0;
	uint8_t next_id = 0;

} MQTT_Queue;

extern MQTT_Queue send_queue;

#endif

