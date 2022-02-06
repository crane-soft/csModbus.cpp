#pragma once
#include "MbFrame.h"
namespace csModbusLib {

	class ModbusDataEvents {
	public:
		virtual void ValueChangedEvent(int Addr, int Size) {}
		virtual void ValueReadEvent(int Addr, int Size) {}
	};

	class ModbusData
	{
	protected:
		ModbusData *NextData;
		int MyBaseAddr;
		int MySize;
	private:
		ModbusDataEvents *gEvents;

	public:
		ModbusData() {
			Init(0, 0);
		}

		ModbusData(int aAddress, int aSize) 	{
			Init(aAddress, aSize);
		}

		int GetBaseAddr()	{
			return MyBaseAddr;
		}

		void AddModbusData(ModbusData *newData)	{
			if (NextData == 0) {
				NextData = newData;
			} else {
				NextData->AddModbusData(newData);
			}
		}

		bool ScannAll(void (ModbusData::*func)(MBSFrame *Frame), MBSFrame *Frame, bool isWrite)
		{
			if (Frame->MatchAddress(MyBaseAddr, MySize)) {
				(this->*func) (Frame);
				if (isWrite)
					RaiseValueChangedEvent(Frame->DataAddress, Frame->DataCount);
				else
					RaiseValueReadEvent(Frame->DataAddress, Frame->DataCount);
				return true;
			} else {
				if (NextData != 0) {
					return NextData->ScannAll(func, Frame, isWrite);
				}
				return false;
			}
		}

		void RegisterEvents(ModbusDataEvents *Events) {
			gEvents = Events;
		}

	protected:
		void Init(int aAddress, int aSize)	{
			MyBaseAddr = aAddress;
			MySize = aSize;
			NextData = 0;
			gEvents = 0;
		}

	public:
		virtual void ReadMultiple(MBSFrame *Frame) {};
		virtual void WriteMultiple(MBSFrame *Frame) {}
		virtual void WriteSingle(MBSFrame *Frame) {}


	private:
		void RaiseValueChangedEvent(int Addr, int Size) {
			if (gEvents)
				gEvents->ValueChangedEvent(Addr, Size);
			this->ValueChangedEvent(Addr, Size);
		}
		void RaiseValueReadEvent(int Addr, int Size) {
			if (gEvents)
				gEvents->ValueReadEvent(Addr, Size);
			this->ValueReadEvent(Addr, Size);
		}

	protected:
		virtual void ValueChangedEvent(int Addr, int Size) {}
		virtual void ValueReadEvent(int Addr, int Size) {}
	};


	template <typename DataT>
	class ModbusDataT : public ModbusData
	{
	public:
		DataT *Data;

		ModbusDataT() {
			Data = 0;
		}

		ModbusDataT(int Address, int Length) : ModbusData(Address, Length) {
			Data = new DataT[Length];
		}

		ModbusDataT(int Address, DataT * bData, int Length)  : ModbusData(Address, Length) {
			Data = bData;
		}
	
		void  AddData(int aAddress, int Length)
		{
			if (MySize == 0) {
				Init(aAddress, Length);
				Data = new DataT[Length];
			} else {
				AddModbusData(new ModbusDataT<DataT>(aAddress, Length));
			}
		}

		void AddData(int aAddress, DataT *bData, int Length)
		{
			if (MySize == 0) {
				Init(aAddress, Length);
				Data = bData;
			} else {
				AddModbusData(new ModbusDataT<DataT>(aAddress, bData, Length));
			}
		}

	protected:
		void ReadMultiple(MBSFrame *Frame);
		void WriteMultiple(MBSFrame *Frame);
		void WriteSingle(MBSFrame *Frame);
	};

	class ModbusRegsData : public ModbusDataT<uint16_t>
	{
	public:
		ModbusRegsData() :ModbusDataT() {}
		ModbusRegsData(int Address, int Length) : ModbusDataT(Address, Length) {}
		ModbusRegsData(int Address, uint16_t*  Data, int Length) : ModbusDataT(Address, Data, Length) {}
	};

	class ModbusCoilsData : ModbusDataT<coil_t>
	{
	public:
		ModbusCoilsData() : ModbusDataT() {}
		ModbusCoilsData(int Address, int Length) : ModbusDataT(Address, Length) {}
		ModbusCoilsData(int Address, coil_t * Data, int Length) : ModbusDataT(Address, Data, Length) {}
	};
}


