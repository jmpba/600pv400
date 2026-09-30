// 
// 
// 

#include "MQTT_600P.h"

MQTTClass MQTT;

MQTT_Response response;
MQTT_Queue send_queue;

void MQTTClass::init()
{

	//sprintf(MQTT_USERNAME, "%s", "a62c724c-8bfe-13ac");
	sprintf(MQTT_USERNAME, "%s", "RElOgfn7ZE");   // or the real Kaa username/token value
	
	//uint32_t sn = PCBserialno;
	// sprintf(MQTT_CLIENT_ID, "%lu", PCBserialno);
	sprintf(MQTT_CLIENT_ID, "%s", "600P_TEST");

	//char topic[] = "v1/devices/me/rpc/request/+";
	// sprintf(MQTT_TOPIC, "%s", "5123b3bb66990ea1463fb5a4147065d461a7");
	//KaaIoT topic: sprintf(MQTT_TOPIC, "%s", kp1/{app_version_name}/{extension_instance_name}/{endpoint_token}/{resource_path}[/{request_id}]);
	sprintf(MQTT_TOPIC, "%s", "kp1/d6eaql2hducs7395o9ng-v1/dcx/RElOgfn7ZE/json/1");


}			                 //5123b3bb66990ea1463fb5a4147065d461a7


/**
*   \brief Fill passed in buffer with a valid mqtt connect message
*
*   Function will use the global variable MQTT_USERNAME as the username
*   Function will use the global variable MQTT version as version
*   Clean session and username flags will be set.
*
*   \return Length of message in bytes.
*/
int MQTTClass::get_mqtt_connect_msg(char* connect_msg_buff, int buff_size) {

	memset(connect_msg_buff, 0, buff_size);
	connect_msg_buff[buff_size - 1] = '\0';

	connect_msg_buff[0] = CONNECT_PACKET_TYPE;
	// next byte is message len (not yet known)
	// next two bytes are protocol len = 4 (M Q T T)
	char body[128]; // temp storage for body of message
	char temp_lower_byte;
	char temp_upper_byte;
	memset(body, 0, 128);
	int body_index = 0;

	uint16_t protocol_name_length = 4; // 'M' 'Q' 'T' 'T'
	uint16_t_to_two_chars(protocol_name_length, &temp_upper_byte, &temp_lower_byte);
	body_index += sprintf(&body[body_index], "%c%c", temp_upper_byte, temp_lower_byte);
	body_index += sprintf(&body[body_index], "MQTT");


	uint8_t protocol_version = MQTT_VERSION;
	body_index += sprintf(&body[body_index], "%c", (char)protocol_version);
//changed to comply with kaaIoT protocols
	// body[body_index++] = USER_NAME_SET | CLEAN_SESSION; //SET FLAGS HERE
	body[body_index++] = CLEAN_SESSION; //SET FLAGS HERE

	uint16_t keep_alive = MQTT_KEEP_ALIVE;
	uint16_t_to_two_chars(keep_alive, &temp_upper_byte, &temp_lower_byte);
	body_index += sprintf(&body[body_index], "%c%c", temp_upper_byte, temp_lower_byte);

	uint16_t client_id_len = strlen(MQTT_CLIENT_ID);
	uint16_t_to_two_chars(client_id_len, &temp_upper_byte, &temp_lower_byte);
	body_index += sprintf(&body[body_index], "%c%c%s", temp_upper_byte, temp_lower_byte, MQTT_CLIENT_ID);
	printf("MQTT USERNAME: %s\r\n", MQTT_USERNAME);
	// commented out to comply with kaaIoT protocols
	uint16_t username_len = strlen(MQTT_USERNAME);
	uint16_t_to_two_chars(username_len, &temp_upper_byte, &temp_lower_byte);
	body_index += sprintf(&body[body_index], "%c%c%s", temp_upper_byte, temp_lower_byte, MQTT_USERNAME);

	// Now we have the message len, we can fill it in (in the header)
	connect_msg_buff[1] = (char)body_index;
	memcpy(&connect_msg_buff[2], body, (size_t)body_index);

	return body_index + 2;
}


