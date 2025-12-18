#include "dwin_driver.h"

static uint16_t				Dd_valZero_Mp; 
static struct ring_buffer	Dd_valRingBuffer_Mp;
static struct dwin_device	Dd_valDevice_Mp;
static struct dwin_device*	Dd_valDevPtr_Mp = &Dd_valDevice_Mp;

/****************************** 处理函数 ******************************/
struct ring_buffer* Dwin_GetRingBuffer(void)
{
	return &Dd_valRingBuffer_Mp;
}

struct dwin_device* Dwin_GetDwinInfo(void)
{
	return Dd_valDevPtr_Mp;
}

/****************************** 硬件操作 ******************************/
/*
 * @brief 实现小数点的切换, 描述指针占16/实际操作0x06
 */
void Dwin_SwitchFrac(uint16_t Dwin_valAddr, uint8_t Dwin_valCmd)
{
	uint8_t	Dd_valBitFrac_Lo = 0;
	uint8_t	Dd_valSendData_Lo[4] = {0};

	if(Dwin_valCmd)
	{
		Dd_valBitFrac_Lo = 2;
	}
	else
	{
		Dd_valBitFrac_Lo = 1;
	}

	Dd_valSendData_Lo[0] = (Dwin_valAddr >> 8) & 0xFF;
	Dd_valSendData_Lo[1] = (Dwin_valAddr >> 0) & 0xFF;
	Dd_valSendData_Lo[2] = Dd_valBitFrac_Lo;
	Dd_valSendData_Lo[3] = 0;

	Dwin_SendData(Dd_valSendData_Lo, 4);
}

/*
 * @brief 处理按键数据
 */
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
	{
		if(DWIN_TOUCH_KEY_VALUE == Dd_valData_Lo)
		{
			Dd_valDevPtr_Mp->ctrl.average = 1 - Dd_valDevPtr_Mp->ctrl.average;
		}
		break;
	}
	case dwin_addr_param_config_e:
	{
		if(DWIN_TOUCH_KEY_VALUE == Dd_valData_Lo)
		{
			Dd_valDevPtr_Mp->ctrl.config_ip = 1;
		}
		break;
	}
	case dwin_addr_frac:
	{
		if(DWIN_TOUCH_KEY_VALUE == Dd_valData_Lo)
		{
			Dd_valDevPtr_Mp->ctrl.fractional = 1;
		}
		break;
	}
	case dwin_addr_hold:
	{
		if(DWIN_TOUCH_KEY_VALUE == Dd_valData_Lo)
		{
			Dd_valDevPtr_Mp->ctrl.hold = 1 - Dd_valDevPtr_Mp->ctrl.hold;
		}
		break;
	}
	case dwin_addr_param_config_r:
	{
		if(DWIN_TOUCH_KEY_VALUE == Dd_valData_Lo)
		{
			Dd_valDevPtr_Mp->ctrl.config_ip = 0;
		}
		break;
	}
	default:
		break;
	}
}

/*
 * @brief 实现所有通道的数据发送
 */
