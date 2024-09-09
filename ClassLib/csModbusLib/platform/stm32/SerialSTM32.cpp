
#include "SerialSTM32.h"


SerialSTM32::SerialSTM32()
{
	mIsOPen = false;
}

bool SerialSTM32::OpenPort()
{
	mIsOPen = false;
	huart.Instance = STM_Uart();
	huart.Init.BaudRate = BaudRate;
	huart.Init.WordLength = STM_DataLen();
	huart.Init.StopBits = STM_Stopbis ();
	huart.Init.Parity = STM_Parity();
	
	huart.Init.Mode = UART_MODE_TX_RX;
	huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart.Init.OverSampling = UART_OVERSAMPLING_16;
	huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_RS485Ex_Init(&huart, UART_DE_POLARITY_HIGH, 0, 0) != HAL_OK) {
		return false;
	}
	mIsOPen = true;
	return true;
}

bool SerialSTM32::IsOpen()
{
	return mIsOPen;
}

void SerialSTM32::Close()
{
	
}

void SerialSTM32::SetTimeouts()
{
}

void SerialSTM32::DiscardInOut()
{

}

void SerialSTM32::Write(const uint8_t * Data, int offs, int count)
{

}

int SerialSTM32::Read(uint8_t * Data, int offs, int count)
{
	return 0;
}


int SerialSTM32::BytesToRead()
{
	return 0;
}; 

USART_TypeDef * SerialSTM32::STM_Uart() const
{
	int uart_nr = PortName[0] - 0x30;
	switch (uart_nr) {
		case 1:
			return USART1;
		case 2:
			return USART2;
	}
	return 0;
}


uint32_t SerialSTM32::STM_DataLen () const
{
	switch (DataBits) {
	case 7:
		return UART_WORDLENGTH_7B;
	case 9:
		return UART_WORDLENGTH_9B;
	default:
		return UART_WORDLENGTH_8B;
	}
}
uint32_t SerialSTM32::STM_Stopbis () const
{
	switch ((int)mStopBits) {
	case (int)SerialPort::StopBits::OnePointFive:
		return UART_STOPBITS_1_5;
	case (int)SerialPort::StopBits::Two:
		return UART_STOPBITS_2;
	default:
		return UART_STOPBITS_1;
	}
}

uint32_t SerialSTM32::STM_Parity () const
{
	switch ((int)mParity) {
		case (int)SerialPort::Parity::Odd:
			return UART_PARITY_ODD;
		case (int)SerialPort::Parity::Even:
			return UART_PARITY_EVEN;
		default:
			return UART_PARITY_NONE;
	}
}

