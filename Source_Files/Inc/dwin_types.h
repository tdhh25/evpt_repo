#ifndef __DWIN_TYPES_H_
#define __DWIN_TYPES_H_

#include <stdint.h>

enum dwin_step {
	dwin_step_process = 0,
	dwin_step_swtich_frac,
};

enum dwin_addr {
	dwin_addr_average 	= 0x2100,
	dwin_addr_frac 		= 0x2101,
	dwin_addr_hold 		= 0x2102,
};

struct dwin_value {
	uint16_t		current;
	uint16_t 		max;
	uint16_t 		min;
	uint16_t 		average;
	uint16_t 		wave;
	uint16_t		reserved;
};

struct dwin_ctrl {
	uint8_t			average;
	uint8_t			fractional;
	uint8_t			fractional_status; /* 0: .0 1: 0.00*/
	uint8_t			hold;
};

struct dwin_device {
	uint8_t					step;
	uint8_t 				value_channel;
	struct dwin_ctrl		ctrl;
	struct dwin_value*		value;
};

#endif /* __DWIN_TYPES_H_ */
