#pragma once
#include <stdint.h>
#include "Interface/SerialPort.h"
#include "csFifo.h"
#include "stm32f0xx_hal.h"

#define FIFO_SIZE	260
class SerialSTM32 : public SerialPort {

public:
	SerialSTM32();
	SerialSTM32(const void* _ComPort, int _BaudRate);

	bool IsOpen() override;
	void Close() override;
	void DiscardInOut() override;
	int Write(const uint8_t * Data, int count) override;
	int Read(uint8_t * Data, int count) override;
	void ReadEv(uint8_t* Data, int count) override;

	void IRQHandler();
	void EventIRQ();
	void TimerIRQ();
protected:
	bool OpenPort();

private:
	void Init();
	void  InitEventHandler();
	void InvokeReadCallBack(int result);
	void ByteReceived(uint8_t rxByte);
	void RaiseEvent();

	uint32_t STM_DataLen() const;
	uint32_t STM_Stopbis () const;
	uint32_t STM_Parity () const;
	csFifoBuff<uint8_t, FIFO_SIZE> RxFifo;
	csFifoBuff<uint8_t, FIFO_SIZE> TxFifo;

	UART_HandleTypeDef huart;
	bool mIsOPen;
	uint8_t *ReadDstPtr;
	int ReadCount;
	int RxTimeoutTimer;
	bool ReadReady;
};
