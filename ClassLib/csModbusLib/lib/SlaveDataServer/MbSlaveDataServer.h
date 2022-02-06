#pragma once
#include "MbFrame.h"

namespace csModbusLib {

    class MbSlaveDataServer {
        public:
			MbSlaveDataServer *NextDataServer;   
	private:
		int gSlaveID;
	protected: 
		MBSFrame *Frame;

	public: 
		MbSlaveDataServer()  {
            gSlaveID = 0;
            NextDataServer = 0;
        }

        MbSlaveDataServer(int SlaveID)  {
            gSlaveID = SlaveID;
			NextDataServer = 0;
		}

        void Add (MbSlaveDataServer *NewServer)  {
            NextDataServer = NewServer;
        }

		int Get_SlaveID()  {
           return gSlaveID; 
		}

		void Set_SlaveID (int value) { 
			gSlaveID = value; 
		}

        void DataServices(MBSFrame *aFrame)
        {
            Frame = aFrame;
            if (Frame->SlaveId == gSlaveID) {
                switch (Frame->FunctionCode) {
				case ModbusCodes::READ_COILS:
                        if (ReadCoils()) return;
                        break;
                    case ModbusCodes::READ_DISCRETE_INPUTS:
                        if (ReadDiscreteInputs()) return;
                        break;
                    case ModbusCodes::READ_HOLDING_REGISTERS:
                        if (ReadHoldingRegisters()) return;
                        break;
                    case ModbusCodes::READ_INPUT_REGISTERS:
                        if (ReadInputRegisters()) return;
                        break;
                    case ModbusCodes::WRITE_SINGLE_COIL:
                        if (WriteSingleCoil()) return;
                        break;
                    case ModbusCodes::WRITE_SINGLE_REGISTER:
                        if (WriteSingleRegister()) return;
                        break;
                    case ModbusCodes::WRITE_MULTIPLE_COILS:
                        if (WriteMultipleCoills()) return;
                        break;
                    case ModbusCodes::WRITE_MULTIPLE_REGISTERS:
                        if (WriteMultipleRegisters()) return;
                        break;
                    case ModbusCodes::READ_WRITE_MULTIPLE_REGISTERS:
						if (ReadHoldingRegisters()) {
							Frame->GetRwWriteAddress();
							if (WriteMultipleRegisters()) {
                                return;
                            }
                        }
                        break;
					default:
						Frame->ExceptionCode = ExceptionCodes::ILLEGAL_FUNCTION;
						return;
                }
				Frame->ExceptionCode = ExceptionCodes::ILLEGAL_DATA_ADDRESS;
            }
        }

	protected:
		virtual bool ReadCoils() { return false; }
        virtual bool ReadDiscreteInputs() { return false; }
        virtual bool ReadHoldingRegisters() { return false; }
        virtual bool ReadInputRegisters() { return false; }
        virtual bool WriteSingleCoil() { return false; }
        virtual bool WriteSingleRegister() { return false; }
        virtual bool WriteMultipleCoills() { return false; }
        virtual bool WriteMultipleRegisters() { return false; }
	};
}
