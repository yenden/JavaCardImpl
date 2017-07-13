#ifndef _appletComp_h
#define _appletComp_h

#include <stdint.h>
#include "component.h"

#define max_Applet 1 //only for this implementation
                    // we assume there is only one applet

typedef struct AppletComponenent {
    uint8_t count;
    struct {
        uint8_t aidLength;
        uint8_t *pAID;
        uint16_t installMethodOffset;
    }applets[max_Applet];
}AppletComponenent;

#endif