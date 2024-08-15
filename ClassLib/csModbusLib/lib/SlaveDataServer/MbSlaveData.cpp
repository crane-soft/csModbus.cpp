#include "MbSlaveData.h"
namespace csModbusLib {

	template<> void ModbusDataT<coil_t>::ReadMultiple(MBSFrame *Frame)
	{
		Frame->PutBitValues(MyBaseAddr, Data);
	}
	template<> void ModbusDataT<uint16_t>::ReadMultiple(MBSFrame *Frame)
	{
		Frame->PutValues(MyBaseAddr, Data);
	}
	template<> void ModbusDataT<coil_t>::WriteMultiple(MBSFrame *Frame)
	{
		Frame->GetBitValues(MyBaseAddr, Data);
	}
	template<> void ModbusDataT<uint16_t>::WriteMultiple(MBSFrame *Frame)
	{
		Frame->GetValues(MyBaseAddr, Data);
	}
	template<> void ModbusDataT<coil_t>::WriteSingle(MBSFrame *Frame)
	{
		Data[Frame->DataAddress - MyBaseAddr] = Frame->GetSingleBit();
	}
	template<> void ModbusDataT<uint16_t>::WriteSingle(MBSFrame *Frame)
	{
		Data[Frame->DataAddress - MyBaseAddr] = Frame->GetSingleUInt16();
	}
}
