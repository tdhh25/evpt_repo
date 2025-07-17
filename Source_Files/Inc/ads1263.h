
#ifndef ADS1263_H_
#define ADS1263_H_

#include <assert.h>
#include <stdint.h>


//*****************************************************************************
//
// SPI Opcodes
//
//*****************************************************************************

    #define OPCODE_NOP                          ((uint8_t) 0x00)
    #define OPCODE_RESET                        ((uint8_t) 0x06)
    #define OPCODE_START1                       ((uint8_t) 0x08)
    #define OPCODE_STOP1                        ((uint8_t) 0x0B)
    #define OPCODE_RDATA1                       ((uint8_t) 0x12)
    #define OPCODE_SYOCAL1                      ((uint8_t) 0x16)
    #define OPCODE_SYGCAL1                      ((uint8_t) 0x17)
    #define OPCODE_SFOCAL1                      ((uint8_t) 0x19)
    #define OPCODE_RREG                         ((uint8_t) 0x20)
    #define OPCODE_WREG                         ((uint8_t) 0x40)

		#define OPCODE_START2                      ((uint8_t) 0x0C)
		#define OPCODE_STOP2                       ((uint8_t) 0x0E)
		#define OPCODE_RDATA2                      ((uint8_t) 0x14)
		#define OPCODE_SYOCAL2                     ((uint8_t) 0x1B)
		#define OPCODE_SYGCAL2                     ((uint8_t) 0x1C)
		#define OPCODE_SFOCAL2                     ((uint8_t) 0x1E)


    #define REG_ADDR_ID                         ((uint8_t) 0x00)

    #define REG_ADDR_POWER                      ((uint8_t) 0x01)

    #define REG_ADDR_INTERFACE                  ((uint8_t) 0x02)

    #define REG_ADDR_MODE0                      ((uint8_t) 0x03)

    #define REG_ADDR_MODE1                      ((uint8_t) 0x04)

    #define REG_ADDR_MODE2                      ((uint8_t) 0x05)

    #define REG_ADDR_INPMUX                     ((uint8_t) 0x06)

    #define REG_ADDR_OFCAL0                     ((uint8_t) 0x07)

    #define REG_ADDR_OFCAL1                     ((uint8_t) 0x08)

    #define REG_ADDR_OFCAL2                     ((uint8_t) 0x09)

    #define REG_ADDR_FSCAL0                     ((uint8_t) 0x0A)

    #define REG_ADDR_FSCAL1                     ((uint8_t) 0x0B)

    #define REG_ADDR_FSCAL2                     ((uint8_t) 0x0C)

    #define REG_ADDR_IDACMUX                    ((uint8_t) 0x0D)

    #define REG_ADDR_IDACMAG                    ((uint8_t) 0x0E)

    #define REG_ADDR_REFMUX                     ((uint8_t) 0x0F)

    #define REG_ADDR_TDACP                      ((uint8_t) 0x10)

    #define REG_ADDR_TDACN                      ((uint8_t) 0x11)

    #define REG_ADDR_GPIOCON                    ((uint8_t) 0x12)

    #define REG_ADDR_GPIODIR                    ((uint8_t) 0x13)

    #define REG_ADDR_GPIODAT                    ((uint8_t) 0x14)

    #define REG_ADDR_ADC2CFG                    ((uint8_t) 0x15)

    #define REG_ADDR_ADC2MUX                    ((uint8_t) 0x16)


    #define REG_ADDR_ADC2OFC0                   ((uint8_t) 0x17)

    #define REG_ADDR_ADC2OFC1                   ((uint8_t) 0x18)

    #define REG_ADDR_ADC2FSC0                   ((uint8_t) 0x19)

    #define REG_ADDR_ADC2FSC1                   ((uint8_t) 0x1A)

    #define NUM_REGISTERS                       ((uint8_t) 0x1B)



#define NULL_PTR                                ((void*)0)
#define ADS_1_CHIP_INDEX                        0
#define ADS_2_CHIP_INDEX                        1
#define ADS_CHIP_INDEX_MAX                      2


#define ADS_RETURN_FAILED                       0
#define ADS_RETURN_SUCCESSFUL                   1


#define ADS_REGISTER_WRITE                      1
#define ADS_REGISTER_READ                       0



#define ADS_STEP_INIT                            0
#define ADS_STEP_CONVERT                         1


#define ADS_MUX_AIN0                            0
#define ADS_MUX_AIN1                            1
#define ADS_MUX_AIN2                            2
#define ADS_MUX_AIN3                            3
#define ADS_MUX_AIN4                            4
#define ADS_MUX_AIN5                            5
#define ADS_MUX_AIN6                            6
#define ADS_MUX_AIN7                            7
#define ADS_MUX_AIN8                            8
#define ADS_MUX_AIN9                            9
#define ADS_MUX_AINCOM                          10
#define ADS_MUX_TempSensor                      11
#define ADS_MUX_AnalogSupplyMon                 12
#define ADS_MUX_DigitalSupplyMon                13


