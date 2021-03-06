#ifndef _dispatcher_h
#define _dispatcher_h


#include <stdbool.h>
#include "../jcvm/appletsMethods.h"
#include "../nvm/nvm.h"
#include "api/apdu.h"


void initVM(VM *vm);
void cardInit();
void mainLoop(VM *vm);

#endif