#include "SP3485EN.h"

uint8_t rx_complete;

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
void Uart_485_Init(void)
{
    MX_USART2_UART_Init();
    Usart_RS485_TransmitModeConfig(UsartReceiveMode);

}
HAL_StatusTypeDef RS485_SendData_DMA(uint8_t* data, uint16_t size)
{
    HAL_StatusTypeDef status;
    
    Usart_RS485_TransmitModeConfig(UsartTransmitMode);

    status = HAL_UART_Transmit_DMA(&huart2, data, size);
    
    return status;
}


HAL_StatusTypeDef RS485_ReceiveData_DMA(uint8_t* buffer, uint16_t size)
{
		Usart_RS485_TransmitModeConfig(UsartReceiveMode);

    return HAL_UART_Receive_DMA(&huart2, buffer, size);
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
    if (huart == &huart2)
		{
			huart->gState = HAL_UART_STATE_READY;
      HAL_GPIO_WritePin(USART_485_PIN_RxMODE);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    if (huart == &huart2)
		{
        rx_complete = true;
    }
}

