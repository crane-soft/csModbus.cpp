#include "Interface/MbASCII.h"
#include "Modbus/MbFrame.h"
#include "platform.h"

namespace csModbusLib
{
	void  MbASCII::WaitFrameStart(int timeout) 
	{
		sp->SetReadTimeout(10);
		int ch;
		while ((ch = sp->ReadByte()) != ASCII_START_FRAME) {
			if (timeout != MbInterface::InfiniteTimeout) {
				if (ch == 0) {
					timeout -= 10;
					if (timeout <= 0) {
						THROW(ErrorCodes::RX_TIMEOUT);
					}
				}
			}
		}

	}


	void MbASCII::ReceiveHeader(int timeOut)
	{
		WaitFrameStart(timeOut);
		FrameData.Clear();
		MbSerial::ReceiveBytes(2); // Node-ID + Function-Byte
	}

	bool MbASCII::EndOfFrame()
	{
		MbSerial::ReceiveBytes(1);   // Read LRC
		MbSerial::ReceiveData(2);	// Read CR-LF
		return Check_EndOfFrame();
	}

	void MbASCII::ReceiveData(int count, int timeout)
	{
		for (int i = 0; i < count; ++i) {
			MbSerial::ReceiveData(2, timeout);
			ASCII2Hex();
		}
	}

	void MbASCII::ASCII2Hex()
	{
		uint8_t* buffPtr = FrameData.BufferEnd() -2;
		*buffPtr = ASCII2Hex(buffPtr);
		FrameData.EndIdx -= 1;
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


	bool MbASCII::Check_EndOfFrame()
	{
		// Check LRC
		FrameData.EndIdx -= 2;	// discard crlf point aftr LCR
		uint8_t calc_lrv = CalcLRC(FrameData.DataStart(), FrameData.Length());
		return (calc_lrv == 0);
	}

	uint8_t MbASCII::CalcLRC(uint8_t * buffer, int length)
	{
		int lrc_result = 0;
		for (int i = 0; i < length; ++i) {
			lrc_result += buffer[i];
			lrc_result &= 0xff;
		}
		lrc_result = (-lrc_result) & 0xff;
		return (uint8_t)lrc_result;
	}

	void MbASCII::SendFrame(int Length)
	{
		// DiscardBuffer to resync start of frame
		sp->DiscardInOut();

		uint8_t lrc_value = CalcLRC(FrameData.DataStart(), Length);
		FrameData.Data[MbRawData::ADU_OFFS + Length] = lrc_value;
		Length += 1;

		uint8_t hexbuff[2];
		hexbuff[0] = (uint8_t)':';
		SendData(hexbuff, 1);

		for (int i = 0; i < Length; ++i) {
			hexbuff[0] = ByteToHexChar(FrameData.Data[MbRawData::ADU_OFFS + i] >> 4);
			hexbuff[1] = ByteToHexChar(FrameData.Data[MbRawData::ADU_OFFS + i]);
			SendData(hexbuff, 2);
		}

		hexbuff[0] = 0x0d;
		hexbuff[1] = 0x0a;
		SendData(hexbuff, 2);
	}

	uint8_t MbASCII::ByteToHexChar(int b)
	{
		b &= 0x0f;
		int hexChar = b < 10 ? (b + 48) : (b + 55);
		return (uint8_t)hexChar;
	}
}
