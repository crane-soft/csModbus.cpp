﻿#include "MbRTU.h"
#include "Modbus/MbFrame.h"

#include <math.h>

namespace csModbusLib {

	void CRC16::InitCRC16Tab()
	{
		uint16_t crc, c;

		for (int ii = 0; ii < 256; ii++) {
			crc = 0;
			c = (uint16_t)ii;
			for (int jj = 0; jj < 8; jj++) {

				if (((crc ^ c) & 0x0001) == 0x0001)
					crc = (uint16_t)((crc >> 1) ^ POLY);
				else
					crc = (uint16_t)(crc >> 1);

				c = (uint16_t)(c >> 1);
			}

			crc_tab16[ii] = crc;
		}
	}

	uint16_t CRC16::UpdateCRC16(uint16_t crc, uint8_t bt)
	{
		uint8_t tableIndex = (uint8_t)(crc ^ bt);
		crc >>= 8;
		crc ^= crc_tab16[tableIndex];
		return crc;
	}

	uint16_t CRC16::CalcCRC16(uint8_t *buffer, int offset, int length)
	{
		uint16_t crc = 0xFFFF;
		for (int ii = 0; ii < length; ii++)
			crc = UpdateCRC16(crc, buffer[offset + ii]);
		// I have to exange high low uint8_t 
		return (uint16_t)((crc >> 8) | ((crc & 0xff) << 8));
	}

	CRC16::CRC16()
	{
		InitCRC16Tab();
	}


	MbRTU::MbRTU(SerialPort *sp) : MbSerial(sp)
	{
		Init();
	}
	
	void MbRTU::Init()
	{
	}

	bool MbRTU::StartOfFrameDetected() 
	{
		return (sp->BytesToRead() >= 2);
	}

	int MbRTU::EndOffFrameLenthth()
	{
		return 2;
	}

	bool MbRTU::Check_EndOfFrame()
	{
		int crc_idx = MbData->EndIdx;
		ReceiveBytes(2);

		// Check CRC
		uint16_t msg_crc = MbData->GetUInt16(crc_idx);
		uint16_t calc_crc = crc16.CalcCRC16(MbData->Data, MbRawData::ADU_OFFS, crc_idx - MbRawData::ADU_OFFS);

		return (msg_crc == calc_crc);
	}

	void MbRTU::SendFrame(int Length)
	{
		uint16_t calc_crc = crc16.CalcCRC16(MbData->Data, MbRawData::ADU_OFFS, Length);
		MbData->PutUInt16(MbRawData::ADU_OFFS + Length, calc_crc);
		Length += 2;
		SendData(MbData->Data, MbRawData::ADU_OFFS, Length);
	}
}
