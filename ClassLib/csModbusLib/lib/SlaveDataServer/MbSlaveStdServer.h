#pragma once
#include "MbSlaveData.h"
#include "MbSlaveDataServer.h"

namespace csModbusLib {
    
    class StdDataServer : public MbSlaveDataServer {

	private:
        ModbusDataT<coil_t> *gDiscreteInputs;
        ModbusDataT<coil_t> *gCoils;
        ModbusDataT<uint16_t> *gInputRegisters;
        ModbusDataT<uint16_t> *gHoldingRegisters;

	public:
		StdDataServer() : MbSlaveDataServer(0) {
			Init();
		}

        StdDataServer(int SlaveID) : MbSlaveDataServer(SlaveID)
        {
			Init();
        }

	private:
		void Init()
		{
			gDiscreteInputs = 0;
			gCoils = 0;
			gInputRegisters = 0;
			gHoldingRegisters = 0;

		}

	public:
		void AddCoils(ModbusDataT<coil_t> *BoolData) {
			AddModbusData<coil_t>(&gCoils, BoolData);
        }

        void AddCoils(int BaseAddr, coil_t *Coils, int Length) {
			AddModbusData<coil_t>(&gCoils, BaseAddr, Coils, Length);
        }

        void AddDiscreteInputs(ModbusDataT<coil_t> *BoolData){   
			AddModbusData<coil_t>(&gDiscreteInputs, BoolData);
        }

        void AddDiscreteInputs(int BaseAddr, coil_t * Coils, int Length) {
			AddModbusData<coil_t>(&gDiscreteInputs,BaseAddr, Coils, Length);
        }

        void AddInputRegisters(ModbusDataT<uint16_t> *RegsData) {
			AddModbusData<uint16_t>(&gInputRegisters,RegsData);
        }

        void AddInputRegisters(int BaseAddr, uint16_t *Registers, int Length) {
			AddModbusData<uint16_t>(&gInputRegisters,BaseAddr, Registers, Length);
        }

        void AddHoldingRegisters(ModbusDataT<uint16_t> *RegsData){
			AddModbusData<uint16_t>(&gHoldingRegisters,RegsData);
        }

        void AddHoldingRegisters(int BaseAddr, uint16_t *Registers, int Length){
			AddModbusData<uint16_t>(&gHoldingRegisters,BaseAddr, Registers, Length);
        }

	protected:
		bool ReadCoils() {
			return RwModbusData(gCoils, &ModbusData::ReadMultiple);
        }

        bool WriteSingleCoil()  {
			return RwModbusData(gCoils, &ModbusData::WriteSingle, true);
        }

        bool WriteMultipleCoills()  {
			return RwModbusData(gCoils, &ModbusData::WriteMultiple, true);
        }

        bool ReadDiscreteInputs() {
			return RwModbusData(gDiscreteInputs, &ModbusData::ReadMultiple);
        }

        bool ReadInputRegisters()  {
			return RwModbusData(gInputRegisters, &ModbusData::ReadMultiple);
         }

        bool ReadHoldingRegisters() {
			return RwModbusData(gHoldingRegisters, &ModbusData::ReadMultiple);
        }

        bool WriteSingleRegister()  {
			return RwModbusData(gHoldingRegisters, &ModbusData::WriteSingle, true);
        }

        bool WriteMultipleRegisters() {
			return RwModbusData(gHoldingRegisters, &ModbusData::WriteMultiple, true);
        }
	private:
		template <typename DataT>
		bool RwModbusData(ModbusDataT<DataT> *Data, void (ModbusData::*func)(MBSFrame *Frame), bool isWrite = 0)
		{
			if (Data == 0)
				return false;
			return Data->ScannAll(func, Frame, isWrite);
		}

		template<typename DataT> void AddModbusData(ModbusDataT<DataT> **BaseData, ModbusDataT<DataT> *AddData)
		{
			if (*BaseData == 0) {
				*BaseData = AddData;
			} else {
				(*BaseData)->AddModbusData(AddData);
			}
		}

		template<typename DataT> void AddModbusData(ModbusDataT<DataT> **BaseData, int BaseAddr, DataT *ModbusData, int Length)
		{
			if (*BaseData == 0) {
				*BaseData = new ModbusDataT<DataT>();
			}
			(*BaseData)->AddData(BaseAddr, ModbusData, Length);
		}
	};
}

