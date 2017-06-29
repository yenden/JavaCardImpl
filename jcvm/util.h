#ifndef _util_h
#define _util_h

#include <stdint.h>

//jcvm types
typedef int8_t jcvm_byte; // byte type
typedef int16_t jcvm_short; //short type
typedef uint16_t jcvm_Reference;

//methods
uint8_t readU1(uint8_t* dataBuffer, uint16_t* iPos);
uint16_t readU2(uint8_t* dataBuffer, uint16_t* iPos);

jcvm_byte readS1(uint8_t* dataBuffer, uint16_t* iPos);
jcvm_short readS2(uint8_t* dataBuffer, uint16_t* iPos);

uint8_t readHigh(uint8_t data);
uint8_t readLow(uint8_t data);
uint8_t readHighShift(uint8_t data);
uint16_t  makeU2(uint8_t byte1, uint8_t byte2);

#endif