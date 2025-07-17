#ifndef _SP3485EN_H_
#define _SP3485EN_H_


#include "main.h"
#include "stdint.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_uart.h"

typedef enum
{
	UsartTransmitMode = 0U,
	UsartReceiveMode,

}USART_ModeSet;


#define USART_485_PIN_TxMODE   GPIOA,GPIO_PIN_1,GPIO_PIN_SET
#define USART_485_PIN_RxMODE   GPIOA,GPIO_PIN_1,GPIO_PIN_RESET
extern UART_HandleTypeDef huart2;
extern void MX_USART2_UART_Init(void);
extern void Uart_485_Init(void);
extern HAL_StatusTypeDef RS485_SendData_DMA(uint8_t* data, uint16_t size);
extern HAL_StatusTypeDef RS485_ReceiveData_DMA(uint8_t* buffer, uint16_t size);


#endif