#ifndef _staticField_h
#define _staticField_h

#include <stdint.h>

#define max_array_values 8
#define max_arrays 4
#define max_nonDefault_values 1

typedef struct ArrayInitInfo{
    uint8_t typ;
    uint16_t count;
    uint8_t pValues[max_array_values];
}ArrayInitInfo;

typedef struct StaticFieldComponent {
    uint16_t imageSize;
    uint16_t referenceCount;
    uint16_t arrayInitCount;
    ArrayInitInfo pArrayInit[max_arrays];
    uint16_t defaultValueCount;
    uint16_t nondefaultValueCount;
    uint8_t pnondefaultValues[max_nonDefault_values];
  //  uint8_t pStaticFieldImage[];
}StaticFieldComponent;

#endif