#define ADS1262_RESETPIN_SETLOW                 GPIOC,GPIO_PIN_0,GPIO_PIN_RESET
#define ADS1262_RESETPIN_SETHIGH                GPIOC,GPIO_PIN_0,GPIO_PIN_SET
#define ADS1262_STARTPIN_DISABLE                GPIOC,GPIO_PIN_2,GPIO_PIN_SET
#define ADS1262_CS1PIN_HIGH 		                GPIOC,GPIO_PIN_3,GPIO_PIN_SET
#define ADS1262_CS1PIN_LOW  		                GPIOC,GPIO_PIN_3,GPIO_PIN_RESET
#define ADS1262_CS2PIN_HIGH  		                GPIOC,GPIO_PIN_4,GPIO_PIN_SET
#define ADS1262_CS2PIN_LOW  		                GPIOC,GPIO_PIN_4,GPIO_PIN_RESET



typedef union  
{
	uint8_t Reg;
	struct 
	{
		uint8_t Reserved7_5    :3;
		uint8_t Reset          :1;
		uint8_t Reserved3_2    :2;
		uint8_t Vbias          :1;
		uint8_t IntRef         :1;
	}Bit;

}Ads_POWER_01Reg;

typedef union  
{
	uint8_t Reg;
	struct 
	{
		uint8_t Reserved7_4    :4;
		uint8_t Timeout        :1;
		uint8_t Status         :1;
		uint8_t CRC_En         :2;
	}Bit;

}Ads_INTERFACE_02Reg;

typedef union  
{
	uint8_t Reg;
	struct 
	{
		uint8_t RefRev        :1;
		uint8_t RunMode       :1;
		uint8_t CHOP          :2;
		uint8_t DELAY         :4;
	}Bit;

}Ads_MODE0_03Reg;

typedef union  
{
	uint8_t Reg;
	struct 
	{
		uint8_t FILter        :3;
		uint8_t SBADC         :1;
		uint8_t SBPOL         :1;
		uint8_t SBMAG         :3;
	}Bit;

}Ads_MODE1_04Reg;

typedef union  
{
	uint8_t Reg;
	struct 
	{
		uint8_t Bypass        :1;
		uint8_t Gain          :3;
		uint8_t DR            :4;
	}Bit;

}Ads_MODE2_05Reg;

typedef union  
{
	uint8_t Reg;
	struct 
	{
		uint8_t MUXP         :4;
		uint8_t MUXN         :4;
	}Bit;

}Ads_INPMUX_06Reg;


typedef union  
{
	uint8_t Reg;
	struct 
	{
		uint8_t MUX2         :4;
		uint8_t MUX1         :4;
	}Bit;

}Ads_IDACMUX_0DReg;

typedef union  
{
	uint8_t Reg;
	struct 
	{
		uint8_t MUX2         :4;
		uint8_t MUX1         :4;
	}Bit;

}Ads_IDACMAG_0EReg;

typedef union  
{
	uint8_t Reg;
	struct 
	{
		uint8_t Reserved7_6         :2;
		uint8_t RMUXP               :3;
		uint8_t RMUXN               :3;
	}Bit;

}Ads_REFMUX_0FReg;

typedef union  
{
	uint8_t Reg;
	struct 
	{
		uint8_t OutP                :1;
		uint8_t Reserved6_5         :2;
		uint8_t MagP                :5;
	}Bit;

}Ads_REFMUX_10Reg;

typedef union  
{
	uint8_t Reg;
	struct 
	{
		uint8_t OutN                :1;
		uint8_t Reserved6_5         :2;
		uint8_t MagN                :5;
	}Bit;

}Ads_REFMUX_11Reg;

typedef union  
{
	uint8_t Reg;
	struct 
	{
		uint8_t DR2                 :2;
		uint8_t Ref2                :3;
		uint8_t Gain2               :3;
	}Bit;

}Ads_ADC2CFG_15Reg;

typedef union  
{
	uint8_t Reg;
	struct 
	{
		uint8_t MUXP               :4;
		uint8_t MUXN               :4;
	}Bit;

}Ads_ADC2MUX_16Reg;




typedef struct 
{
    uint8_t Adc_P_Channel;
    uint8_t Adc_N_Channel;

}Ads_GroupCfg_STDR;


typedef enum
{
    ADS_ConversionMode_EM,
    ADS_PulseMode_EM,
}ADS_RunModeCfg_TDEM;




typedef struct 
{
    uint8_t  valDatavaild;
    uint8_t valStatus;
	uint8_t valCRC_CHM;
    int32_t valRawResult;
	int32_t valVoltage;
}Abs_Result_STDR;




typedef uint8_t    Ads_OFCAL0_07Reg;
typedef uint8_t    Ads_OFCAL1_08Reg;
typedef uint8_t    Ads_OFCAL2_09Reg;
typedef uint8_t    Ads_FSC0_0AReg;
typedef uint8_t    Ads_FSC1_0BReg;
typedef uint8_t    Ads_FSC2_0CReg;
typedef uint8_t    Ads_GPIOCON_12Reg;
typedef uint8_t    Ads_GPIODIR_13Reg;
typedef uint8_t    Ads_GPIODAT_14Reg;
typedef uint8_t    Ads_ADC2OFC0_17Reg;
typedef uint8_t    Ads_ADC2OFC1_18Reg;
typedef uint8_t    Ads_ADC2FSC0_19Reg;
typedef uint8_t    Ads_ADC2FSC1_1AReg;
typedef uint32_t   Ads_ReturnType;

extern void Ads_1msMain_Function(void);
extern uint8_t calculateCRC(const uint8_t dataBytes[], uint8_t numBytes);
extern uint8_t calculateChecksum(const uint8_t dataBytes[], uint8_t numBytes);
#endif /* ADS1263_H_ */
