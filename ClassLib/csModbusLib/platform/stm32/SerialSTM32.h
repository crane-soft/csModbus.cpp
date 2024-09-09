#pragma once
#include <stdint.h>
#include "SerialPort.h"
#include "stm32f0xx_hal.h"

class SerialSTM32 : public SerialPort {

public:
	SerialSTM32();
	bool IsOpen();
	void Close();
	void DiscardInOut();
	void Write(const uint8_t * Data, int offs, int count);
	int Read(uint8_t * Data, int offs, int count);
	int BytesToRead();

protected:
	bool OpenPort();
	void SetTimeouts();
private:
	UART_HandleTypeDef huart;
	bool mIsOPen;
	USART_TypeDef *STM_Uart() const;
	uint32_t STM_DataLen() const;
	uint32_t STM_Stopbis () const;
	uint32_t STM_Parity () const;

};
