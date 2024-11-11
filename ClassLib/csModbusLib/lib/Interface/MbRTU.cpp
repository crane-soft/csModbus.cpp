#include "MbRTU.h"
#include "Modbus/MbFrame.h"

#include <math.h>
#include <array>
#include "platform.h"

namespace csModbusLib {

	// https://godbolt.org/z/zEczaE9WG

	const uint16_t POLY = 0xA001;
	const int N = 256;

	constexpr uint16_t crc_value(int idx)
	{
		uint16_t  crc = 0;
		uint16_t c = (uint16_t)idx;
		for (int jj = 0; jj < 8; jj++) {

			if (((crc ^ c) & 0x0001) == 0x0001)
				crc = (uint16_t)((crc >> 1) ^ POLY);
			else
				crc = (uint16_t)(crc >> 1);

			c = (uint16_t)(c >> 1);
		}
		return crc;
	}

	constexpr std::array<uint16_t, N> get_crc()
	{
		std::array<uint16_t, N> table{ 0 };
		for (int i = 0; i < N; ++i)
			table[i] = crc_value(i);
		return table;
	}
	static constexpr auto crc_tab16 = get_crc();


	MbRTU::MbRTU(SerialPort *sp) : MbSerial(sp)
	{
		Init();
	}
	
	void MbRTU::Init()
	{
	}

	void MbRTU::ReceiveHeader(int timeOut)
	{
		MbData->Clear();
		ReceiveData(2, timeOut); // Node-ID + Function-Byte
	}

	bool MbRTU::EndOfFrame()
	{
		ReceiveBytes(2);
		return Check_EndOfFrame();
	}

	bool  MbRTU::Check_EndOfFrame()
	{
		int crc_idx = MbData->EndIdx-2;

		// Check CRC
		uint16_t msg_crc = MbData->GetUInt16(crc_idx);
		uint16_t calc_crc = CalcCRC16(&MbData->Data[MbRawData::ADU_OFFS], crc_idx - MbRawData::ADU_OFFS);
		return (msg_crc == calc_crc);
		// If the server receives the request, but detects a communication error (parity, LRC, CRC,  ...),
		// no response is returned. The client program will eventually process a timeout condition for the request.
	}

	void MbRTU::SendFrame(int Length)
	{
		// DiscardBuffer to resync start of frame
		sp->DiscardInOut();

		uint16_t calc_crc = CalcCRC16(&MbData->Data[MbRawData::ADU_OFFS], Length);
		MbData->PutUInt16(MbRawData::ADU_OFFS + Length, calc_crc);
		Length += 2;
		SendData(&MbData->Data[MbRawData::ADU_OFFS], Length);
	}

	uint16_t MbRTU::UpdateCRC16(uint16_t crc, uint8_t bt) const
	{
		uint8_t tableIndex = (uint8_t)(crc ^ bt);
		crc >>= 8;
		crc ^= crc_tab16[tableIndex];
		return crc;
	}

	uint16_t MbRTU::CalcCRC16(uint8_t* buffer, int length) const
	{
		uint16_t crc = 0xFFFF;
		for (int ii = 0; ii < length; ii++)
			crc = UpdateCRC16(crc, buffer[ii]);
		// I have to exange high low uint8_t 
		return (uint16_t)((crc >> 8) | ((crc & 0xff) << 8));
	}

}
