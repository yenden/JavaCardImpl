#ifndef _header_h
#define _header_h

#include <stdint.h>
#include "component.h"

#define max_Name_Length 20

typedef struct PackageNameInfo PackageNameInfo;
struct PackageNameInfo{
    uint8_t nameLength;
	uint8_t name[max_Name_Length];       
};

typedef struct HeaderComponent HeaderComponent;
struct HeaderComponent{
    uint8_t minorVersion;
    uint8_t majorVersion;
    uint8_t flags;
    PackageInfo pThisPackage;
	PackageNameInfo pNameInfo;
};


#endif