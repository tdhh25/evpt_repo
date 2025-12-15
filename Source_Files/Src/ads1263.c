#include "ads1263.h"
#include "ads1262_Interface.h"
#include "main.h"


extern SPI_HandleTypeDef hspi1;
static SlideWindow Ads_TempDataBuf[ADS_NUM_CHANNEL_MAX] = {0};
uint8_t Ads_numMainStep_Mp = ADS_STEP_INIT;
volatile Abs_Result_STDR Ads_valResults_Mp[ADS_NUM_CHANNEL_MAX] = {{0}};
static uint8_t Ads_AllRegistrs[NUM_REGISTERS] =
{
  0x00, 
	0x11, 
	0x04, 
	0x40, 
	0x80, 
	0x54,/*****Gain 32****/
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
uint16_t TestNumber_Mp = 3;

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
    uint8_t* valTxBuf_Lo;
    uint8_t* valRxBuf_Lo;

    if((length > NUM_REGISTERS) || ((regData == NULL_PTR)))
    {
        return ADS_RETURN_FAILED;

    }
    else
    {
        valTxBuf_Lo[0] = 0x40 | (address & 0x1F);


        valTxBuf_Lo[1] = length - 1;
        uint8_t valDataLegth_Lo = length + 2;

        for(uint8_t numTxCnt = 2; numTxCnt < valDataLegth_Lo; numTxCnt++)
        {
            valTxBuf_Lo[numTxCnt] = regData[numTxCnt - 2];

        }

        Ads_SpiCommunication_Function(ChipId, valTxBuf_Lo, valRxBuf_Lo, valDataLegth_Lo);
        return ADS_RETURN_SUCCESSFUL;
    }
}
static Ads_ReturnType Ads_ReadRegister_Function(uint8_t ChipId, uint8_t address, uint8_t *RxRegData, uint8_t length)
{
    uint8_t* valTxBuf_Lo;
    uint8_t* valRxBuf_Lo;

    if((length > NUM_REGISTERS) || ((RxRegData == NULL_PTR)))
    {
        return ADS_RETURN_FAILED;

    }
    else
    {
        valTxBuf_Lo[0] = 0x20 | (address & 0x1F);


        valTxBuf_Lo[1] = length - 1;
        uint8_t valDataLegth_Lo = length + 2;

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
static Ads_ReturnType Ads_Init_Function(void)
{
    Ads_ReturnType valRet_Lo = ADS_RETURN_FAILED;
    static uint8_t numCnt = 0;

    if(numCnt < 100)
    {
        numCnt++;
        HAL_GPIO_WritePin(ADS1262_RESETPIN_SETLOW);
        valRet_Lo = ADS_RETURN_FAILED;

    }
    else
    {

        numCnt = 0;
        valRet_Lo = ADS_RETURN_SUCCESSFUL;
        HAL_GPIO_WritePin(ADS1262_RESETPIN_SETHIGH);

        for(uint8_t numChip = 0; numChip < ADS_CHIP_INDEX_MAX; numChip++)
        {
            Ads_WriteRegister_Function(numChip, REG_ADDR_ID, Ads_AllRegistrs, NUM_REGISTERS);
        }
				for(uint8_t chIndex = 0; chIndex < ADS_NUM_CHANNEL_MAX; chIndex++)
				{
					if (slide_window_init(&Ads_TempDataBuf[chIndex], WINDOW_SIZE) != 0) 
					{
							return ADS_RETURN_FAILED;
					}	
				
				}

    }

    return valRet_Lo;
}
Abs_Result_STDR Ads_valResult_Mp[ADS_NUM_CHANNEL_MAX] = {{0}};
float Ads_valResistance_Mp[ADS_NUM_CHANNEL_MAX] = {0.0};
float Ads_valTemperature_Mp[ADS_NUM_CHANNEL_MAX] = {0.0};
uint8_t numErrorCnt = 0;
uint8_t numRightCnt = 0;
uint32_t TestAllVolt_Mp[10] = {0};



static float Ads_ResistanceProcess_Function(uint8_t valChannel_Lo,float RawVolt)
{

	float valRetRes_Lo = 0.0;
	const float ADS_PullUpVoltage =       3300.0f;
	const uint32_t ADS_PullDownResistance =  100000000;
	const uint32_t ADS_PullUpResistance =    100000000;
	float valCurrent = 0.0;
	if(valChannel_Lo >= ADS_NUM_CHANNEL_MAX)
	{
		return -1.0;
	
	}
	valCurrent = (ADS_PullUpVoltage -RawVolt)/(ADS_PullDownResistance+ADS_PullUpResistance);
	valRetRes_Lo = RawVolt/valCurrent;
}
		float valVolt_Lo = 0.0;
static float Ads_GetTemperatureByVolt_Function(uint8_t valChannel_Lo)
{

	float valResistance = 0.0;
		uint32_t volt_Lo = 0;
		uint8_t valDataTxBuf_Lo[7] = { 0 };
		uint8_t valDataRxBuf_Lo[7] = { 0 };
		valDataTxBuf_Lo[0] = OPCODE_RDATA1;
		uint8_t ChipId = (valChannel_Lo < ADS_NUM_6_CHANNEL) ? ADS_1_CHIP_INDEX : ADS_2_CHIP_INDEX;
		Ads_SpiCommunication_Function((valChannel_Lo < ADS_NUM_6_CHANNEL) ? ADS_1_CHIP_INDEX : ADS_2_CHIP_INDEX, valDataTxBuf_Lo, valDataRxBuf_Lo, 7);

		if(valDataRxBuf_Lo[1] & 0x40)
		{
				Ads_CommandTransmit_Function(ChipId, OPCODE_STOP1);
				volt_Lo  = valDataRxBuf_Lo[2] << 24;
				volt_Lo |= valDataRxBuf_Lo[3] << 16;
				volt_Lo |= valDataRxBuf_Lo[4] << 8;
				volt_Lo |= valDataRxBuf_Lo[5] << 0;
				valVolt_Lo = (float)volt_Lo * (2500.0/32/ 0x80000000);
			  valResistance = Ads_ResistanceProcess_Function(valChannel_Lo,valVolt_Lo);
				return (valResistance > 0)?PT100_CalibrationTemperature_Function(valResistance):-1.0;
		}
		else
		{
			return -1.0;
		
		}

}
static void Ads_Convert_Function(uint8_t valChannel_Lo)
{

    uint8_t ChipId = (valChannel_Lo < ADS_NUM_6_CHANNEL) ? ADS_1_CHIP_INDEX : ADS_2_CHIP_INDEX;
    Ads_CommandTransmit_Function(ChipId, OPCODE_START1);
    (void)Ads_ConvertChannelConfig_Function(ChipId, &Ads_valGroupAllocate_Mp[valChannel_Lo]);
}
uint8_t Test_ch_C = 0;
void Ads_1msMain_Function(void)
{

    

    switch(Ads_numMainStep_Mp)
    {
        case ADS_STEP_INIT:
            if(ADS_RETURN_SUCCESSFUL == Ads_Init_Function())
            {
                Ads_numMainStep_Mp = ADS_STEP_CONVERT;
            }

            break;

        case ADS_STEP_CONVERT:
            Ads_Convert_Function(Test_ch_C);
            Ads_numMainStep_Mp = 111;

            break;

        case 111:

						Ads_valTemperature_Mp[Test_ch_C]= Ads_GetTemperatureByVolt_Function(Test_ch_C);
						slide_window_add_data(&Ads_TempDataBuf[Test_ch_C], Ads_valTemperature_Mp[Test_ch_C]);
						slide_window_calc_stats(&Ads_TempDataBuf[Test_ch_C],\
																		&Dc_valChannelData_Mp[Test_ch_C].max,\
																		&Dc_valChannelData_Mp[Test_ch_C].min,\
																		&Dc_valChannelData_Mp[Test_ch_C].average);
						if(Test_ch_C < 10)
						{
								Test_ch_C++;

						}
						else
						{
								Test_ch_C = 0;
						}
            Ads_numMainStep_Mp = ADS_STEP_CONVERT;
            break;

        default:
            break;

    }


}
Ads_ReturnType Ads_GetAdResult_Function(uint8_t valChannel)
{
//    return Ads_valResults_Mp[valChannel].valRawResult;
}


