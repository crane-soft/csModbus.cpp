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

	bool IsOpen();
	void Close();
	void DiscardInOut();
	void Write(const uint8_t * Data, int offs, int count);
	int Read(uint8_t * Data, int offs, int count);
	int BytesToRead();
	void IRQHandler();
	void EventIRQ();
protected:
	bool OpenPort();
	void SetTimeouts();
	bool StartEventHandler() override;
private:
	void ByteReceived(uint8_t rxByte);
	UART_HandleTypeDef huart;
	bool mIsOPen;
	uint32_t STM_DataLen() const;
	uint32_t STM_Stopbis () const;
	uint32_t STM_Parity () const;
	csFifoBuff<uint8_t, FIFO_SIZE> RxFifo;
	csFifoBuff<uint8_t, FIFO_SIZE> TxFifo;
};
