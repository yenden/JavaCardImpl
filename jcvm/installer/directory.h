#ifndef _directory_h
#define _directory_h

#include <stdint.h>
typedef struct StaticFieldSizeInfo StaticFieldSizeInfo;
struct StaticFieldSizeInfo{
    uint16_t imageSize;
    uint16_t arrayInitCount;
    uint16_t arrayInitSize;
};

typedef struct DirectoryComponent DirectoryComponent;
struct DirectoryComponent{
    uint16_t componentSizes[12];
    StaticFieldSizeInfo pStaticFieldSize;
    uint8_t importCount;
    uint8_t appletCount;
    //This implementation doesn't support
    //custom components
};

#endif