/**
*    \brief Fill buffer with a valid MQTT publish message.
*
*   \warning The function relies on strlen for the data length. As a result it is not
*   safe to send data that is not in a string format.
*
*   \warning Continuation bit for the message remaining length field is not implemented. This leaves
*   only 7 bits for the message length therefore any message longer than 2^7=128 bytes will
*   fail (ungracefully).
*
*   \return Length of message in bytes.
*/
int MQTTClass::get_mqtt_pub_message(char* msg_buffer, int buff_size, const char* topic, const char* message, int qos, int duplicate, uint16_t new_message_id) {

	//printf("Getting publish message\r\n");
	//char body[buffer_size] = { '\0' };
	char temp_upper_byte = '\0';
	char temp_lower_byte = '\0';
	int body_index = 0;

	/* QOS of zero ==> packet id not required ==> differnt format */
	if (qos == 0) {
		
		msg_buffer[body_index++] = PUBLISH_PACKET_TYPE | QOS_ZERO | (duplicate ? DUP_FLAG : 0);

		uint16_t message_len = strlen(topic) + strlen(message) + 2; // two bytes for topic len field.
		uint16_t message_len_temp = message_len;
		uint8_t message_count = 0;
		if (message_len <= 127) { msg_buffer[body_index++] = (char)message_len; }

		if (message_len > 127) { //if message is > 16383 bytes another function needs to be written to add a third byte to the message length!!!!

			while (message_len_temp > 127) {
				message_count++;
				message_len_temp -= 127;
			}
			body_index++;
			msg_buffer[body_index] = (char)message_count;
			body_index--;
			message_len = message_len % 128;
			message_len = message_len | 128; //set MSB bit high to indicate the message size is two bytes long ie between 127 and 16383 bytes in length
			msg_buffer[body_index] = (char)message_len;
			body_index += 2;
		}

		/*
				|  Digits  |                From                 |                   To                  |
				|----------|-------------------------------------|---------------------------------------|
				|    1     |  0 (0x00)                           |  127 (0x7F)                           |
				|	 2     |  128 (0x80, 0x01)                   |  16 383 (0xFF, 0x7F)                  |
				|    3     |  16 384 (0x80, 0x80, 0x01)          |  2 097 151 (0xFF, 0xFF, 0x7F)	     |
				|	 4     |  2 097 152 (0x80, 0x80, 0x80, 0x01) |  268 435 455 (0xFF, 0xFF, 0xFF, 0x7F) |
		*/
		uint16_t_to_two_chars(strlen(topic), &temp_upper_byte, &temp_lower_byte);
		body_index += sprintf(&msg_buffer[body_index], "%c%c", temp_upper_byte, temp_lower_byte);

		body_index += sprintf(&msg_buffer[body_index], "%s%s", topic, message);

		//memcpy(&msg_buffer[0], body, body_index);

		return body_index;
	}

	/* QOS of 1 ==> packet id required */
	if (qos == 1) {

		msg_buffer[body_index++] = PUBLISH_PACKET_TYPE | QOS_ONE | (duplicate ? DUP_FLAG : 0);

		uint16_t message_len = strlen(topic) + strlen(message) + 4; // two bytes for topic len and for two message id
		uint16_t message_len_temp = message_len;
		uint8_t message_count = 0;
		if (message_len <= 127) { msg_buffer[body_index++] = (char)message_len; }

		if (message_len > 127) { //if message is > 16383 bytes another function needs to be written to add a third byte to the message length!!!!

			while (message_len_temp > 127) {
				message_count++;
				message_len_temp -= 127;
			}
			body_index++;
			msg_buffer[body_index] = (char)message_count;
			body_index--;
			message_len = message_len % 128;
			message_len = message_len | 128; //set MSB bit high to indicate the message size is two bytes long ie between 127 and 16383 bytes in length
			msg_buffer[body_index] = (char)message_len;
			body_index += 2;
		}


		uint16_t_to_two_chars(strlen(topic), &temp_upper_byte, &temp_lower_byte);
		body_index += sprintf(&msg_buffer[body_index], "%c%c", temp_upper_byte, temp_lower_byte);

		uint16_t_to_two_chars(new_message_id, &temp_upper_byte, &temp_lower_byte);
		body_index += sprintf(&msg_buffer[body_index], "%s%c%c%s", topic, temp_upper_byte, temp_lower_byte, message);

		//memcpy(msg_buffer, body, body_index);

		return body_index;
	}

	if (qos == 2) {
		//printf("qos 2 not supported!\r\n");
		return 0;
	}
	/*
		for (int i = 0; i < body_index; i++) {
			print_byte(msg_buffer[i]);
		}
	*/
	return body_index;

}

