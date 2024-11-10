#pragma once
#include <stdint.h>
#include "Interface/MbSerial.h"

namespace csModbusLib {

    class MbRTU : public MbSerial {
 
	public: 
		MbRTU() {}
		MbRTU(SerialPort * sp);
		void ReceiveHeader(int timeOut) override;
		void SendFrame(int Length) override;
		ConnectionType getConnectionType() const override { return ConnectionType::SERIAL_RTU; };

		void EndOfFrame() override;
		void Check_EndOfFrame() override;


	private: 
		uint16_t UpdateCRC16(uint16_t crc, uint8_t bt) const;
		uint16_t CalcCRC16(uint8_t* buffer, int length) const;

		void Init();
	};
}
