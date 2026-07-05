#ifndef _PT100_PROCESS_H_
#define _PT100_PROCESS_H_

#include <math.h>
#include <stdint.h>

#define  Invalid_ERROR   -1.0f
extern double PT100_CalibrationTemperature_Function(double pt100_resistance);
#endif