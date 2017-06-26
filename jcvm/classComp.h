#ifndef _classComp_h
#define _classComp_h

#include<stdint.h>

#define max_classes 1
#define max_methods 10

//This implementation does not support remote classes
//nor remote interfaces


//The applet against which the implementation
//will be tested has no interface declared
//thus we will not consider interfaces

typedef struct ClassInfo {
    uint8_t bitfield;
    uint16_t superClassRef;
    uint8_t declaredInstanceSize;
    uint8_t firstReferenceToken;
    uint8_t referenceCount;
    uint8_t publicMethodTableBase;
    uint8_t publicMethodTableCount;
    uint8_t packageMethodTableBase;
    uint8_t packageMethodTableCount;
    uint16_t publicMethodTable[max_methods];
    uint16_t packageMethodTable[max_methods];
    //ImplementedInterfaceInfo *interfaces;
}ClassInfo;

/*
struct InterfaceInfo {
    uint8_t bitfield;
    uint16_t superInterfaces[];
};

struct ImplementedInterfaceInfo {
    uint16_t interfaceRf;
    uint8_t count;
    uint8_t *index;
};
*/
typedef struct ClassComponent {
   // InterfaceInfo *interfaces;
    ClassInfo pClasses[max_classes];
}ClassComponent;



#endif