#pragma once
#include "MbSlaveDataServer.h"

namespace csModbusLib
{
    class MbSlaveEmbServer : public MbSlaveDataServer
    {
    public:
        MbSlaveEmbServer(int SlaveID) :  MbSlaveDataServer(SlaveID)
        {
        }

    protected:
        bool ReadCoils() override
        { 
            coil_t* Bits = ReadCoils(Frame->DataAddress, Frame->DataCount);
            if (Bits != 0) {
            	Frame->PutBitValues(Bits);
            	return true;
                        }
            return false;
        }

        bool WriteSingleCoil() override
        {
            return WriteSingleCoil(Frame->DataAddress, Frame->GetSingleBit());
        }

        bool WriteSingleRegister() override
        {
            return WriteSingleRegister(Frame->DataAddress, Frame->GetSingleUInt16());
        }

        virtual coil_t* ReadCoils(uint16_t Address, int Size) { return 0; }

        virtual bool WriteSingleCoil(uint16_t Address, coil_t Bit) { return false; }
        virtual bool WriteSingleRegister(uint16_t Address, uint16_t Value) { return false; }
    };
}

