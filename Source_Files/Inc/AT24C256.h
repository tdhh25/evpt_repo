#ifndef _AT24C256_H_
#define _AT24C256_H_

#include "main.h"
#include "stm32f4xx_hal.h"



#define AT24C_ADDR_WRITE 0xA0       // 写地址（A0/A1/A2接地时）
#define AT24C_ADDR_READ  0xA1       // 读地址
#define AT24C_PAGE_SIZE  64         // 页大小64字节（）
#define AT24C_TOTAL_SIZE 32768      // AT24C128C总容量（32768字节，）
#define AT24C_MAX_ADDR   		(AT24C_TOTAL_SIZE - 1) // 最大地址0x3FFF
#define AT24C_WRITE_TIMEOUT 5       // 最大写周期5ms（）

// 状态机定义（覆盖读写全流程，基于操作时序）
typedef enum
{
    AT24C_STATE_WRITE_SEND,         // 写-发送页数据（）
    AT24C_STATE_WRITE_WAIT,         // 写-等待内部写周期完成（）
    AT24C_STATE_READ_ADDR_SEND,     // 读-发送读地址（虚拟写，）
    AT24C_STATE_READ_RECV,           // 读-接收数据（）
		AT24C_STATE_IDEL           // 读-接收数据（）
} AT24C_State;

// 异步操作结构体（统一管理读写参数）
typedef struct
{
    AT24C_State state;              // 当前操作状态
    uint16_t curr_addr;             // 当前操作地址
    uint16_t remain_len;            // 剩余未操作长度
    uint8_t *data_buf;              // 数据缓冲区（读写共用）
    uint8_t is_busy;                // 操作忙标志（0=空闲，1=忙）
    uint8_t result;                 // 操作结果（0=成功，非0=错误）
} AT24C_Async_t;


typedef enum
{
	
	AT24C_TransmitIdel = 0U,
	AT24C_TransmitBusy,
	AT24C_InvaildParameter,
	
}AT24C_RetType_TDEM;

extern I2C_HandleTypeDef hi2c1;

extern void AT24C_AsyncPolling_Function(void);
extern AT24C_RetType_TDEM AT24C_EEpromRead_Function(uint16_t addr, uint8_t *data, uint16_t len);
extern AT24C_RetType_TDEM AT24C_EEpromWrite_Function(uint16_t addr, uint8_t *data, uint16_t len);
static inline void Memcpy_Function(volatile uint8_t* dst, uint8_t* __RESTRICT src, uint32_t len)
{
  uint32_t i;
  for (i = 0U; i < len; ++i) 
  {
    dst[i] = src[i];
  }
}


#endif


