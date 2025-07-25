#include "dwin_driver.h"

static struct dwin_device Dd_valDevice_Mp;

void Dwin_ProcessChannelData(uint8_t Dd_valChannel, uint8_t Dd_valIndex)
{
    uint8_t Dd_valSendData_Lo[4] = {0};
    uint8_t Dd_valOffsets_Lo[5] = {0, 2, 4, 6, 8};
    uint16_t Dd_valBaseAddr_Lo = 0, Dd_valAddr_Lo = 0, Dd_valValue_Lo = 0;
    struct dwin_value* Dd_valChannelData_Lo = NULL;
    uint16_t* Dd_valPointers_Lo[5] = {NULL, NULL, NULL, NULL, NULL};

	if((Dd_valChannel >= 10) || (Dd_valIndex >= 5))
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

	Dd_valAddr_Lo = Dd_valBaseAddr_Lo + Dd_valOffsets_Lo[Dd_valIndex];
	Dd_valValue_Lo = *Dd_valPointers_Lo[Dd_valIndex];

	Dd_valSendData_Lo[0] = (Dd_valAddr_Lo >> 8) & 0xFF;
	Dd_valSendData_Lo[1] = Dd_valAddr_Lo & 0xFF;
	Dd_valSendData_Lo[2] = (Dd_valValue_Lo >> 8) & 0xFF;
	Dd_valSendData_Lo[3] = Dd_valValue_Lo & 0xFF;
	
	Dwin_SendData(Dd_valSendData_Lo, 4);
}

void Dwin_MainFunction(void)
{
	struct dwin_scheduler* Dd_valScheduler_Lo = NULL;
	
	Dd_valScheduler_Lo = &Dd_valDevice_Mp.scheduler;
    Dwin_ProcessChannelData(Dd_valScheduler_Lo->channel, Dd_valScheduler_Lo->index);

    Dd_valScheduler_Lo->index++;
    if(Dd_valScheduler_Lo->index >= 5) {
        Dd_valScheduler_Lo->index = 0;
        Dd_valScheduler_Lo->channel = (Dd_valScheduler_Lo->channel + 1) % 10;
    }
}
