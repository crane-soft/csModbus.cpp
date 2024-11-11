#pragma once
#include <stdint.h>
#include "Interface/MbSerial.h"

namespace csModbusLib {

	class MbASCII : public MbSerial   {

	public:
		MbASCII() {}
		MbASCII(SerialPort * sp) : MbSerial(sp) {}
		ConnectionType getConnectionType() const override { return ConnectionType::SERIAL_ASCII; };

		void ReceiveHeader(int timeOut) override;
		void SendFrame(int Length) override;
		bool EndOfFrame() override;
		bool Check_EndOfFrame() override;

		void ASCII2Hex();
		static const char ASCII_START_FRAME = ':';

	protected:
		void ReceiveData(int count, int timeOut = ByteCountTimeout) override;

	private:
		void WaitFrameStart(int timeout);
		static int ASCII2Hex(uint8_t * hexchars);
		static int ASCII2nibble(int hexchar);

		static uint8_t CalcLRC(uint8_t * buffer, int length);
		static uint8_t ByteToHexChar(int b);
	};
}
