#include <stdbool.h>
#include "appletsMethods.h"
#include "interpreter.h"


//method install of the Applet
void callInstallMethod( VM *vm, CardApplet *cardApp){
    printf("Start installing \n");
    uint16_t offset = cardApp->pApplet.applets[0].installMethodOffset;
	executeByteCode(vm, &cardApp->absApp, offset, false);
	printf("Install finished!\n");
}

//process method 
void callProcessMethod(VM *vm, CardApplet *cardApp){
    printf("Start processing the apdu ...");
    JcvmValue instanceClassRef = vm->stackFrame[vm->frameTop].localVariables[0];
    //todo
}