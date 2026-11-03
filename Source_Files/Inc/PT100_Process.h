#ifndef _PT100_PROCESS_H_
#define _PT100_PROCESS_H_

#include <math.h>
#include <stdint.h>


#define PT100_TEMP_INVALID_C100   (-1.0E6) 
#define PT100_TEMP_MIN_C100       (-2.0E4)   
#define PT100_TEMP_MAX_C100       (8.5E4) 
extern double PT100_CalibrationTemperature_Function(double pt100_resistance);
#endif