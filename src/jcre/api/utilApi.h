#ifndef _utilApi_h
#define _utilApi_h

#include <stdint.h>

uint16_t arrayCopy(uint8_t *scrArray, uint16_t srcOff, uint8_t *destArray, uint16_t destOff, uint16_t length);
uint16_t arrayCopyNonAtomic(uint8_t *src, uint16_t srcOff, uint8_t *destArray, uint16_t destOff, uint16_t length);
uint16_t arrayfillNonAtomic(uint8_t *bArray, uint16_t bOff, uint16_t bLen, uint8_t bValue);
uint16_t arrayCompare(uint8_t *src, uint16_t srcOff, uint8_t *dest, uint16_t destOff, uint16_t length);
uint8_t checkArrayArgs(uint8_t *bArray, uint16_t bArrayLen, uint16_t offset, uint16_t length);


#endif