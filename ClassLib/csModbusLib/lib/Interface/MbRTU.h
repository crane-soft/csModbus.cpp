#pragma once
#include <stdint.h>
#include "Interface/MbSerial.h"

namespace csModbusLib {

    /// <summary>
    /// Static class for CRC16 compute
    /// </summary>
	class CRC16 {
		// TODO CRC into Utils verschieben
	private:
		const uint16_t POLY = 0xA001;
		uint16_t crc_tab16[256];

		void InitCRC16Tab();
		uint16_t UpdateCRC16(uint16_t crc, uint8_t bt);
	public:
		uint16_t CalcCRC16(uint8_t *buffer, int offset, int length);
		CRC16();
	};


    class MbRTU : public MbSerial {
 
	public: 
		MbRTU(SerialPort * sp);
		void SendFrame(int Length);
		int EndOffFrameLenthth();

	protected:
		bool StartOfFrameDetected();
		bool Check_EndOfFrame();

	private: 
		CRC16 crc16;
		void Init();
	};
}
