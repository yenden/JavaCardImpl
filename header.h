#ifndef _header_h
#define _header_h

#include "base.h"
#include "component.h"
typedef struct HeaderComponent HeaderComponent;
struct HeaderComponent{
    u1 minorVersion;
    u1 majorVersion;
    u1 flags;
    PackageInfo pThisPackage;
	PackageNameInfo pNameInfor;
};

typedef struct PackageNameInfo PackageNameInfo;
struct PackageNameInfo{
    u1 nameLength;
	u1 name[nameLength];       
};
#endif