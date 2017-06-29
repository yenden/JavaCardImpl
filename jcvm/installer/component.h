#ifndef _component_h
#define _component_h

#include <stdint.h>

#define max_AID_Length 8 //16

/*jcvm constants that are being use in components*/
#define TAG_HEADER_COMP   0x01
#define TAG_DIR_COMP      0x02
#define TAG_APPLET_COMP   0x03
#define TAG_IMPORT_COMP   0x04
#define TAG_CONSTANTPOOL_COMP   0x05
#define TAG_CLASS_COMP   0x06
#define TAG_METHOD_COMP   0x07
#define TAG_STATICFIELD_COMP   0x08
#define TAG_REFERENCELOCATION_COMP   0x09
#define TAG_EXPORT_COMP   0x0A
#define TAG_DESCRIPTOR_COMP   0x0B
#define TAG_DEBUG_COMP   0x0C

#define  ACC_PUBLIC      0x01
#define  ACC_PRIVATE     0x02
#define  ACC_PROTECTED   0x04
#define  ACC_STATIC      0x08
#define  ACC_FINAL       0x10

#define  ACC_ABSTRACT    0x40
#define  ACC_INIT        0x80

#define  TYPE_VOID                 0x0001
#define  TYPE_BOOLEAN              0x0002
#define  TYPE_BYTE                 0x0003
#define  TYPE_SHORT                0x0004
#define  TYPE_INT                  0x0005
#define  TYPE_REFERRENCE           0x0006
#define  TYPE_ARRAY_OF_BOOLEAN     0x000A
#define  TYPE_ARRAY_OF_BYTE        0x000B
#define  TYPE_ARRAY_OF_SHROT       0x000C
#define  TYPE_ARRAY_OF_INT         0x000D
#define  TYPE_ARRAY_OF_REFERENCE   0x000E


typedef struct PackageInfo{
    uint8_t minorVersion;
    uint8_t majorVersion;
    uint8_t AIDLength;
    uint8_t AID[max_AID_Length];
}PackageInfo;

#endif