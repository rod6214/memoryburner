#ifndef __AFTB_X9C103S_H__
#define __AFTB_X9C103S_H__ 

#include <stdlib.h>
#include <avr/io.h>

#ifndef POT_CS
#define POT_CS   A3
#endif

#ifndef POT_INC
#define POT_INC  A4
#endif

#ifndef POT_UD
#define POT_UD  A5
#endif

typedef struct _potx9 {
  int max_wiper;
  int min_wiper;
} POTX9_t;

extern POTX9_t potx9;

uint16_t x9c103s_reg(uint16_t value);
void x9c103s_init(void);
uint8_t x9c103s_detect(void);


#endif