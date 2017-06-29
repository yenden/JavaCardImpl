#ifndef _import_h
#define _import_h

#include <stdint.h>
#include "component.h"

//for this implementation we assume 
//two packages at most will be imported 
//: java.lang for object creation and 
// javacard.framework

#define max_Packages 2
typedef struct ImportComponent {
    uint8_t count;
    PackageInfo packages[max_Packages];
}ImportComponent;
#endif