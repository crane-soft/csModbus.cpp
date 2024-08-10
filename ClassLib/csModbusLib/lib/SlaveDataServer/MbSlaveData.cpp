#include "MbSlaveData.h"
namespace csModbusLib {

	template<> void ModbusDataT<coil_t>::ReadMultiple(MBSFrame *Frame)
	{
		Frame->PutResponseBitValues(MyBaseAddr, Data);
	}
	template<> void ModbusDataT<uint16_t>::ReadMultiple(MBSFrame *Frame)
	{
		Frame->PutResponseValues(MyBaseAddr, Data);
	}
	template<> void ModbusDataT<coil_t>::WriteMultiple(MBSFrame *Frame)
	{
		Frame->GetRequestBitValues(MyBaseAddr, Data);
	}
	template<> void ModbusDataT<uint16_t>::WriteMultiple(MBSFrame *Frame)
	{
		Frame->GetRequestValues(MyBaseAddr, Data);
	}
	template<> void ModbusDataT<coil_t>::WriteSingle(MBSFrame *Frame)
	{
		Data[Frame->DataAddress - MyBaseAddr] = Frame->GetRequestSingleBit();
	}
	template<> void ModbusDataT<uint16_t>::WriteSingle(MBSFrame *Frame)
	{
		Data[Frame->DataAddress - MyBaseAddr] = Frame->GetRequestSingleUInt16();
	}
}
