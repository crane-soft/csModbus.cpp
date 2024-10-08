﻿#pragma once
#include <stdint.h>
#include "Interface/MbSerial.h"

namespace csModbusLib {

	class MbASCII : public MbSerial   {

	public:
		MbASCII(SerialPort * sp) : MbSerial(sp) {}
		void SendFrame(int Length);
		int NumOfSerialBytes(int count);
		int EndOffFrameLenthth();
	protected:
		bool StartOfFrameDetected();

		bool Check_EndOfFrame();
		void ReceiveBytes(uint8_t *RxData, int offset, int count);

	private:
		const char ASCII_START_FRAME = ':';
		int ASCII2Hex(uint8_t * hexchars);
		int ASCII2nibble(int hexchar);

		static uint8_t CalcLRC(uint8_t * buffer, int offset, int length);
		static uint8_t ByteToHexChar(int b);
	};
}
