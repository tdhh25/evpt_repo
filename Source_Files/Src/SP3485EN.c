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
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
	RS485_ReceiveData_DMA(&Rb_valRxBuffer_Mp[0], UART_RECEIVE_LEN);
}


void USART2_IRQHandler(void)
{
    if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE) != RESET)
    {
        __HAL_UART_CLEAR_IDLEFLAG(&huart2);

    }

    HAL_UART_IRQHandler(&huart2);
}
