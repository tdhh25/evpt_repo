#include "dwin_driver.h"

static struct dwin_device Dd_valDevice_Mp;

void Dwin_ProcessChannelData(uint8_t Dd_valChannel)
{
	uint8_t Dd_valIndex_Lo = 0;
    uint8_t Dd_valSendData_Lo[4] = {0};
    uint8_t Dd_valOffsets_Lo[5] = {0, 2, 4, 6, 8};
    uint16_t Dd_valBaseAddr_Lo = 0, Dd_valAddr_Lo = 0, Dd_valValue_Lo = 0;
    struct dwin_value* Dd_valChannelData_Lo = NULL;
    uint16_t* Dd_valPointers_Lo[5] = {NULL, NULL, NULL, NULL, NULL};

	if(Dd_valChannel >= 10)
		return;
	
    Dd_valChannelData_Lo = Dwin_GetChannelData(Dd_valChannel);
    if (NULL == Dd_valChannelData_Lo)
		return;

    Dd_valBaseAddr_Lo = Dd_valChannelData_Lo->base_addr;
    Dd_valPointers_Lo[0] = &Dd_valChannelData_Lo->average;
    Dd_valPointers_Lo[1] = &Dd_valChannelData_Lo->max;
    Dd_valPointers_Lo[2] = &Dd_valChannelData_Lo->min;
    Dd_valPointers_Lo[3] = &Dd_valChannelData_Lo->average;
    Dd_valPointers_Lo[4] = &Dd_valChannelData_Lo->wave;

    for (Dd_valIndex_Lo = 0; Dd_valIndex_Lo < 5; Dd_valIndex_Lo++)
	{
        Dd_valAddr_Lo = Dd_valBaseAddr_Lo + Dd_valOffsets_Lo[Dd_valIndex_Lo];
        Dd_valValue_Lo = *Dd_valPointers_Lo[Dd_valIndex_Lo];

        Dd_valSendData_Lo[0] = (Dd_valAddr_Lo >> 8) & 0xFF;
        Dd_valSendData_Lo[1] = Dd_valAddr_Lo & 0xFF;
        Dd_valSendData_Lo[2] = (Dd_valValue_Lo >> 8) & 0xFF;
        Dd_valSendData_Lo[3] = Dd_valValue_Lo & 0xFF;
        
        Dwin_SendData(Dd_valSendData_Lo, 4);
    }
}

void Dwin_MainFunction(void)
{
	uint8_t Dd_valIndex_Lo = 0;
	
	Dd_valIndex_Lo = Dd_valDevice_Mp.step % 10;
	Dd_valDevice_Mp.step++;
	
	Dwin_ProcessChannelData(Dd_valIndex_Lo);
}

