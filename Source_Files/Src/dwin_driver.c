#include "dwin_driver.h"

static struct dwin_device Dd_valDevice_Mp;

void Dwin_ProcessTouchData()
{

}

void Dwin_ProcessChannelData(uint8_t Dd_valChannel)
{
	uint8_t i = 0;
    uint8_t Dd_valSendData_Lo[12] = {0};
    uint8_t Dd_valOffsets_Lo[5] = {0, 2, 4, 6, 8};
    uint16_t Dd_valBaseAddr_Lo = 0, Dd_valValue_Lo = 0;
    struct dwin_value* Dd_valChannelData_Lo = NULL;
    uint16_t* Dd_valPointers_Lo[5] = {NULL, NULL, NULL, NULL, NULL};

	if(Dd_valChannel >= 10)
		return;
	
    Dd_valChannelData_Lo = Dwin_GetChannelData(Dd_valChannel);
    if (NULL == Dd_valChannelData_Lo)
		return;

    Dd_valBaseAddr_Lo = Dd_valChannelData_Lo->base_addr;
    Dd_valPointers_Lo[0] = &Dd_valChannelData_Lo->current;
    Dd_valPointers_Lo[1] = &Dd_valChannelData_Lo->max;
    Dd_valPointers_Lo[2] = &Dd_valChannelData_Lo->min;
    Dd_valPointers_Lo[3] = &Dd_valChannelData_Lo->average;
    Dd_valPointers_Lo[4] = &Dd_valChannelData_Lo->wave;

	for(i = 0;i < 5;i++)
	{
		Dd_valValue_Lo = *Dd_valPointers_Lo[i];
		Dd_valSendData_Lo[2 + 2 * i] = (Dd_valValue_Lo >> 8) & 0xFF;
		Dd_valSendData_Lo[3 + 2 * i] = (Dd_valValue_Lo >> 0) & 0xFF;
	}
	Dd_valSendData_Lo[0] = (Dd_valBaseAddr_Lo >> 8) & 0xFF;
	Dd_valSendData_Lo[1] = Dd_valBaseAddr_Lo & 0xFF;
	
	Dwin_SendData(Dd_valSendData_Lo, 12);
}

void Dwin_MainFunction(void)
{
	Dd_valDevice_Mp.step = Dd_valDevice_Mp.step % 10;
	Dd_valDevice_Mp.step++;
	
    //Dwin_ProcessChannelData(Dd_valDevice_Mp.step);
	Dwin_ProcessChannelData(Dd_valDevice_Mp.step);
}
