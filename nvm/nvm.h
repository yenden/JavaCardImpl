#ifndef _nvm_h
#define _nvm_h

typedef char* string;
uint8_t nvmReader(uint32_t address);
string nvmErase(uint32_t address);
string nvmProgramByte(uint32_t address,uint8_t value);
string nvmProgramWord(uint32_t address, uint8_t *value, int len);
#endif