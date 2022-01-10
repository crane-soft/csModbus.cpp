#pragma once
#include <stdint.h>
#include "MbSerial.h"
#include "MbFrame.h"

namespace csModbusLib {

	class MbASCII : public MbSerial   {

	public:
		MbASCII() {};
		MbASCII(const char * port, int baudrate)
			: MbSerial(port, baudrate, 8, SerialPort::Parity::NoParity, SerialPort::StopBits::One) {}

		MbASCII(const char *port, int baudrate, int databits, SerialPort::Parity parity, SerialPort::StopBits stopbits)
			: MbSerial(port, baudrate, databits, parity, stopbits) {}

		void SendFrame(MbRawData *TransmitData, int Length);

	protected:
		bool StartOfFrameFound();
		bool Check_EndOfFrame(MbRawData *RxData);
		void ReceiveBytes(uint8_t *RxData, int offset, int count);

	private:
		const char ASCII_START_FRAME = ':';
		int ASCII2Hex(uint8_t * hexchars);
		int ASCII2nibble(int hexchar);


		static uint8_t CalcLRC(uint8_t * buffer, int offset, int length);
		static uint8_t ByteToHexChar(int b);
	};
}
