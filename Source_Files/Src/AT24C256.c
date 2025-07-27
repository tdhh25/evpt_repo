#include "AT24C256.h"

AT24C_Async_t at24c_async =
{
    .state = AT24C_STATE_WRITE_SEND,
    .is_busy = 0,
    .result = 0
};
#define  NULL_PTR  ((void *)0)
uint8_t *Wr_DataLastPtr = NULL_PTR;
uint16_t Wr_AddrLast = 0xFFFF;
uint16_t Wr_LenLast = 0;
#define SET_SUCCESSFUL           0
#define STATUS_BUSY              1
#define INVALID_PARAMETER        2
#define STATUS_IDEL              0
/**
 * @brief  启动异步写操作（非阻塞）
 * @param  addr：起始地址（0x0000~0x3FFF）
 * @param  data：待写数据
 * @param  len：写入长度
 * @retval 0=成功启动；1=忙；2=参数错误（地址越界/数据为空）
 * @依据：页写操作流程（）
 */
uint8_t AT24C_AsyncWriteStart(uint16_t addr, uint8_t *data, uint16_t len)
{
	if((Wr_AddrLast == addr) && (Wr_DataLastPtr == data) && (Wr_LenLast == len))
	{
		// 检查忙状态和参数合法性
    if (at24c_async.is_busy)
    {
        return STATUS_BUSY;
    }

    if (addr + len > AT24C_TOTAL_SIZE || data == NULL || len == 0)
    {
        return INVALID_PARAMETER;
    }

    // 初始化异步参数
    at24c_async.state = AT24C_STATE_WRITE_SEND;
    at24c_async.curr_addr = addr;
    at24c_async.remain_len = len;
    at24c_async.data_buf = data;
    at24c_async.is_busy = STATUS_BUSY;
    at24c_async.result = SET_SUCCESSFUL;
    return SET_SUCCESSFUL;
	
	}
	else
	{
		
	
	}
	Wr_AddrLast = addr;
	Wr_DataLastPtr = data;
	Wr_LenLast = len;
	return at24c_async.is_busy;

}

/**
 * @brief  启动异步读操作（非阻塞）
 * @param  addr：起始地址（0x0000~0x3FFF）
 * @param  data：接收缓冲区
 * @param  len：读取长度
 * @retval 0=成功启动；1=忙；2=参数错误
 * @依据：随机读操作流程（）
 */
uint8_t AT24C_AsyncReadStart(uint16_t addr, uint8_t *data, uint16_t len)
{
    // 检查忙状态和参数合法性
    if (at24c_async.is_busy)
    {
        return STATUS_BUSY;
    }

    if (addr + len > AT24C_TOTAL_SIZE || data == NULL || len == 0)
    {
        return INVALID_PARAMETER;
    }

    // 初始化异步参数
    at24c_async.state = AT24C_STATE_READ_ADDR_SEND;
    at24c_async.curr_addr = addr;
    at24c_async.remain_len = len;
    at24c_async.data_buf = data;
    at24c_async.is_busy = STATUS_BUSY;
    at24c_async.result = SET_SUCCESSFUL;
    return SET_SUCCESSFUL;
}


/**
 * @brief  合并读写的Polling函数（需在主循环定期调用）
 * @依据：写操作时序（）、读操作时序（）、ACK Polling机制（）
 */
void AT24C_AsyncPoll(void)
{

    // 非忙状态直接返回
    if (!at24c_async.is_busy)
    {
        return;
    }

    switch (at24c_async.state)
    {
        // -------------------------- 写操作处理 --------------------------
        case AT24C_STATE_WRITE_SEND:
        {
            // 1. 计算当前页可写长度（不跨页，）
            uint16_t page_start = at24c_async.curr_addr & ~(AT24C_PAGE_SIZE - 1); // 页起始地址
            uint16_t page_remain = AT24C_PAGE_SIZE - (at24c_async.curr_addr - page_start); // 页内剩余空间
            uint16_t write_len = (at24c_async.remain_len <= page_remain) ? at24c_async.remain_len : page_remain;

            // 2. 构建发送数据（2字节地址 + 数据，）
            uint8_t tx_buf[2 + AT24C_PAGE_SIZE];
            tx_buf[0] = (at24c_async.curr_addr >> 8) & 0xFF; // 地址高8位
            tx_buf[1] = at24c_async.curr_addr & 0xFF;        // 地址低8位
            Memcpy_Function(&tx_buf[2], at24c_async.data_buf, write_len);

            // 3. I2C发送（短时间阻塞，符合Polling特性）
            if (HAL_I2C_Master_Transmit(&hi2c1, AT24C_ADDR_WRITE, tx_buf, 2 + write_len, 100) != HAL_OK)
            {
                at24c_async.result = 0x02; // 发送失败
                at24c_async.is_busy = SET_SUCCESSFUL;
                return;
            }
            break;
        }

        case AT24C_STATE_WRITE_WAIT:
        {
            // 2. ACK Polling确认写完成（）
            if (HAL_I2C_IsDeviceReady(&hi2c1, AT24C_ADDR_WRITE, 1, 10) == HAL_OK)
            {
                // 3. 更新写进度
                uint16_t page_start = at24c_async.curr_addr & ~(AT24C_PAGE_SIZE - 1);
                uint16_t page_remain = AT24C_PAGE_SIZE - (at24c_async.curr_addr - page_start);
                uint16_t actual_write = (at24c_async.remain_len <= page_remain) ?
                                        at24c_async.remain_len : page_remain;

                at24c_async.curr_addr += actual_write;
                at24c_async.data_buf += actual_write;
                at24c_async.remain_len -= actual_write;

                // 4. 判断是否完成所有写入
                if (at24c_async.remain_len == 0)
                {
                    at24c_async.result = 0x00; // 全部完成
                    at24c_async.is_busy = SET_SUCCESSFUL;
                }
                else
                {
                    at24c_async.state = AT24C_STATE_WRITE_SEND; // 继续写下一页
                }
            }

            break;
        }

        // -------------------------- 读操作处理 --------------------------
        case AT24C_STATE_READ_ADDR_SEND:
        {
            // 1. 发送读地址（虚拟写操作，）
            uint8_t addr_buf[2] =
            {
                (at24c_async.curr_addr >> 8) & 0xFF,
                at24c_async.curr_addr & 0xFF
            };

            if (HAL_I2C_Master_Transmit(&hi2c1, AT24C_ADDR_WRITE, addr_buf, 2, 100) != HAL_OK)
            {
                at24c_async.result = 0x04; // 地址发送失败
                at24c_async.is_busy = 0;
                return;
            }

            // 2. 进入接收状态
            at24c_async.state = AT24C_STATE_READ_RECV;
            break;
        }

        case AT24C_STATE_READ_RECV:
        {
            // 1. 接收数据（无内部等待周期，）
            if (HAL_I2C_Master_Receive(&hi2c1, AT24C_ADDR_READ, at24c_async.data_buf,
                                       at24c_async.remain_len, 100) == HAL_OK)
            {
                at24c_async.result = 0x00; // 接收成功
            }
            else
            {
                at24c_async.result = 0x05; // 接收失败
            }

            at24c_async.is_busy = SET_SUCCESSFUL; // 读操作完成
            break;
        }
				default:
				break;
    }
}