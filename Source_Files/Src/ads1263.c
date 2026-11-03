#include "ads1263.h"
#include "ads1262_Interface.h"
#include "main.h"


/****************************** 外部引用 ******************************/
extern SPI_HandleTypeDef hspi1;

/****************************** 模块全局变量 ******************************/
uint8_t Ads_numMainStep_Mp = ADS_STEP_INIT;
static uint8_t Ads_numChannel_Mp = 0U;
static SlideWindow Ads_TempDataBuf[ADS_NUM_CHANNEL_MAX] = {0};
volatile Abs_Result_STDR Ads_valResults_Mp[ADS_NUM_CHANNEL_MAX] = {{0}};
Abs_Result_STDR Ads_valResult_Mp[ADS_NUM_CHANNEL_MAX] = {{0}};
float Ads_valResistance_Mp[ADS_NUM_CHANNEL_MAX] = {0.0};
float Ads_valTemperature_Mp[ADS_NUM_CHANNEL_MAX] = {0.0};
uint16_t TestNumber_Mp = 3;
uint8_t numErrorCnt = 0;
uint8_t numRightCnt = 0;
uint32_t TestAllVolt_Mp[10] = {0};
double valVolt_Lo = 0.0;
double valResistance = 0.0;
double Tempaurature = 0.0;
/****************************** 常量配置表 ******************************/
/* ADS1263 全部寄存器初值 */
static uint8_t Ads_AllRegistrs[NUM_REGISTERS] =
{
  0x00, 
	0x11, 
	0x04, 
	0x40, 
	0x80, 
	0x48,/*****MODE2: PGA enable, GAIN=16 V/V, DR=400 SPS****/
	0x01, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00,
	0x40, 
	0xBB, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x01, 
	0x00, 
	0x00,
	0x00, 
	0x40
};

/* 通道正/负输入分配表 */
const Ads_GroupCfg_STDR Ads_valGroupAllocate_Mp[ADS_NUM_CHANNEL_MAX] =
{
    {ADS_MUX_AIN0, ADS_MUX_AIN1},
    {ADS_MUX_AIN2, ADS_MUX_AIN3},
    {ADS_MUX_AIN4, ADS_MUX_AIN5},
    {ADS_MUX_AIN6, ADS_MUX_AIN7},
    {ADS_MUX_AIN8, ADS_MUX_AIN9},
    {ADS_MUX_AIN0, ADS_MUX_AIN1},
    {ADS_MUX_AIN2, ADS_MUX_AIN3},
    {ADS_MUX_AIN4, ADS_MUX_AIN5},
    {ADS_MUX_AIN6, ADS_MUX_AIN7},
    {ADS_MUX_AIN8, ADS_MUX_AIN9},
};

void Ads_SpiCommunication_Function(uint8_t valChipId, uint8_t *valTxBuf, uint8_t *valRxBuf, uint8_t valLength)
{
    if(valChipId)
    {
        HAL_GPIO_WritePin(ADS1262_CS2PIN_LOW);
    }
    else
    {
        HAL_GPIO_WritePin(ADS1262_CS1PIN_LOW);
    }

    HAL_SPI_TransmitReceive(&hspi1, valTxBuf, valRxBuf, valLength, TestNumber_Mp);

    if(valChipId)
    {
        HAL_GPIO_WritePin(ADS1262_CS2PIN_HIGH);
    }
    else
    {
        HAL_GPIO_WritePin(ADS1262_CS1PIN_HIGH);
    }

}
static Ads_ReturnType Ads_WriteRegister_Function(uint8_t ChipId, uint8_t address, uint8_t *regData, uint8_t length)
{
    uint8_t valTxBuf_Lo[NUM_REGISTERS + 2] = { 0 };
    uint8_t valRxBuf_Lo[NUM_REGISTERS + 2] = { 0 };
    uint8_t valDataLegth_Lo = 0;

    if((length > NUM_REGISTERS) || (regData == NULL_PTR))
    {
        return ADS_RETURN_FAILED;
    }

    valTxBuf_Lo[0] = 0x40 | (address & 0x1F);
    valTxBuf_Lo[1] = length - 1;
    valDataLegth_Lo = length + 2;

    for(uint8_t numTxCnt = 2; numTxCnt < valDataLegth_Lo; numTxCnt++)
    {
        valTxBuf_Lo[numTxCnt] = regData[numTxCnt - 2];
    }

    Ads_SpiCommunication_Function(ChipId, valTxBuf_Lo, valRxBuf_Lo, valDataLegth_Lo);
    return ADS_RETURN_SUCCESSFUL;
}
static Ads_ReturnType Ads_ReadRegister_Function(uint8_t ChipId, uint8_t address, uint8_t *RxRegData, uint8_t length)
{
    uint8_t valTxBuf_Lo[NUM_REGISTERS + 2] = { 0 };
    uint8_t valRxBuf_Lo[NUM_REGISTERS + 2] = { 0 };
    uint8_t valDataLegth_Lo = 0;

    if((length > NUM_REGISTERS) || (RxRegData == NULL_PTR))
    {
        return ADS_RETURN_FAILED;
    }

    valTxBuf_Lo[0] = 0x20 | (address & 0x1F);
    valTxBuf_Lo[1] = length - 1;
    valDataLegth_Lo = length + 2;

    for(uint8_t numTxCnt = 2; numTxCnt < valDataLegth_Lo; numTxCnt++)
    {
        valTxBuf_Lo[numTxCnt] = 0;
    }

    Ads_SpiCommunication_Function(ChipId, valTxBuf_Lo, valRxBuf_Lo, valDataLegth_Lo);

    for(uint8_t numTxCnt = 0; numTxCnt < length; numTxCnt++)
    {
        RxRegData[numTxCnt] = valRxBuf_Lo[2 + numTxCnt];
    }

    return ADS_RETURN_SUCCESSFUL;
}


