#include "SP3485EN.h"

static uint8_t Rb_valRxBuffer_Mp[UART_RECEIVE_LEN] = {0};
static struct ring_buffer Rb_valBuffer_Mp;

void Usart_RS485_TransmitModeConfig(USART_ModeSet Mode)
{
	if(Mode == UsartTransmitMode)
	{
		HAL_GPIO_WritePin(USART_485_PIN_TxMODE);
	}
	else
	{
		HAL_GPIO_WritePin(USART_485_PIN_RxMODE);
	}
    
}

HAL_StatusTypeDef RS485_SendData_DMA(uint8_t* data, uint16_t size)
{
    HAL_StatusTypeDef status;
    
    status = HAL_UART_Transmit_DMA(&huart2, data, size);
    
    return status;
}


HAL_StatusTypeDef RS485_ReceiveData_DMA(uint8_t* buffer, uint16_t size)
{
    return HAL_UART_Receive_DMA(&huart2, buffer, size);
}

void Uart_485_Init(void)
{
    MX_USART2_UART_Init();
	RingBuffer_Init(&Rb_valBuffer_Mp);
	RS485_ReceiveData_DMA(&Rb_valRxBuffer_Mp[0], UART_RECEIVE_LEN);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
	if (huart == &huart2)
	{
		huart->gState = HAL_UART_STATE_READY;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
	if (huart == &huart2)
	{
		if(0x83 == Rb_valRxBuffer_Mp[3])
			RingBuffer_Put(&Rb_valBuffer_Mp, Rb_valRxBuffer_Mp, UART_RECEIVE_LEN);
		RS485_ReceiveData_DMA(&Rb_valRxBuffer_Mp[0], UART_RECEIVE_LEN);
	}
}

