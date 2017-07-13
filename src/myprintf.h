#ifndef _myprintf_h
#define _myprintf_h

#include <stdint.h>



void myprintf(uint8_t *arrayToprint, uint8_t length, uint8_t type);

#define HEX 0x11//par defaut
#define CHAR 0x10

#endif