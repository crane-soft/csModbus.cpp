
#include "SerialSTM32.h"

#define USART_PRIORITY	0
#define EVENT_PRIORITY	3

#define STM_EVENT_MASK	0x10

SerialSTM32::SerialSTM32()
{
	mIsOPen = false;
}

SerialSTM32::SerialSTM32(const void* _ComPort, int _BaudRate)
	: SerialPort(_ComPort, _BaudRate)
{
	mIsOPen = false;
	RxTimeoutTimer = 0;
}

bool SerialSTM32::OpenPort()
{
	mIsOPen = false;
	if (this->ComPort == 0)
		return false;
	
	huart.Instance = (USART_TypeDef*)ComPort;
	huart.Init.BaudRate = BaudRate;
	huart.Init.WordLength = STM_DataLen();
	huart.Init.StopBits = STM_Stopbis ();
	huart.Init.Parity = STM_Parity();
	
	huart.Init.Mode = UART_MODE_TX_RX;
	huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart.Init.OverSampling = UART_OVERSAMPLING_16;
	huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart) != HAL_OK) {
		return false;
	}
	
	DiscardInOut();
	InitEventHandler();

    HAL_NVIC_SetPriority(USART2_IRQn, USART_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);

    ATOMIC_SET_BIT(huart.Instance->CR1, USART_CR1_RXNEIE);
	
	mIsOPen = true;
	return true;
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

bool SerialSTM32::IsOpen()
{
	return mIsOPen;
}

void SerialSTM32::Close()
{
    ATOMIC_CLEAR_BIT(huart.Instance->CR1, USART_CR1_RXNEIE);
	ATOMIC_CLEAR_BIT(huart.Instance->CR1, USART_CR1_TXEIE);
}


void SerialSTM32::DiscardInOut()
{
	RxFifo.Clear();
	TxFifo.Clear();
}

void SerialSTM32::InitEventHandler()
{
	EXTI->IMR |= STM_EVENT_MASK;
	EXTI->EMR |= STM_EVENT_MASK;
    HAL_NVIC_SetPriority(EXTI4_15_IRQn, EVENT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}

int SerialSTM32::Write(const uint8_t * Data, int count)
{
	const uint8_t * dptr = Data;
	for (int i= 0; i < count; ++i) {
		TxFifo.Write (*dptr++);
	}
	ATOMIC_SET_BIT(huart.Instance->CR1, USART_CR1_TXEIE);
	return count;
}

int SerialSTM32::Read(uint8_t * Data, int count)
{
	uint8_t * dptr = Data;
	RxTimeoutTimer = ReadTimeout;
	for (int i= 0; i < count; ++i) {
		while (RxFifo.Available() == 0)
			__WFI();
			if (RxTimeoutTimer < 0)
				return 0;
		*dptr ++ = RxFifo.Read();
	}
	return count;
}

void SerialSTM32::ReadEv(uint8_t* Data,  int count)
{
	int Bytes2Read = count;
	int BytesAvail = RxFifo.Available();
	if (BytesAvail  < Bytes2Read) {
		Bytes2Read = BytesAvail;
	}
	int BytesLeft = count - Bytes2Read;
	uint8_t * dptr = Data;

	while (Bytes2Read--) {
		*dptr ++ = RxFifo.Read();
	}
	if (BytesLeft > 0) {
		RxTimeoutTimer = ReadTimeout;
		ReadDstPtr = dptr;
		ReadCount =  BytesLeft;
	}
}


void SerialSTM32::IRQHandler()
{
	uint32_t isrflags   = READ_REG(huart.Instance->ISR);
	uint32_t cr1its     = READ_REG(huart.Instance->CR1);
	//uint32_t cr3its     = READ_REG(huart.Instance->CR3);
	uint32_t errorflags;

	
	errorflags = (isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE | USART_ISR_RTOF));
	if (errorflags == 0U) {
		// UART RX
		if (((isrflags & USART_ISR_RXNE) != 0U) && ((cr1its & USART_CR1_RXNEIE) != 0U)) {
			uint16_t uhdata = (uint16_t) READ_REG(huart.Instance->RDR);
			ByteReceived ((uint8_t)uhdata);
			__HAL_UART_SEND_REQ(&huart, UART_RXDATA_FLUSH_REQUEST);		
			return;
		}
	} else {
      __HAL_UART_CLEAR_FLAG(&huart, UART_CLEAR_PEF | UART_CLEAR_FEF | UART_CLEAR_NEF | UART_CLEAR_OREF | UART_CLEAR_RTOF);
	}
	
	// UART-TX
	if (((isrflags & USART_ISR_TXE) != 0U) && ((cr1its & USART_CR1_TXEIE) != 0U)) {
		if (TxFifo.Available()) {
			uint32_t TxVal = TxFifo.Read();
			huart.Instance->TDR = TxVal;
		} else {
			// No data to send the disable tx interrupt
			ATOMIC_CLEAR_BIT(huart.Instance->CR1, USART_CR1_TXEIE);
		}
	}
}

void SerialSTM32::ByteReceived(uint8_t rxByte)
{
	if (RxFifo.FreeLeft() > 0)
		RxFifo.Write (rxByte);
	// Raise Rx-Event
	EXTI->SWIER |= STM_EVENT_MASK;
}


void SerialSTM32::EventIRQ()
{
	// Uart RX-Event (not IRQ)
	EXTI->PR = STM_EVENT_MASK;
	while (ReadCount > 0) {
		if (RxFifo.Available() == 0)
			break;
		* ReadDstPtr++ = RxFifo.Read();
		if (--ReadCount == 0)
			InvokeReadCallBack (1);
	}
}

void SerialSTM32::TimerIRQ()
{
	if (RxTimeoutTimer > 0) {
		if (--RxTimeoutTimer == 0) {
			RxTimeoutTimer = -1;
			if (ReadCount > 0) {
				ReadCount = 0;
				// TODO you have to leave the timer IRQ here
				// raise event unstead (same as UART IRQ)
				InvokeReadCallBack (0);
			}
		}
	}
}

void SerialSTM32::InvokeReadCallBack(int result)
{
	if (ReadCallBack) {
		 ReadCallBack(result);
	}
}

