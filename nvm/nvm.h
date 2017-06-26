#ifndef _nvm_h
#define _nvm_h

#include <stdint.h>
#include <stdio.h>

typedef char* string;
string nvmRead(const uint16_t address, uint8_t *destBuffer, uint16_t length);
string nvmWrite(const uint16_t address, uint8_t *srcBuffer, uint16_t len);
void init_nvm();

#endif