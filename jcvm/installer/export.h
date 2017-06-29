#ifndef _export_h
#define _export_h

#include <stdint.h>

#define max_export_static_field 0
#define max_export_method 0
#define max_export_class 0

//the Export component is not used for the 
// purse applet against which this implementation will be tested
typedef struct ExportComponent{
    uint8_t classCount;
    struct ClassExportinfo {
        uint16_t classOffset;
        uint8_t staticFieldCount;
        uint8_t staticMethodCount;
        uint16_t pStaticFieldOffsets[max_export_static_field];
        uint16_t pStaticMethodsOffsets[max_export_method];
    }pClassExport[max_export_class];
}ExportComponent;
#endif