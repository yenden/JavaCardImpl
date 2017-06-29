#ifndef _method_h
#define _method_h

#include <stdint.h>

#define max_handlers 2
#define max_methoInfo 660

typedef struct ExceptionHandlerInfo {
        uint16_t startOffset;
        uint16_t activeLength;
        uint16_t handlerOffset;
        uint16_t catchTypeIndex;
}ExceptionHandlerInfo;

typedef struct MethodComponent {
    uint8_t handlerCount;
    ExceptionHandlerInfo pExceptionHandlers[max_handlers];
    uint8_t pMethodInfo[max_methoInfo]; 
}MethodComponent;

//void executeByteCode(uint16_t offset, AbstractApplet *pCA, VM *vm, bool invokerCond, bool processCond);

#endif