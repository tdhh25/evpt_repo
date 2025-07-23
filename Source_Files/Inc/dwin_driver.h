#ifndef __DWIN_DRIVER_H_
#define __DWIN_DRIVER_H_

#include <stdio.h>
#include "SP3485EN.h"
#include "dwin_com.h"
#include "dwin_config.h"
#include "dwin_types.h"

extern struct dwin_value Dc_valChannelData_Mp[DWIN_CHANNEL_MAX];

static inline struct dwin_value* Dwin_GetChannelData(uint8_t Dd_valChannel)
{
	return &Dc_valChannelData_Mp[Dd_valChannel];
}

extern void Dwin_MainFunction(void);

#endif /* __DWIN_DRIVER_H_ */
