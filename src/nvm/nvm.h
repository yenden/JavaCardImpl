#ifndef _nvm_h
#define _nvm_h

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../addresses.h"

#define NvmTotalSize 2688//3072 //32kb
#define NvmErasedValue 0xFF
#define NvmWordSize 8
#define NvmSectorSize 256 //4096

typedef unsigned char* string;
string nvmRead(const uint16_t address, uint8_t *destBuffer, uint16_t length);
string nvmWrite(const uint16_t address, uint8_t *srcBuffer, uint16_t len);
void init_nvm();
void free_nvm();

#endif
