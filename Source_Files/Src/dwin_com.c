#include "dwin_driver.h"

void Dwin_SendData(uint8_t* Dc_valData, uint16_t Dc_valLength)
{
	uint8_t 	Dc_valDataPos_Lo = 4, Dc_valDataCnt_Lo = 1;
	uint8_t 	Dc_valSendData_Lo[8] = {0x00};
	
	//TODO
	if(!Dc_valData) 
		return;
	
	while(Dc_valLength--)
	{
		Dc_valSendData_Lo[Dc_valDataPos_Lo] = *Dc_valData++;
		Dc_valDataPos_Lo++;
		Dc_valDataCnt_Lo++;
	}
	Dc_valSendData_Lo[0] = 0x5A;
	Dc_valSendData_Lo[1] = 0xA5;
	Dc_valSendData_Lo[2] = Dc_valDataCnt_Lo;
	Dc_valSendData_Lo[3] = 0x82;
	
	RS485_SendData_DMA(&Dc_valSendData_Lo[0], Dc_valDataPos_Lo);
}
