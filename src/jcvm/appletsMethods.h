#ifndef _appletsMethods_h
#define _appletsMethods_h

#include <stdbool.h>
#include "interpreter.h"


void callInstallMethod( VM *vm, CardApplet *cardApp);
void callProcessMethod(VM *vm, CardApplet *cardApp);

#endif