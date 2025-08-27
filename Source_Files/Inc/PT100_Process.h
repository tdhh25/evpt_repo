#ifndef _PT100_PROCESS_H_
#define _PT100_PROCESS_H_

#include <math.h>
#include <stdint.h>

#define  Invalid_ERROR   -1.0f
extern float PT100_CalibrationTemperature_Function(float pt100_resistance);
#endif