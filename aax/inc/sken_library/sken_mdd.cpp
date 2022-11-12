/*
 * sken_mdd.cpp
 *
 *  Created on: 2021/02/01
 *      Author: TakumaNakao
 */

#include "sken_mdd.h"

void SkenMdd::init(Pin tx_pin,Pin rx_pin,UartNumber uart_num)
{
	serial.init(tx_pin,rx_pin,uart_num,115200);
	serial.startDmaRead(&receive_data,1);
}

bool SkenMdd::tcp(uint8_t id,const float (&command_data)[4],unsigned int resend_time,unsigned int max_wait_time)
{
	unsigned int send_time = 0;
	unsigned int start_time = sken_system.millis();
	do {
		if ((sken_system.millis() - start_time) > max_wait_time) { //最大待ち時間を超えたら失敗を返す
			return false;
		}
		if ((sken_system.millis() - send_time) > resend_time) { //再送信時間を超えたら再送信
			sendData(id,command_data);
			send_time = sken_system.millis();
		}
	} while (receive_data != seq); //送信したseqと受信したseqが一致するまでループ
	return true;
}

void SkenMdd::udp(uint8_t id,const float (&command_data)[4])
{
	sendData(id,command_data);
}

void SkenMdd::sendData(uint8_t id,const float (&command_data)[4])
{
	uint8_t send_data[21] = {};
	send_data[0] = 0xA5;
	send_data[1] = 0xA5;
	send_data[2] = ++seq;
	send_data[3] = id;
	setFloatData(command_data,send_data);
	send_data[20] = calcChecksum(send_data);
	serial.write(send_data,21);
}

void SkenMdd::setFloatData(const float (&command_data)[4],uint8_t (&send_data)[21])
{
	ConvertIntFloat cif;
	for (int i = 0; i < 4; i++) {
		cif.float_val = command_data[i];
		for (int j = 0; j < 4; j++) {
			send_data[4 + (i * 4) + j] = cif.uint8_val[j];
		}
	}
}

uint8_t SkenMdd::calcChecksum(const uint8_t (&send_data)[21])
{
	uint8_t checksum = 0;
	for (int i = 2; i < 20; i++) {
		checksum += send_data[i];
	}
	return checksum;
}
