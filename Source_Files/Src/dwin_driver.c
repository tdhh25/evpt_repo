#include "dwin_driver.h"

static struct dwin_device Dd_valDevice_Mp;
static struct ring_buffer Dd_valRingBuffer_Mp;

struct ring_buffer* Dwin_GetRingBuffer(void)
{
	return &Dd_valRingBuffer_Mp;
}

void Dwin_ProcessTouchData(void)
{
	uint8_t		Dd_valRxLength_Lo = 0;
	uint8_t		Dd_valRxData_Lo[16] = {0};
	uint16_t	Dd_valAddr_Lo = 0, Dd_valData_Lo = 0;
	
	RingBuffer_Get(&Dd_valRingBuffer_Mp, &Dd_valRxData_Lo[0], &Dd_valRxLength_Lo);
	Dd_valAddr_Lo = ((Dd_valRxData_Lo[4] << 8) | Dd_valRxData_Lo[5]);
	Dd_valData_Lo = ((Dd_valRxData_Lo[7] << 8) | Dd_valRxData_Lo[8]);
	switch(Dd_valAddr_Lo)
	{
		case dwin_addr_average:
			if(DWIN_TOUCH_KEY_VALUE == Dd_valData_Lo)
			{
				Dd_valDevice_Mp.ctrl.average = 1 - Dd_valDevice_Mp.ctrl.average;
			}
			break;
		default:
			break;
	}
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

void Dwin_InitFunction(void)
{
	RingBuffer_Init(&Dd_valRingBuffer_Mp);
}

void Dwin_MainFunction(void)
{
	/* 处理触控数据 */
	Dwin_ProcessTouchData();

	Dd_valDevice_Mp.step = Dd_valDevice_Mp.step % 10;

	/* 发送采集值 */
	if(Dd_valDevice_Mp.ctrl.average)
	{
		//Dwin_ProcessChannelData(0);
		Dwin_ProcessChannelData(Dd_valDevice_Mp.step);
	}
	Dd_valDevice_Mp.step++;
}
