#include "ads1263.h"
#include "ads1262_Interface.h"
#include "main.h"


extern SPI_HandleTypeDef hspi1;

uint8_t Ads_numMainStep_Mp = ADS_STEP_INIT;
volatile Abs_Result_STDR Ads_valResults_Mp[ADS_NUM_CHANNEL_MAX] = {{0}};
static uint8_t Ads_AllRegistrs[NUM_REGISTERS] = 
{
	0x00, 0x11, 0x04, 0x00, 0x80, 0x04,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x40, 0xBB, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
	0x00, 0x40 
};
const Ads_GroupCfg_STDR Ads_valGroupAllocate_Mp[ADS_NUM_CHANNEL_MAX] = 
{
    {ADS_MUX_AIN0,ADS_MUX_AIN1},
    {ADS_MUX_AIN2,ADS_MUX_AIN3},
    {ADS_MUX_AIN4,ADS_MUX_AIN5},
    {ADS_MUX_AIN6,ADS_MUX_AIN7},
    {ADS_MUX_AIN8,ADS_MUX_AIN9},
};
uint16_t TestNumber_Mp = 3;

void Ads_SpiCommunication_Function(uint8_t valChipId,uint8_t *valTxBuf,uint8_t *valRxBuf,uint8_t valLength)
{
				if(valChipId)
				{
					HAL_GPIO_WritePin(ADS1262_CS2PIN_LOW);
				}
				else
				{
					HAL_GPIO_WritePin(ADS1262_CS1PIN_LOW);
				}
		
        HAL_SPI_TransmitReceive(&hspi1,valTxBuf,valRxBuf,valLength,TestNumber_Mp);
				if(valChipId)
				{
					HAL_GPIO_WritePin(ADS1262_CS2PIN_HIGH);
				}
				else
				{
					HAL_GPIO_WritePin(ADS1262_CS1PIN_HIGH);
				}

}
static Ads_ReturnType Ads_WriteRegister_Function(uint8_t ChipId,uint8_t address,uint8_t *regData,uint8_t length)
{
		uint8_t* valTxBuf_Lo;
		uint8_t* valRxBuf_Lo;
    if((length > NUM_REGISTERS) || ((regData == NULL_PTR)))
		{
			return ADS_RETURN_FAILED;
		
		}
		else
		{
				valTxBuf_Lo[0] = 0x40|(address & 0x1F);

        
        valTxBuf_Lo[1] = length - 1;
        uint8_t valDataLegth_Lo = length + 2;
        for(uint8_t numTxCnt = 2;numTxCnt < valDataLegth_Lo; numTxCnt++)
        {
            valTxBuf_Lo[numTxCnt] = regData[numTxCnt-2];
            
        }
				Ads_SpiCommunication_Function(ChipId,valTxBuf_Lo,valRxBuf_Lo,valDataLegth_Lo);
				return ADS_RETURN_SUCCESSFUL;	
		}
}
static Ads_ReturnType Ads_ReadRegister_Function(uint8_t ChipId,uint8_t address,uint8_t *RxRegData,uint8_t length)
{
		uint8_t* valTxBuf_Lo;
		uint8_t* valRxBuf_Lo;
    if((length > NUM_REGISTERS) || ((RxRegData == NULL_PTR)))
		{
			return ADS_RETURN_FAILED;
		
		}
		else
		{
				valTxBuf_Lo[0] = 0x20|(address & 0x1F);

        
        valTxBuf_Lo[1] = length - 1;
        uint8_t valDataLegth_Lo = length + 2;
        for(uint8_t numTxCnt = 2;numTxCnt < valDataLegth_Lo; numTxCnt++)
        {
            valTxBuf_Lo[numTxCnt] = 0;
            
        }
				Ads_SpiCommunication_Function(ChipId,valTxBuf_Lo,valRxBuf_Lo,valDataLegth_Lo);
				for(uint8_t numTxCnt = 0;numTxCnt < length; numTxCnt++)
        {
            RxRegData[numTxCnt] = valRxBuf_Lo[2+numTxCnt];
            
        }	
				return ADS_RETURN_SUCCESSFUL;					
						
		}
}


static void Ads_RunModeConfig_Function(uint8_t ChipId,ADS_RunModeCfg_TDEM valMode)
{
	  uint8_t valRxBuf_Lo = 0;
    uint8_t valTxBuf_Lo = valMode << 6;
		Ads_WriteRegister_Function(ChipId,REG_ADDR_MODE0,&valTxBuf_Lo,1);
}
uint8_t sdfg = 0;
static Ads_ReturnType Ads_ConvertChannelConfig_Function(uint8_t valChipId,const Ads_GroupCfg_STDR *valExtCh_Lo)
{
    uint8_t valTxBuf_Lo = 0;
    uint8_t valRxBuf_Lo = 0;
    Ads_ReturnType valret_Lo = ADS_RETURN_FAILED;
    Ads_GroupCfg_STDR valRegData_Lo = *valExtCh_Lo;
    if(valExtCh_Lo != NULL_PTR)
    {
        valTxBuf_Lo   = valRegData_Lo.Adc_P_Channel << 4;
        valTxBuf_Lo  |= valRegData_Lo.Adc_N_Channel;
			sdfg = valTxBuf_Lo;
				valret_Lo = Ads_WriteRegister_Function(valChipId,REG_ADDR_INPMUX,&valTxBuf_Lo,1);
    }
    else
    {
        valret_Lo =  ADS_RETURN_FAILED;
    }
    return valret_Lo;


}
static void Ads_CommandTransmit_Function(uint8_t valChipId,uint8_t valOpcode)
{
	uint8_t valRxBuf_Lo = 0;
	uint8_t valTxBuf_Lo = valOpcode ;
	Ads_SpiCommunication_Function(valChipId,&valTxBuf_Lo,&valRxBuf_Lo,1);


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
			for(uint8_t numChip = 0; numChip < ADS_CHIP_INDEX_MAX;numChip++)
			{
				Ads_WriteRegister_Function(numChip,REG_ADDR_ID,Ads_AllRegistrs,NUM_REGISTERS);
				Ads_CommandTransmit_Function(numChip,OPCODE_START1);
			}
		}
		return valRet_Lo;
}