void Dwin_ProcessChannelData(uint8_t Dd_valChannel)
{
	uint8_t i = 0;
	uint8_t Dd_valSet_Lo = 0;
    uint8_t Dd_valSendData_Lo[12] = {0};
    uint16_t Dd_valBaseAddr_Lo = 0;
	uint16_t Dd_valValue_Lo = 0;
    struct dwin_value* Dd_valChannelData_Lo = NULL;
    uint16_t* Dd_valPointers_Lo[5] = {NULL, NULL, NULL, NULL, NULL};

	if(Dd_valChannel >= 10)
		return;
	
	/* 如果正在进行参数配置/不要发送数据 */
	if(Dd_valDevPtr_Mp->ctrl.config_ip)
		return;

	Dd_valBaseAddr_Lo = Dc_valChannelDataAddr_Mp[Dd_valChannel];
	
	/* hold的优先级被设置为最高 */
	if(1 == Dd_valDevPtr_Mp->ctrl.hold)
	{
		Dd_valSet_Lo = 1;
		Dd_valChannelData_Lo = Dwin_GetLastChannelData(Dd_valChannel);
		if (NULL == Dd_valChannelData_Lo)
			return;
	}
	else if(1 == Dd_valDevPtr_Mp->ctrl.average)
	{
		Dd_valSet_Lo = 1;
		Dd_valChannelData_Lo = Dwin_GetChannelData(Dd_valChannel);
#if DWIN_TEST
#else
		memcpy((void*)Dwin_GetLastChannelData(Dd_valChannel),\
				(void*)Dd_valChannelData_Lo,\
				sizeof(struct dwin_value));
#endif
		if (NULL == Dd_valChannelData_Lo)
			return;
	}
	
	if(Dd_valSet_Lo)
	{
		Dd_valPointers_Lo[0] = &Dd_valChannelData_Lo->current;
		Dd_valPointers_Lo[1] = &Dd_valChannelData_Lo->max;
		Dd_valPointers_Lo[2] = &Dd_valChannelData_Lo->min;
		Dd_valPointers_Lo[3] = &Dd_valChannelData_Lo->average;
		Dd_valPointers_Lo[4] = &Dd_valChannelData_Lo->wave;
	}
	else
	{
		for(i = 0;i < 5;i++)
		{
			Dd_valPointers_Lo[i] = &Dd_valZero_Mp;
		}
	}
	
	for(i = 0;i < 5;i++)
	{
		Dd_valValue_Lo = *Dd_valPointers_Lo[i];
		/* 如果不是两位小数需要除以10 */
		if(!Dd_valDevPtr_Mp->ctrl.fractional_status)
			Dd_valValue_Lo = (*Dd_valPointers_Lo[i] / 10);
		Dd_valSendData_Lo[2 + 2 * i] = (Dd_valValue_Lo >> 8) & 0xFF;
		Dd_valSendData_Lo[3 + 2 * i] = (Dd_valValue_Lo >> 0) & 0xFF;
	}
	Dd_valSendData_Lo[0] = (Dd_valBaseAddr_Lo >> 8) & 0xFF;
	Dd_valSendData_Lo[1] = Dd_valBaseAddr_Lo & 0xFF;
	
	Dwin_SendData(Dd_valSendData_Lo, 12);
}

/****************************** 实现调度函数 ******************************/
void Dwin_InitFunction(void)
{
	RingBuffer_Init(&Dd_valRingBuffer_Mp);
}

void Dwin_MainFunction(void)
{
	static uint8_t i = 0;

	Dwin_ProcessTouchData();
	switch(Dd_valDevPtr_Mp->step)
	{
	case DWIN_STEP_PROCESS:
		if(1 == Dd_valDevPtr_Mp->ctrl.fractional)
		{
			Dd_valDevPtr_Mp->ctrl.fractional = 0;
			Dd_valDevPtr_Mp->ctrl.fractional_status = 1 - Dd_valDevPtr_Mp->ctrl.fractional_status;
			Dd_valDevPtr_Mp->step = DWIN_STEP_SWITCH_FRAC;
		}
		Dd_valDevPtr_Mp->value_channel = Dd_valDevPtr_Mp->value_channel % 10;
		Dwin_ProcessChannelData(Dd_valDevPtr_Mp->value_channel);
		Dd_valDevPtr_Mp->value_channel++;
		break;
	case DWIN_STEP_SWITCH_FRAC:
		Dwin_SwitchFrac(0x8006 + 22 * i, Dd_valDevPtr_Mp->ctrl.fractional_status);
		i++;
		if(50 == i)
		{
			i = 0;
			Dd_valDevPtr_Mp->step = DWIN_STEP_PROCESS;
		}
		break;
	default:
		break;
	}
}