int MQTTClass::get_mqtt_subscribe_message(char* msg_buffer, int buff_size, const char* topic, int qos, uint16_t packet_identifier) {

	//printf("Getting publish message\r\n");
	//char body[buffer_size];
	char temp_upper_byte = '\0';
	char temp_lower_byte = '\0';
	int body_index = 0;

	/* QOS of zero ==> packet id not required ==> differnt format */
	if (qos == 0) {

		msg_buffer[body_index++] = SUBSCRIBE_PACKET_TYPE;

		uint16_t message_len = strlen(topic) + 2 + 2 + 1; //topic + topic length + packet_identifier + qos
		uint16_t message_len_temp = message_len;
		uint8_t message_count = 0;
		if (message_len <= 127) { msg_buffer[body_index++] = (char)message_len; }

		if (message_len > 127) { //if message is > 16383 bytes another function needs to be written to add a third byte to the message length!!!!

			while (message_len_temp > 127) {
				message_count++;
				message_len_temp -= 127;
			}
			body_index++;
			msg_buffer[body_index] = (char)message_count;
			body_index--;
			message_len = message_len % 128;
			message_len = message_len | 128; //set MSB bit high to indicate the message size is two bytes long ie between 127 and 16383 bytes in length
			msg_buffer[body_index] = (char)message_len;
			body_index += 2;
		}

		/*
				|  Digits  |                From                 |                   To                  |
				|----------|-------------------------------------|---------------------------------------|
				|    1     |  0 (0x00)                           |  127 (0x7F)                           |
				|	 2     |  128 (0x80, 0x01)                   |  16 383 (0xFF, 0x7F)                  |
				|    3     |  16 384 (0x80, 0x80, 0x01)          |  2 097 151 (0xFF, 0xFF, 0x7F)	     |
				|	 4     |  2 097 152 (0x80, 0x80, 0x80, 0x01) |  268 435 455 (0xFF, 0xFF, 0xFF, 0x7F) |
		*/
		uint16_t_to_two_chars(packet_identifier, &temp_upper_byte, &temp_lower_byte); //add packet identifier
		body_index += sprintf(&msg_buffer[body_index], "%c%c", temp_upper_byte, temp_lower_byte);

		uint16_t_to_two_chars(strlen(topic), &temp_upper_byte, &temp_lower_byte); //add topic length
		body_index += sprintf(&msg_buffer[body_index], "%c%c", temp_upper_byte, temp_lower_byte);

		body_index += sprintf(&msg_buffer[body_index], "%s", topic);//add topic

		msg_buffer[body_index++] = qos;

		//memcpy(&msg_buffer[0], body, body_index);

		return body_index;
	}

	/* QOS of 1 ==> packet id required */
	if (qos == 1) {

		//printf("qos 2 not supported!\r\n");
		return 0;
	}

	if (qos == 2) {
		//printf("qos 2 not supported!\r\n");
		return 0;
	}
	/*
		for (int i = 0; i < body_index; i++) {
			print_byte(msg_buffer[i]);
		}
	*/
	return body_index;

}

