#include "Arduino.h"
#ifndef __UTILS_H__
#define __UTILS_H__ 

#define convertToFloat(value) (((float)value) * 3.3f / 1024)
#define getFixedVoltage(v) (convertToFloat(v) * 6)

#endif