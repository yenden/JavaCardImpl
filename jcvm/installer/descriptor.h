#ifndef _descriptor_h
#define _descriptor_h

#include <stdint.h>


#define max_types 29
#define max_nibble 6
#define max_fields 9
#define max_methods 10
#define max_pool 30
#define max_desc_classes 2

typedef struct MethodDescriptorInfo {
    uint8_t token;
    uint8_t pAF;
    uint16_t methodOffset;
    uint16_t typeOffset;
    uint16_t bytecodeCount;
    uint16_t exceptionHandlerCount;
    uint16_t exceptionHandlerIndex;
}MethodDescriptorInfo;
typedef struct FieldRef{
    uint8_t fieldRef[3];
}FieldRef;
typedef struct FieldDescriptorInfo {
    uint8_t token;
    uint8_t pAF;
    FieldRef pFieldRef;
    uint16_t pFieldType;
}FieldDescriptorInfo;
typedef struct ClassDescriptorInfo {
    uint8_t token;
    uint8_t accessFlag;
    uint16_t thisClassRef;
   // uint8_t interfaceCount;
    uint16_t fieldCount;
    uint16_t methodCount;
   // []uint16 interfaces;
   FieldDescriptorInfo fields[max_fields];
   MethodDescriptorInfo methods[max_methods]; 
}ClassDescriptorInfo;

typedef struct TypeDescriptor{
    uint8_t nibbleCount;
    uint8_t typeArray[max_nibble];
}TypeDescriptor;
typedef struct TypeDescriptorInfo{
    uint16_t constPoolCount;
    uint16_t pConstantPoolTypes[max_pool];
    TypeDescriptor pTypeDesc[max_types];
}TypeDescriptorInfo;

typedef struct DescriptorComponent {
    uint8_t classCount;
    ClassDescriptorInfo classes[max_desc_classes];
    TypeDescriptorInfo types;
}DescriptorComponent;

#endif