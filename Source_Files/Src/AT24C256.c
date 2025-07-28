#include "AT24C256.h"

static AT24C_Async_t AT24C_StateMachine =
{
    .state = AT24C_STATE_IDEL,
    .is_busy = 0,
    .result = 0
};

static uint16_t Wr_AddrLast = 0xFFFF;
static uint16_t Wr_LenLast = 0;

static uint16_t Rd_AddrLast = 0xFFFF;
static uint16_t Rd_LenLast = 0;



AT24C_RetType_TDEM AT24C_EEpromWrite_Function(uint16_t addr, uint8_t *data, uint16_t len)
{
    AT24C_RetType_TDEM valRet = AT24C_TransmitIdel;

    if((Wr_AddrLast != addr) && (Wr_LenLast != len))
    {
        if (addr + len > AT24C_TOTAL_SIZE || data == NULL || len == 0)
        {
            valRet = AT24C_InvaildParameter;
        }

        // 初始化异步参数
        AT24C_StateMachine.state = AT24C_STATE_WRITE_SEND;
        AT24C_StateMachine.curr_addr = addr;
        AT24C_StateMachine.remain_len = len;
        AT24C_StateMachine.data_buf = data;
        AT24C_StateMachine.is_busy = AT24C_TransmitBusy;
        AT24C_StateMachine.result = 0;

    }
    else
    {


    }

    Wr_AddrLast = addr;
    Wr_LenLast = len;
    valRet = AT24C_StateMachine.is_busy;
    return valRet;

}


AT24C_RetType_TDEM AT24C_EEpromRead_Function(uint16_t addr, uint8_t *data, uint16_t len)
{

    AT24C_RetType_TDEM valRet = AT24C_TransmitIdel;

    if((Rd_AddrLast != addr) && (Rd_LenLast != len))
    {
        if (addr + len > AT24C_TOTAL_SIZE || data == NULL || len == 0)
        {
            valRet =  AT24C_InvaildParameter;
        }

        // 初始化异步参数
        AT24C_StateMachine.state = AT24C_STATE_READ_ADDR_SEND;
        AT24C_StateMachine.curr_addr = addr;
        AT24C_StateMachine.remain_len = len;
        AT24C_StateMachine.data_buf = data;
        AT24C_StateMachine.is_busy = AT24C_TransmitBusy;
        AT24C_StateMachine.result = 0;

    }
    else
    {

    }

    Rd_AddrLast = addr;
    Rd_LenLast = len;
    valRet = AT24C_StateMachine.is_busy;
    return valRet;
}



void AT24C_AsyncPolling_Function(void)
{

    if (!AT24C_StateMachine.is_busy)
    {
        return;
    }

    switch (AT24C_StateMachine.state)
    {
        case AT24C_STATE_WRITE_SEND:
        {
            uint16_t page_start = AT24C_StateMachine.curr_addr & ~(AT24C_PAGE_SIZE - 1); // 页起始地址
            uint16_t page_remain = AT24C_PAGE_SIZE - (AT24C_StateMachine.curr_addr - page_start); // 页内剩余空间
            uint16_t write_len = (AT24C_StateMachine.remain_len <= page_remain) ? AT24C_StateMachine.remain_len : page_remain;

            uint8_t tx_buf[2 + AT24C_PAGE_SIZE];
            tx_buf[0] = (AT24C_StateMachine.curr_addr >> 8) & 0xFF;
            tx_buf[1] = AT24C_StateMachine.curr_addr & 0xFF;
            Memcpy_Function(&tx_buf[2], AT24C_StateMachine.data_buf, write_len);

            if (HAL_I2C_Master_Transmit(&hi2c1, AT24C_ADDR_WRITE, tx_buf, 2 + write_len, 100) != HAL_OK)
            {
                AT24C_StateMachine.state  = AT24C_STATE_WRITE_SEND;
            }
            else
            {
                AT24C_StateMachine.state = AT24C_STATE_WRITE_WAIT;
            }

            break;
        }

        case AT24C_STATE_WRITE_WAIT:
        {
            if (HAL_I2C_IsDeviceReady(&hi2c1, AT24C_ADDR_WRITE, 1, 10) == HAL_OK)
            {
                uint16_t page_start = AT24C_StateMachine.curr_addr & ~(AT24C_PAGE_SIZE - 1);
                uint16_t page_remain = AT24C_PAGE_SIZE - (AT24C_StateMachine.curr_addr - page_start);
                uint16_t actual_write = (AT24C_StateMachine.remain_len <= page_remain) ? AT24C_StateMachine.remain_len : page_remain;

                AT24C_StateMachine.curr_addr += actual_write;
                AT24C_StateMachine.data_buf += actual_write;
                AT24C_StateMachine.remain_len -= actual_write;

                if (AT24C_StateMachine.remain_len == 0)
                {
                    AT24C_StateMachine.state = AT24C_STATE_IDEL;
                }
                else
                {
                    AT24C_StateMachine.state = AT24C_STATE_WRITE_SEND;
                }
            }

            break;
        }

        case AT24C_STATE_READ_ADDR_SEND:
        {
            uint8_t addr_buf[2] =
            {
                (AT24C_StateMachine.curr_addr >> 8) & 0xFF,
                AT24C_StateMachine.curr_addr & 0xFF
            };

            if (HAL_I2C_Master_Transmit(&hi2c1, AT24C_ADDR_WRITE, addr_buf, 2, 100) != HAL_OK)
            {
                AT24C_StateMachine.state = AT24C_STATE_READ_ADDR_SEND;
            }
            else
            {
                AT24C_StateMachine.state = AT24C_STATE_READ_RECV;

            }

            break;
        }

        case AT24C_STATE_READ_RECV:
        {
            if (HAL_I2C_Master_Receive(&hi2c1, AT24C_ADDR_READ, AT24C_StateMachine.data_buf, AT24C_StateMachine.remain_len, 100) == HAL_OK)
            {
                AT24C_StateMachine.state = AT24C_STATE_IDEL;

            }
            else
            {
                AT24C_StateMachine.state = AT24C_STATE_READ_ADDR_SEND;
            }


            break;
        }

        case AT24C_STATE_IDEL:
            AT24C_StateMachine.is_busy = AT24C_TransmitIdel;
            break;

        default:
            break;
    }
}

