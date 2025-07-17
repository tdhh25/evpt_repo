#ifndef _ADS1262_INTERFACE_H_
#define _ADS1262_INTERFACE_H_


#include "ads1263.h"

#define ADS_NUM_1_CHANNEL       0
#define ADS_NUM_2_CHANNEL       1
#define ADS_NUM_3_CHANNEL       2
#define ADS_NUM_4_CHANNEL       3
#define ADS_NUM_5_CHANNEL       4
#define ADS_NUM_6_CHANNEL       5
#define ADS_NUM_7_CHANNEL       6
#define ADS_NUM_8_CHANNEL       7
#define ADS_NUM_9_CHANNEL       8
#define ADS_NUM_10_CHANNEL      9
#define ADS_NUM_CHANNEL_MAX     10

extern Ads_ReturnType Ads_GetAdResult_Function(uint8_t valChannel);
#endif