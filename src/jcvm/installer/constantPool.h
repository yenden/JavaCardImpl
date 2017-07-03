#ifndef _constantPool_h
#define _constantPool_h

#include <stdint.h>

#define max_constant_pool 30

typedef struct CpInfo {
    uint8_t tag;
    uint8_t info[3];
}CpInfo;

typedef struct ConstantPoolComponent {
    uint16_t count;
    CpInfo constantPool[max_constant_pool];
}ConstantPoolComponent;


#endif