/**
*   \brief Fill passed in buffer with a valid mqtt disconnect message
*
*
*   \return Length of message in bytes.
*/
int MQTTClass::get_mqtt_disconnect_msg(char* connect_msg_buff, int buff_size) {

	memset(connect_msg_buff, 0, buff_size);
	connect_msg_buff[buff_size - 1] = '\0';

	connect_msg_buff[0] = DISCONNECT_PACKET_TYPE;

	int body_index = 2;

	return body_index;
}

/* Convert 16 bit int to two chars */
void MQTTClass::uint16_t_to_two_chars(uint16_t value, char* upper_byte, char* lower_byte) {
	*lower_byte = value & 0xFF;
	*upper_byte = value >> 8;
}


/* random number in supplied range */
int MQTTClass::random_num(int min, int max) {
	return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

void MQTTClass::parse_mqtt_response(int buff_size, char* buff) {

	if (buff_size <= 0 || buff == NULL) { return; }

	const uint16_t buffer_capacity = sizeof(mqtt_rx_buffer);
	for (int input_index = 0; input_index < buff_size; input_index++) {
		if (mqtt_rx_length >= buffer_capacity) { mqtt_rx_length = 0; }
		mqtt_rx_buffer[mqtt_rx_length++] = (uint8_t)buff[input_index];

		while (mqtt_rx_length >= 2) {
			uint8_t header_type = mqtt_rx_buffer[0] >> 4;
			uint8_t header_flags = mqtt_rx_buffer[0] & 0x0F;
			bool valid_header = header_type > 0 && header_type < 15;
			if (header_type == 3) { valid_header = valid_header && ((header_flags >> 1) & 0x03) != 3; }
			else {
				uint8_t required_flags = (header_type == 6 || header_type == 8 || header_type == 10) ? 2 : 0;
				valid_header = valid_header && header_flags == required_flags;
			}
			if (!valid_header) {
				mqtt_rx_length--;
				memmove(mqtt_rx_buffer, mqtt_rx_buffer + 1, mqtt_rx_length);
				continue;
			}

			uint32_t remaining_length = 0;
			uint32_t multiplier = 1;
			uint8_t length_bytes = 0;
			bool length_complete = false;

			for (uint16_t index = 1; index < mqtt_rx_length && length_bytes < 4; index++) {
				uint8_t encoded = mqtt_rx_buffer[index];
				remaining_length += (encoded & 0x7F) * multiplier;
				length_bytes++;
				if ((encoded & 0x80) == 0) {
					length_complete = true;
					break;
				}
				multiplier *= 128;
			}

			if (!length_complete) {
				if (length_bytes == 4) { mqtt_rx_length = 0; }
				break;
			}

			uint32_t packet_length = 1 + length_bytes + remaining_length;
			if (packet_length > buffer_capacity) {
				mqtt_rx_length = 0;
				break;
			}
			if (mqtt_rx_length < packet_length) { break; }

			uint8_t packet_type = mqtt_rx_buffer[0] & 0xF0;
			uint16_t body_offset = 1 + length_bytes;
			if (debugEN == 1) {
				DEBUG.print(F("MQTT RX FRAME type=0x"));
				DEBUG.print(mqtt_rx_buffer[0], HEX);
				DEBUG.print(F(" bytes="));
				DEBUG.print(packet_length);
				DEBUG.print(F(" remaining="));
				DEBUG.println(remaining_length);
				DEBUG.print(F("MQTT RX RAW: "));
				for (uint32_t frame_index = 0; frame_index < packet_length; frame_index++) {
					if (mqtt_rx_buffer[frame_index] < 0x10) { DEBUG.print('0'); }
					DEBUG.print(mqtt_rx_buffer[frame_index], HEX);
					DEBUG.print(' ');
				}
				DEBUG.println();
			}

			if (packet_type == PUBLISH_ACK_PACKET_TYPE && remaining_length == 2) {
				response.ack_type = PUBACK;
				response.message_id = ((uint16_t)mqtt_rx_buffer[body_offset] << 8) |
					mqtt_rx_buffer[body_offset + 1];
				if (debugEN == 1) {
					DEBUG.print(F("PUBACK received ID="));
					DEBUG.print(response.message_id);
					DEBUG.print(F(" expected ID="));
					DEBUG.println(published_ID);
				}
			} else if (packet_type == CONNACK_PACKET_TYPE && remaining_length == 2) {
				response.ack_type = CONACK;
				response.return_code = (MQTT_RETURN_Code)mqtt_rx_buffer[body_offset + 1];
				if (debugEN == 1) { DEBUG.println(F("CONNACK_PACKET_TYPE SEEN")); }
			} else if (packet_type == SUBACK_PACKET_TYPE && remaining_length >= 3) {
				response.ack_type = SUBACK;
				response.message_id = ((uint16_t)mqtt_rx_buffer[body_offset] << 8) |
					mqtt_rx_buffer[body_offset + 1];
				if (debugEN == 1) { DEBUG.println(F("SUBACK_PACKET_TYPE SEEN")); }
			} else if (packet_type == PUBLISH_PACKET_TYPE && debugEN == 1) {
				DEBUG.println(F("PUBLISH_PACKET_TYPE SEEN"));
			}

			mqtt_rx_length -= packet_length;
			if (mqtt_rx_length > 0) {
				memmove(mqtt_rx_buffer, mqtt_rx_buffer + packet_length, mqtt_rx_length);
			}
		}
	}
}

void MQTTClass::queue_MQTT_update(uint32_t data, const char* data_name) { //this queue only stores names of 2 characters or less

/**
 * MQTT queue packet
 ***************************************************************************************************
 * |     7     |     6     |     5     |     4     |     3     |     2     |     1     |     0     |
 * |-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|
 * |  MQTT tag |  MQTT tag |    data   |	data   | 	data   |	data   | messageID | data type |
 */

	if (queue_isFull() && debugEN == 1) { DEBUG.println(F("MQTT queue full")); }

	if (send_queue.itemCount == 0) {
		send_queue.next_id = 1; //reset message identifier if all messages have been sent.
		send_queue.rear = 0;
		send_queue.front = 0;
		MQTT.message_id = 0;
	}
	
	if (send_queue.itemCount != 0) { send_queue.rear++; }
	if (send_queue.rear == QUEUE_SIZE) { send_queue.rear = 0; } //return rear of queue to start

	if ((send_queue.front == send_queue.rear) && send_queue.itemCount > 1) { //bump the start of the queue up one as oldest values will be overwritten by rollover
		send_queue.front++;
		if (send_queue.front == QUEUE_SIZE) { send_queue.front = 0; } //return rear of queue to start
	}

	uint32_t address = 0;
	if (send_queue.rear > 0) {

		address += 8 * (send_queue.rear);
	}

	memcpy(send_queue.buffer + address, &MQTT_uint32_t, 1); //flag uint32_t data type
	memcpy(send_queue.buffer + address + 1, &send_queue.next_id, 1); //add queue ID
	memcpy(send_queue.buffer + address + 2, &data, 4); //add data
	memcpy(send_queue.buffer + address + 6, data_name, 2); //add data_name

	send_queue.next_id++;

	if (send_queue.itemCount < QUEUE_SIZE) { send_queue.itemCount++; }

}

void MQTTClass::queue_MQTT_update(uint16_t data, const char* data_name) { //this queue only stores names of 2 characters or less

	if (queue_isFull() && debugEN == 1) { DEBUG.println(F("MQTT queue full")); }

	if (send_queue.itemCount == 0) {
		send_queue.next_id = 1; //reset message identifier if all messages have been sent.
		send_queue.rear = 0;
		send_queue.front = 0;
		MQTT.message_id = 0;
	}

	if (send_queue.itemCount != 0) { send_queue.rear++; }
	if (send_queue.rear == QUEUE_SIZE) { send_queue.rear = 0; } //return rear of queue to start

	if ((send_queue.front == send_queue.rear) && send_queue.itemCount > 1) { //bump the start of the queue up one as oldest values will be overwritten by rollover
		send_queue.front++;
		if (send_queue.front == QUEUE_SIZE) { send_queue.front = 0; } //return rear of queue to start
	}

	uint32_t address = 0;
	if (send_queue.rear > 0) {

		address += 8 * (send_queue.rear);
	}

	memcpy(send_queue.buffer + address, &MQTT_uint16_t, 1); //flag uint32_t data type
	memcpy(send_queue.buffer + address + 1, &send_queue.next_id, 1); //add queue ID
	memcpy(send_queue.buffer + address + 2, &data, 2); //add data
	memset(send_queue.buffer + address + 4, '\0', 1);
	memset(send_queue.buffer + address + 5, '\0', 1);
	memcpy(send_queue.buffer + address + 6, data_name, 2); //add data_name

	send_queue.next_id++;

	if (send_queue.itemCount < QUEUE_SIZE) { send_queue.itemCount++; }

}

void MQTTClass::queue_MQTT_update(uint8_t data, const char* data_name) { //this queue only stores names of 2 characters or less

	if (queue_isFull() && debugEN == 1) { DEBUG.println(F("MQTT queue full")); 
	DEBUG.print(F("MQTT Queue size: "));
	DEBUG.println(queue_size());

	DEBUG.print(F("MQTT state: "));
	DEBUG.println(HL7650.ModemCommandStep);

	DEBUG.print(F("ACK type: "));
	DEBUG.println(response.ack_type);

	DEBUG.print(F("ACK Message ID: "));
	DEBUG.println(MQTT.published_ID);

	DEBUG.print(F("Modem READY:"));
	DEBUG.println(HL7650.modemreadyfornextcommand);

	DEBUG.print(F("Modem response: "));
	DEBUG.println(HL7650.modemresponsereceived);
	}

	if (send_queue.itemCount == 0) {
		send_queue.next_id = 1; //reset message identifier if all messages have been sent.
		send_queue.rear = 0;
		send_queue.front = 0;
		MQTT.message_id = 0;
	}

	if (send_queue.itemCount != 0) { send_queue.rear++; }
	if (send_queue.rear == QUEUE_SIZE) { send_queue.rear = 0; } //return rear of queue to start

	if ((send_queue.front == send_queue.rear) && send_queue.itemCount > 1) { //bump the start of the queue up one as oldest values will be overwritten by rollover
		send_queue.front++;
		if (send_queue.front == QUEUE_SIZE) { send_queue.front = 0; } //return rear of queue to start
	}

	uint32_t address = 0;
	if (send_queue.rear > 0) {

		address += 8 * (send_queue.rear);
	}

	memcpy(send_queue.buffer + address, &MQTT_uint8_t, 1); //flag uint8_t data type
	memcpy(send_queue.buffer + address + 1, &send_queue.next_id, 1); //add queue ID
	memcpy(send_queue.buffer + address + 2, &data, 1); //add data
	memset(send_queue.buffer + address + 3, '\0', 1);
	memset(send_queue.buffer + address + 4, '\0', 1);
	memset(send_queue.buffer + address + 5, '\0', 1);
	memcpy(send_queue.buffer + address + 6, data_name, 2); //add data_name

	send_queue.next_id++;

	if (send_queue.itemCount < QUEUE_SIZE) { send_queue.itemCount++; }

}

void MQTTClass::queue_MQTT_update(int8_t data, const char* data_name) { //this queue only stores names of 2 characters or less

	if (queue_isFull() && debugEN == 1) { DEBUG.println(F("MQTT queue full")); }

	if (send_queue.itemCount == 0) {
		send_queue.next_id = 1; //reset message identifier if all messages have been sent.
		send_queue.rear = 0;
		send_queue.front = 0;
		MQTT.message_id = 0;
	}

	if (send_queue.itemCount != 0) { send_queue.rear++; }
	if (send_queue.rear == QUEUE_SIZE) { send_queue.rear = 0; } //return rear of queue to start

	if ((send_queue.front == send_queue.rear) && send_queue.itemCount > 1) { //bump the start of the queue up one as oldest values will be overwritten by rollover
		send_queue.front++;
		if (send_queue.front == QUEUE_SIZE) { send_queue.front = 0; } //return rear of queue to start
	}

	uint32_t address = 0;
	if (send_queue.rear > 0) {

		address += 8 * (send_queue.rear);
	}

	memcpy(send_queue.buffer + address, &MQTT_int8_t, 1); //flag int8_t data type
	memcpy(send_queue.buffer + address + 1, &send_queue.next_id, 1); //add queue ID
	memcpy(send_queue.buffer + address + 2, &data, 1); //add data
	memset(send_queue.buffer + address + 3, '\0', 1);
	memset(send_queue.buffer + address + 4, '\0', 1);
	memset(send_queue.buffer + address + 5, '\0', 1);
	memcpy(send_queue.buffer + address + 6, data_name, 2); //add data_name

	send_queue.next_id++;

	if (send_queue.itemCount < QUEUE_SIZE) { send_queue.itemCount++; }

}

void MQTTClass::queue_MQTT_update(float data, const char* data_name) { //this queue only stores names of 2 characters or less

	if (queue_isFull() && debugEN == 1) { DEBUG.println(F("queue full")); }

	if (send_queue.itemCount == 0) {
		send_queue.next_id = 1; //reset message identifier if all messages have been sent.
		send_queue.rear = 0;
		send_queue.front = 0;
	}

	if (send_queue.itemCount != 0) { send_queue.rear++; }
	if (send_queue.rear == QUEUE_SIZE) { send_queue.rear = 0; } //return rear of queue to start

	if ((send_queue.front == send_queue.rear) && send_queue.itemCount > 1) { //bump the start of the queue up one as oldest values will be overwritten by rollover
		send_queue.front++;
		if (send_queue.front == (QUEUE_SIZE-1)) { send_queue.front = 0; } //return rear of queue to start
	}

	uint32_t address = 0;
	if (send_queue.rear > 0) {

		address += 8 * (send_queue.rear);
	}

	memcpy(send_queue.buffer + address, &MQTT_float, 1); //flag float data type
	memcpy(send_queue.buffer + address + 1, &send_queue.next_id, 1); //add queue ID
	memcpy(send_queue.buffer + address + 2, &data, 4); //add data
	memcpy(send_queue.buffer + address + 6, data_name, 2); //add data_name

	send_queue.next_id++;

	if (send_queue.itemCount < QUEUE_SIZE) { send_queue.itemCount++; }

}

void MQTTClass::queue_MQTT_update(char* data, const char* data_name) { //this queue only stores names of 2 characters or less

	if (queue_isFull() && debugEN == 1) { DEBUG.println(F("queue full")); }

	if (send_queue.itemCount == 0) {
		send_queue.next_id = 1; //reset message identifier if all messages have been sent.
		send_queue.rear = 0;
		send_queue.front = 0;
	}

	if (send_queue.itemCount != 0) { send_queue.rear ++; }
	if (send_queue.rear == (QUEUE_SIZE-1)) { send_queue.rear = 0; } //return rear of queue to start

	if ((send_queue.front == send_queue.rear) && send_queue.itemCount > 1) { //bump the start of the queue up one as oldest values will be overwritten by rollover
		send_queue.front ++;
		if (send_queue.front == (QUEUE_SIZE-1)) { send_queue.front = 0; } //return rear of queue to start
	}

	uint32_t address = 0;
	if (send_queue.rear > 0) { address += 8 * (send_queue.rear); }

	memcpy(send_queue.buffer + address, &MQTT_char, 1); //flag char data type
	memcpy(send_queue.buffer + address + 1, &send_queue.next_id, 1); //add queue ID
	memcpy(send_queue.buffer + address + 2, data, 12); //add data
	memcpy(send_queue.buffer + address + 14, data_name, 2); //add data_name

	send_queue.next_id++;
	send_queue.rear++; //add extra as char storage takes up two spaces (16 bytes)

	if (send_queue.itemCount < QUEUE_SIZE) { send_queue.itemCount++; }

}

void MQTTClass::dequeue_MQTT_update() {

	if (send_queue.itemCount > 0) {
		uint32_t address = 0;
		memset(dequeued_data, '\0', 16);

		if (send_queue.front > 0) {
			address += 8 * (send_queue.front);
		}

		if (send_queue.buffer[address] == MQTT_char) { memcpy(dequeued_data, send_queue.buffer + address, 16); }

		else { memcpy(dequeued_data, send_queue.buffer + address, 8); }

		if (send_queue.itemCount > 0) { 
			send_queue.itemCount--;

			if (send_queue.buffer[address] == MQTT_char) { send_queue.front += 2; }
				
			else { send_queue.front++; }
		}

		if (send_queue.front >= QUEUE_SIZE) { send_queue.front = 0; }

	}

	else {
		if (debugEN == 1) {
			DEBUG.println(F("ERROR! No data in queue\r\n"));
		}
	}

}

void MQTTClass::queue_peek() {

	if (send_queue.itemCount > 0) {
		uint32_t address = 0;

		if (send_queue.front > 0) {
			address += 8 * (send_queue.front);
		}

		if (send_queue.buffer[address] == MQTT_char) {
			memcpy(dequeued_data, send_queue.buffer + address, 16);
		}

		else { memcpy(dequeued_data, send_queue.buffer + address, 8); }
	}

	else {
		if (debugEN == 1) {
			DEBUG.println(F("ERROR! No data in queue\r\n"));
		}
	}

}

uint8_t MQTTClass::queue_isEmpty() {

	return send_queue.itemCount == 0;
}


uint8_t MQTTClass::queue_isFull() {

	return send_queue.itemCount == QUEUE_SIZE;
}

uint16_t MQTTClass::queue_size() {

	return send_queue.itemCount;
}
//Heartbeat reading current dpt 145 values regardless if they have changed or not. This is to ensure that the MQTT server has the most up to date values at all times.

/*
void MQTTClass::queue_MQTT_TIMED_update(void) {
    queue_MQTT_update(sensor1.T,    "ta");
    queue_MQTT_update(sensor1.Tdf,  "td");
    queue_MQTT_update(sensor1.Tdfa, "tf");
    queue_MQTT_update(sensor1.H2O,  "h1");
    queue_MQTT_update(sensor1.P,    "p1");
    queue_MQTT_update(sensor1.Rhoo, "r1");
    queue_MQTT_update(sensor2.T,    "tb");
    queue_MQTT_update(sensor2.P,    "p2");
    queue_MQTT_update(sensor3.T,    "tc");
    queue_MQTT_update(sensor3.P,    "p3");
}
*/
void MQTTClass::queue_MQTT_TIMED_update(void){
	//SENSOR 1
	queue_MQTT_update(sensor1.T,  "ta");
	queue_MQTT_update(sensor1.Tdf,  "td");
	queue_MQTT_update(sensor1.Tdfa,  "tf");
	queue_MQTT_update(sensor1.H2O,  "h1");
	queue_MQTT_update(sensor1.P,  "p1");
	queue_MQTT_update(sensor1.Rhoo,  "r1");
	queue_MQTT_update(sensor1.Pnorm,  "pn");
	//SENSOR 2 
	queue_MQTT_update(sensor2.T, "tb"); 
	queue_MQTT_update(sensor2.Tdf, "td"); 
	queue_MQTT_update(sensor2.Tdfa, "tf");
	queue_MQTT_update(sensor2.H2O,  "h2");
	queue_MQTT_update(sensor2.P,  "p2");
	queue_MQTT_update(sensor2.Rhoo,  "r2");
	queue_MQTT_update(sensor2.Pnorm,  "pn");
	//SENSOR 3
	queue_MQTT_update(sensor3.T, "tc");
	queue_MQTT_update(sensor3.Tdf, "td");
	queue_MQTT_update(sensor3.Tdfa, "tf");
	queue_MQTT_update(sensor3.H2O,  "h3");
	queue_MQTT_update(sensor3.P,  "p3");
	queue_MQTT_update(sensor3.Rhoo,  "r3");
	queue_MQTT_update(sensor3.Pnorm,  "pn");
}