#include "SP3485EN.h"

static uint8_t Rb_valRxLength = 0;
static uint8_t Rb_valRxBuffer_Mp[UART_RECEIVE_LEN] = {0};

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
    
    status = HAL_UART_Transmit_DMA(&huart1, data, size);
    
    return status;
}


HAL_StatusTypeDef RS485_ReceiveData_DMA(uint8_t* buffer, uint16_t size)
{
    return HAL_UART_Receive_DMA(&huart1, buffer, size);
}

void Uart_485_Init(void)
{
	MX_USART2_UART_Init();
	RS485_ReceiveData_DMA(&Rb_valRxBuffer_Mp[0], UART_RECEIVE_LEN);
}

void HAL_UART_IdleCallback(UART_HandleTypeDef *huart)
{
	struct ring_buffer* Rb_valRingBuffer_Lo = NULL;
	
	if (huart == &huart1)
	{
		HAL_UART_DMAStop(&huart1);
		Rb_valRxLength = UART_RECEIVE_LEN - __HAL_DMA_GET_COUNTER(huart->hdmarx);
		if(Rb_valRxLength > 6)
		{
			Rb_valRingBuffer_Lo = Dwin_GetRingBuffer();
			if((Rb_valRingBuffer_Lo) && (0x83 == Rb_valRxBuffer_Mp[3]))
				RingBuffer_Put(Rb_valRingBuffer_Lo, Rb_valRxBuffer_Mp, Rb_valRxLength);
		}
		RS485_ReceiveData_DMA(&Rb_valRxBuffer_Mp[0], UART_RECEIVE_LEN);
	}
	else if((huart == &huart2))
	{
			HAL_UART_DMAStop(&huart2);
			Rb_valRxLength = UART_RECEIVE_LEN - __HAL_DMA_GET_COUNTER(huart->hdmarx);
			if(Rb_valRxLength > 6)
			{
				Rb_valRingBuffer_Lo = Dwin_GetRingBuffer();
				if((Rb_valRingBuffer_Lo) && (0x83 == Rb_valRxBuffer_Mp[3]))
					RingBuffer_Put(Rb_valRingBuffer_Lo, Rb_valRxBuffer_Mp, Rb_valRxLength);
			}
			RS485_ReceiveData_DMA(&Rb_valRxBuffer_Mp[0], UART_RECEIVE_LEN);
	}
}

// void USART2_IRQHandler(void)
// {
//     if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE) != RESET)
//     {
//         __HAL_UART_CLEAR_IDLEFLAG(&huart2);
// 		HAL_UART_IdleCallback(&huart2);
//     }
// }
/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET)
    {
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);
		HAL_UART_IdleCallback(&huart1);
    }
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
    if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE) != RESET)
    {
        __HAL_UART_CLEAR_IDLEFLAG(&huart2);
		HAL_UART_IdleCallback(&huart2);
    }
}
