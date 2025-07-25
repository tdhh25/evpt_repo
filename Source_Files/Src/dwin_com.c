#include "dwin_driver.h"

static uint8_t Dc_valDwinSendData_Mp[16];

void Dwin_SendData(uint8_t* Dc_valData, uint16_t Dc_valLength)
{
    uint8_t Dc_valDataCnt_Lo = 1, Dc_valDataPos_Lo = 4;

    if (!Dc_valData || Dc_valLength > 12)
        return;

    for (uint16_t i = 0; i < Dc_valLength; i++) {
        Dc_valDwinSendData_Mp[Dc_valDataPos_Lo] = Dc_valData[i];
        Dc_valDataPos_Lo++;
        Dc_valDataCnt_Lo++;
    }

    Dc_valDwinSendData_Mp[0] = 0x5A;
    Dc_valDwinSendData_Mp[1] = 0xA5;
    Dc_valDwinSendData_Mp[2] = Dc_valDataCnt_Lo;
    Dc_valDwinSendData_Mp[3] = 0x82;

    RS485_SendData_DMA(&Dc_valDwinSendData_Mp[0], Dc_valDataPos_Lo);
}
