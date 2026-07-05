#ifndef __DWIN_DRIVER_H_
#define __DWIN_DRIVER_H_

#include <stdio.h>
#include <string.h>
#include "ring_buffer.h"
#include "SP3485EN.h"
#include "dwin_com.h"
#include "dwin_config.h"
#include "dwin_types.h"

extern const uint16_t  Dc_valChannelDataAddr_Mp[DWIN_CHANNEL_MAX];
extern struct dwin_value Dc_valChannelData_Mp[DWIN_CHANNEL_MAX];
extern struct dwin_value Dc_valLastChannelData_Mp[DWIN_CHANNEL_MAX];

static inline struct dwin_value* Dwin_GetChannelData(uint8_t Dd_valChannel)
{
	return &Dc_valChannelData_Mp[Dd_valChannel];
}

static inline void Dwin_SetChannelData(
	uint8_t Dd_valChannel,
	uint16_t current,
	uint16_t max,
	uint16_t min,
	uint16_t average,
	uint16_t wave
)
{
	Dc_valChannelData_Mp[Dd_valChannel].current = current;
	Dc_valChannelData_Mp[Dd_valChannel].max = max;
	Dc_valChannelData_Mp[Dd_valChannel].min = min;
	Dc_valChannelData_Mp[Dd_valChannel].average = average;
	Dc_valChannelData_Mp[Dd_valChannel].wave = wave;
}

static inline struct dwin_value* Dwin_GetLastChannelData(uint8_t Dd_valChannel)
{
	return &Dc_valLastChannelData_Mp[Dd_valChannel];
}

extern void Dwin_InitFunction(void);
extern void Dwin_MainFunction(void);
extern struct ring_buffer* Dwin_GetRingBuffer(void);

#endif /* __DWIN_DRIVER_H_ */
