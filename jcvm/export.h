#ifndef _export_h
#define _export_h

#include <stdint.h>

#define max_static_field 

//the Export component is not used for the 
// purse applet against which this implementation will be tested
typedef struct ExportComponent{
    uint8_t classCount;
    struct ClassExportinfo {
        uint16_t classOffset;
        uint8_t staticFieldCount;
        uint8_t staticMethodCount;
        uint16_t pStaticFieldOffsets[0];
        uint16_t pStaticMethodsOffsets[0];
    }pClassExport[0];
}ExportComponent;
#endif