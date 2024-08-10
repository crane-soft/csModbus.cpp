#include "MbBase.h"
#include "MbASCII.h"

namespace csModbusLib
{
	bool MbASCII::StartOfFrameDetected()
    {
		if (sp.BytesToRead() > 0) {
			if (sp.ReadByte() == ':') {
				return true;
			}
		}
        return false;
	}

	int MbASCII::NumOfSerialBytes(int count)
	{
		return 2 * count;
	}

	void MbASCII::ReceiveBytes(uint8_t* RxData, int offset, int count)
	{
		uint8_t hexchars[2];
		for (int i = 0; i < count; ++i) {
			MbSerial::ReceiveBytes(hexchars, 0, 2);
			RxData[offset + i] = (uint8_t)ASCII2Hex(hexchars);
		}
	}

	int MbASCII::ASCII2Hex(uint8_t * hexchars)
	{
		return ((ASCII2nibble(hexchars[0]) << 4) | ASCII2nibble(hexchars[1])) & 0xff;
	}

	int MbASCII::ASCII2nibble(int hexchar)
	{
		if ((hexchar >= '0') && (hexchar <= '9')) {
			return hexchar - 0x30;
		} else {
			hexchar = hexchar & ~0x20; // toupper
			if ((hexchar >= 'A') && (hexchar <= 'F')) {
				return hexchar - ('A' - 10);
			} else {
				return 0;
			}
		}
	}

	int MbASCII::EndOffFrameLenthth()
	{
		return 4;   // 2 chars LRC  + CRLF
	}

	bool MbASCII::Check_EndOfFrame()
	{
		MbSerial::ReceiveBytes(1);   // Read LRC

		uint8_t crlf[2];// Read CR/LF
		MbSerial::ReceiveBytes(crlf, 0, 2);

		// Check LRC
		uint8_t calc_lrv = CalcLRC(MbData->Data, MbRawData::ADU_OFFS, MbData->EndIdx - MbRawData::ADU_OFFS);
		return (calc_lrv == 0);
	}

	uint8_t MbASCII::CalcLRC(uint8_t * buffer, int offset, int length)
	{
		int lrc_result = 0;
		for (int i = 0; i < length; ++i) {
			lrc_result += buffer[offset + i];
			lrc_result &= 0xff;
		}
		lrc_result = (-lrc_result) & 0xff;
		return (uint8_t)lrc_result;
	}

	void MbASCII::SendFrame(int Length)
	{
		uint8_t lrc_value = CalcLRC(MbData->Data, MbRawData::ADU_OFFS, Length);

		MbData->Data[MbRawData::ADU_OFFS + Length] = lrc_value;
		Length += 1;

		int hexbuff_Length = Length * 2 + 3;
		uint8_t hexbuff[MbBase::MAX_FRAME_LEN * 2 + 3];
		hexbuff[0] = (uint8_t)':';

		for (int i = 0; i < Length; ++i) {
			hexbuff[1 + 2 * i] = ByteToHexChar(MbData->Data[MbRawData::ADU_OFFS + i] >> 4);
			hexbuff[2 + 2 * i] = ByteToHexChar(MbData->Data[MbRawData::ADU_OFFS + i]);
		}
		hexbuff[hexbuff_Length - 2] = 0x0d;
		hexbuff[hexbuff_Length - 1] = 0x0a;

		SendData(hexbuff, 0, hexbuff_Length);
	}

	uint8_t MbASCII::ByteToHexChar(int b)
	{
		b &= 0x0f;
		int hexChar = b < 10 ? (b + 48) : (b + 55);
		return (uint8_t)hexChar;
	}
}
