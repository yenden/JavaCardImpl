#ifndef _installer_h
#define _installer_h

#include <stdint.h>
#include "cardApplet.h"

void installer(uint8_t dataBuffer[], uint16_t length, CardApplet *newApplet);

#endif