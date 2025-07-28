#ifndef __DWIN_TYPES_H_
#define __DWIN_TYPES_H_

#include <stdint.h>

enum dwin_addr {
	dwin_addr_average = 0x2100,
};

struct dwin_value {
	uint16_t		current;
	uint16_t 		max;
	uint16_t 		min;
	uint16_t 		average;
	uint16_t 		wave;
	const uint16_t 	base_addr;
};

struct dwin_ctrl {
	uint8_t			average;
	uint8_t			fractional;
	uint8_t			hold;
};

struct dwin_device {
	uint8_t 				step;
	struct dwin_ctrl		ctrl;
	struct dwin_value*		value;
};

#endif /* __DWIN_TYPES_H_ */
