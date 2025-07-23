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

struct dwin_device {
	uint8_t 			step;
	struct dwin_value*	value;
};

#endif /* __DWIN_TYPES_H_ */
