#ifndef _dispatcher_h
#define _dispatcher_h

#include <stdio.h>
#include <stdbool.h>
#include "../jcvm/appletsMethods.h"
#include "../nvm/nvm.h"
#include "api/apdu.h"

extern CardApplet constantApplet; 

void initVM(VM *vm);
void cardInit();
void mainLoop(VM *vm);

#endif