static Ads_ReturnType Ads_ConvertChannelConfig_Function(uint8_t valChipId, const Ads_GroupCfg_STDR *valExtCh_Lo)
{
    uint8_t valTxBuf_Lo = 0;
    uint8_t valRxBuf_Lo = 0;
    Ads_ReturnType valret_Lo = ADS_RETURN_FAILED;
    Ads_GroupCfg_STDR valRegData_Lo = *valExtCh_Lo;

    if(valExtCh_Lo != NULL_PTR)
    {
        valTxBuf_Lo   = valRegData_Lo.Adc_P_Channel << 4;
        valTxBuf_Lo  |= valRegData_Lo.Adc_N_Channel;
        valret_Lo = Ads_WriteRegister_Function(valChipId, REG_ADDR_INPMUX, &valTxBuf_Lo, 1);
    }
    else
    {
        valret_Lo =  ADS_RETURN_FAILED;
    }

    return valret_Lo;


}
static void Ads_CommandTransmit_Function(uint8_t valChipId, uint8_t valOpcode)
{
    uint8_t valRxBuf_Lo = 0;
    uint8_t valTxBuf_Lo = valOpcode ;
    Ads_SpiCommunication_Function(valChipId, &valTxBuf_Lo, &valRxBuf_Lo, 1);


}
static uint8_t Ads_ResetStart_Function(void)
{
    static uint8_t numCnt_Lo = 0U;
    uint8_t valRet_Lo = 0U;

    HAL_GPIO_WritePin(ADS1262_RESETPIN_SETLOW);

    numCnt_Lo++;
    if(numCnt_Lo >= ADS_RESET_LOW_TICKS)
    {
        numCnt_Lo = 0U;
        valRet_Lo = 1U;
    }

    return valRet_Lo;
}

static uint8_t Ads_ResetWait_Function(void)
{
    static uint8_t numCnt_Lo = 0U;
    uint8_t valRet_Lo = 0U;

    HAL_GPIO_WritePin(ADS1262_RESETPIN_SETHIGH);

    numCnt_Lo++;
    if(numCnt_Lo >= ADS_RESET_SETTLE_TICKS)
    {
        numCnt_Lo = 0U;
        valRet_Lo = 1U;
    }

    return valRet_Lo;
}

static uint8_t Ads_InitWrite_Function(void)
{
    uint8_t valRet_Lo = 1U;

    for(uint8_t numChip = 0; numChip < ADS_CHIP_INDEX_MAX; numChip++)
    {
        (void)Ads_WriteRegister_Function(numChip, REG_ADDR_ID, Ads_AllRegistrs, NUM_REGISTERS);
    }

    for(uint8_t chIndex = 0; chIndex < ADS_NUM_CHANNEL_MAX; chIndex++)
    {
        if(0 != slide_window_init(&Ads_TempDataBuf[chIndex], WINDOW_SIZE))
        {
            valRet_Lo = 0U;
            break;
        }
    }

    return valRet_Lo;
}

static float Ads_ResistanceProcess_Function(uint8_t valChannel_Lo,float RawVolt)
{

	float valRetRes_Lo = 0.0;
	const float ADS_PullUpVoltage =       3300.0f;
	const uint32_t ADS_PullDownResistance =  4700;
	const uint32_t ADS_PullUpResistance   =  4700;
	float valCurrent = 0.0;
	if(valChannel_Lo >= ADS_NUM_CHANNEL_MAX)
	{
		return -1.0;
	
	}
	valCurrent = (ADS_PullUpVoltage -RawVolt)/(ADS_PullDownResistance+ADS_PullUpResistance);
	valRetRes_Lo = RawVolt/valCurrent;
	return valRetRes_Lo;
}

