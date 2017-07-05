#ifndef _installer_h
#define _installer_h

#include "cardApplet.h"
#include "../util.h"
#include "../../nvm/nvm.h"
#include "../../addresses.h"


void installer(uint8_t dataBuffer[], uint16_t length, CardApplet *newApplet);

#endif