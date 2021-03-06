#pragma once
#include <stdint.h>
#include "MbSerial.h"
#include "MbFrame.h"

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
		MbRTU();
		MbRTU(const char * port, int baudrate);
		void SendFrame(MbRawData *TransmitData, int Length);
	protected:
		bool StartOfFrameDetected();
		bool Check_EndOfFrame(MbRawData *RxData);

	private: 
		CRC16 crc16;
		void Init();
	};
}
