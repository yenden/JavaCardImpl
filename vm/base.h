#ifndef _base_h
#define _base_h

uint8_t readU1(uint8_t* dataBuffer, int* iPos);
int8_t readS1(uint8_t* dataBuffer, int* iPos);
uint16_t readU2(uint8_t* dataBuffer, int* iPos);
int16_t readS2(uint8_t* dataBuffer, int* iPos);
uint32_t readU4(uint8_t* dataBuffer, int* iPos);
int32_t  readS4(uint8_t* dataBuffer, int* iPos);
uint8_t readHigh(uint8_t data);
uint8_t readLow(uint8_t data);
uint8_t readHighShift(uint8_t data);
uint16_t  makeU2(uint8_t byte1, uint8_t byte2);
int32_t makeInt(int16_t short1,int16_t short2);
int16_t getShortHigh(int32_t value);
int16_t  getShortLow(int32_t value);
#endif