static double Ads_GetTemperatureByVolt_Function(uint8_t valChannel_Lo)
{	
		int32_t valCode_Lo = 0;
		const double valVrefMv_Lo = 2500.0;
		const double valFullScaleCode_Lo = 2147483648.0;
		const double valAdcGain_Lo = 16.0;
		uint8_t valDataTxBuf_Lo[7] = { 0 };
		uint8_t valDataRxBuf_Lo[7] = { 0 };
		valDataTxBuf_Lo[0] = OPCODE_RDATA1;
		uint8_t ChipId = (valChannel_Lo < ADS_NUM_6_CHANNEL) ? ADS_1_CHIP_INDEX : ADS_2_CHIP_INDEX;
		Ads_SpiCommunication_Function((valChannel_Lo < ADS_NUM_6_CHANNEL) ? ADS_1_CHIP_INDEX : ADS_2_CHIP_INDEX, valDataTxBuf_Lo, valDataRxBuf_Lo, 7);

		if(valDataRxBuf_Lo[1] & 0x40)
		{
            Ads_CommandTransmit_Function(ChipId, OPCODE_STOP1);
            /* 32 位二进制补码：先拼成无符号位型，再转回有符号 */
            valCode_Lo = (int32_t)(((uint32_t)valDataRxBuf_Lo[2] << 24) |
                                   ((uint32_t)valDataRxBuf_Lo[3] << 16) |
                                   ((uint32_t)valDataRxBuf_Lo[4] << 8)  |
                                   ((uint32_t)valDataRxBuf_Lo[5] << 0));
            valVolt_Lo = (double)valCode_Lo * (valVrefMv_Lo / (valAdcGain_Lo * valFullScaleCode_Lo));
            valResistance = Ads_ResistanceProcess_Function(valChannel_Lo,valVolt_Lo);
            Tempaurature =  (valResistance > 0) ? PT100_CalibrationTemperature_Function(valResistance) : PT100_TEMP_INVALID_C100;
			return Tempaurature*100;
		}
		else
		{
			return PT100_TEMP_INVALID_C100;
		}

}
static void Ads_Convert_Function(uint8_t valChannel_Lo)
{

    uint8_t ChipId = (valChannel_Lo < ADS_NUM_6_CHANNEL) ? ADS_1_CHIP_INDEX : ADS_2_CHIP_INDEX;
    (void)Ads_ConvertChannelConfig_Function(ChipId, &Ads_valGroupAllocate_Mp[valChannel_Lo]);
    Ads_CommandTransmit_Function(ChipId, OPCODE_START1);
}

static void Ads_DataProcess_Function(uint8_t valChannel_Lo)
{
    Ads_valTemperature_Mp[valChannel_Lo] = Ads_GetTemperatureByVolt_Function(valChannel_Lo);

    if((Ads_valTemperature_Mp[valChannel_Lo] >= PT100_TEMP_MIN_C100) &&\
       (Ads_valTemperature_Mp[valChannel_Lo] <= PT100_TEMP_MAX_C100))
    {
        (void)slide_window_add_data(&Ads_TempDataBuf[valChannel_Lo], Ads_valTemperature_Mp[valChannel_Lo]);
        (void)slide_window_calc_stats(&Ads_TempDataBuf[valChannel_Lo], &Dc_valChannelData_Mp[valChannel_Lo]);
    }
}

void Ads_10msMain_Function(void)
{
    switch(Ads_numMainStep_Mp)
    {
        case ADS_STEP_INIT:
            if(0U != Ads_ResetStart_Function())
            {
                Ads_numMainStep_Mp = ADS_STEP_INIT_WAIT;
            }
            break;

        case ADS_STEP_INIT_WAIT:
            if(0U != Ads_ResetWait_Function())
            {
                Ads_numMainStep_Mp = ADS_STEP_INIT_WRITE;
            }
            break;

        case ADS_STEP_INIT_WRITE:
            if(0U != Ads_InitWrite_Function())
            {
                Ads_numMainStep_Mp = ADS_STEP_CONVERT;
            }
            else
            {
                Ads_numMainStep_Mp = ADS_STEP_INIT;
            }
            break;

        case ADS_STEP_CONVERT:
            Ads_Convert_Function(Ads_numChannel_Mp);
            Ads_numMainStep_Mp = ADS_STEP_PROCESS;
            break;

        case ADS_STEP_PROCESS:
            Ads_DataProcess_Function(Ads_numChannel_Mp);
            Ads_numChannel_Mp = (uint8_t)((Ads_numChannel_Mp + 1U) % ADS_NUM_CHANNEL_MAX);
            Ads_numMainStep_Mp = ADS_STEP_CONVERT;
            break;

        default:
            Ads_numMainStep_Mp = ADS_STEP_INIT;
            break;

    }


}
Ads_ReturnType Ads_GetAdResult_Function(uint8_t valChannel)
{
    //reserved
}


