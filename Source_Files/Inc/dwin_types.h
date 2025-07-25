#ifndef __DWIN_TYPES_H_
#define __DWIN_TYPES_H_

#include <stdint.h>

struct dwin_value {
	uint16_t		current;
	uint16_t 		max;
	uint16_t 		min;
	uint16_t 		average;
	uint16_t 		wave;
	const uint16_t 	base_addr;
};

struct dwin_scheduler{
    uint8_t channel;
    uint8_t index;
};

struct dwin_device {
	uint8_t 				step;
	struct dwin_value*		value;
	struct dwin_scheduler	scheduler;
};

#endif /* __DWIN_TYPES_H_ */