static Abs_Result_STDR Abs_ReadData_Function(uint8_t ChipId)       //status and CRC enable
{
    uint8_t valDataTxBuf_Lo[7] = { 0 };
    uint8_t valDataRxBuf_Lo[7] = { 0 }; 
    Abs_Result_STDR valRetResult_Lo = {0};

    valDataTxBuf_Lo[0] = OPCODE_RDATA1;
		Ads_SpiCommunication_Function(ChipId,valDataTxBuf_Lo,valDataRxBuf_Lo,7);	


    valRetResult_Lo.valStatus = valDataRxBuf_Lo[1];
     
    if(valRetResult_Lo.valStatus & 0x40) 
    {
        valRetResult_Lo. valDatavaild = 1;
        valRetResult_Lo.valRawResult  = valDataRxBuf_Lo[2] << 24;
        valRetResult_Lo.valRawResult |= valDataRxBuf_Lo[3] << 16;
        valRetResult_Lo.valRawResult |= valDataRxBuf_Lo[4] << 8;
        valRetResult_Lo.valRawResult |= valDataRxBuf_Lo[5] << 0;
		valRetResult_Lo.valVoltage = valRetResult_Lo.valRawResult*(2500.0/0x80000000);
		valRetResult_Lo.valCRC_CHM = valDataRxBuf_Lo[6];
    }
    else
    {
        valRetResult_Lo. valDatavaild = 0;
        valRetResult_Lo.valRawResult  = 0;
    }

    return valRetResult_Lo;
}
uint8_t numErrorCnt = 0; 
static void Ads_Convert_Function(void)
{
    static uint8_t valChannel_Lo = ADS_NUM_1_CHANNEL;
		(void)Ads_ConvertChannelConfig_Function(ADS_1_CHIP_INDEX,&Ads_valGroupAllocate_Mp[valChannel_Lo]);
		if(Abs_ReadData_Function(ADS_1_CHIP_INDEX).valDatavaild)
		{
			Ads_valResults_Mp[valChannel_Lo] = Abs_ReadData_Function(ADS_1_CHIP_INDEX);
		
		}
		else
		{
			numErrorCnt++;
		
		}

		
	
//		(void)Ads_ConvertChannelConfig_Function(ADS_2_CHIP_INDEX,&Ads_valGroupAllocate_Mp[valChannel_Lo]);
//		Ads_valResults_Mp[valChannel_Lo+5] = Abs_ReadData_Function(ADS_2_CHIP_INDEX);
//		if(valChannel_Lo < 4)
//		{
//			valChannel_Lo++;
//			
//		}
//		else
//		{
//			valChannel_Lo = 0;
//		
//		}

}
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
			Ads_Convert_Function();
			break;
		default:
		break;
	
	}
    

}
Ads_ReturnType Ads_GetAdResult_Function(uint8_t valChannel)
{
    return Ads_valResults_Mp[valChannel].valRawResult;
}
uint8_t calculateCRC(const uint8_t dataBytes[], uint8_t numBytes)
{
    /* Check that "numBytes" is between 1 and 4 */
    assert((numBytes >= 3) && (numBytes <= 4));

    /* Check that "dataBytes" is not a null pointer */
    assert(dataBytes != NULL_PTR);

    uint_fast8_t i;
    uint_fast8_t crc        = 0xFFu;        /* Initial value of crc register     */
    uint_fast8_t crcMSb;                    /* Most significant bit of crc byte  */
    const uint_fast8_t poly = 0x07u;        /* CRC polynomial byte               */
    uint_fast8_t shift_by   = 0u;           /* Intermediate variable             */
    uint32_t data           = 0u;           /* Data storage variable             */
    uint32_t msbMask        = 0x80000000u;  /* Points to the next data bit       */
    uint32_t dataMSb;                       /* Most significant bit of data int  */

    /* Construct data word from data bytes */
    for (i = 0; i < numBytes; i++)
    {
        shift_by = 8 * (numBytes - i - 1);
        data |= (((uint32_t) dataBytes[i]) << shift_by);
    }

    /* Determine the location of the first data byte */
    shift_by = 8 * (4 - numBytes);
    msbMask >>= shift_by;

    /* CRC algorithm */
    while (msbMask > 0)
    {
        // Check MSB's of data and crc
        dataMSb = data & msbMask;
        crcMSb  = crc & 0x80u;

        // Shift crc byte
        crc <<= 1;

        // Check if XOR operation of MSbs results in additional XOR operation
        if (dataMSb ^ crcMSb)   { crc ^= poly; }

        /* Shift MSb pointer */
        msbMask >>= 1;
    }

    return crc;
}


uint8_t calculateChecksum(const uint8_t dataBytes[], uint8_t numBytes)
{
    /* Check that "numBytes" is between 3 and 4 */
    assert((numBytes >= 3) && (numBytes <= 4));

    /* Check that "dataBytes" is not a null pointer */
    assert(dataBytes != NULL_PTR);

    /* Checksum initial value */
    uint_fast8_t checksum   = 0x9Bu;

    /* Checksum calculation */
    uint8_t i;
    for (i = 0; i < numBytes; i++)
    {
        checksum = (checksum + dataBytes[i]) & 0xFFu;
    }

    return checksum;
}

