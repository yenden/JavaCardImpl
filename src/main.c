#include <stdio.h>
#include <stdlib.h>
#include "nvm/nvm.h"
#include "jcvm/installer/installer.h"
#include "jcvm/installer/cardApplet.h"
#include "jcvm/appletsMethods.h"
#include "jcvm/vmUtil.h"
#include "applet.h"

void createVM(VM *vm){
    vm->frameTop = -1;
    Frame frame;
    frame.opStackTop = -1;
    pushFrame(vm, frame);
}

int main(void){
    init_nvm(); //allocate memory for non volatile memory
    CardApplet newApplet;
    VM vm;
    createVM(&vm);
    installer(applet, applet_buff_length, &newApplet);
    callInstallMethod(&vm, &newApplet);
    free_nvm(); //free memory for non volatile memory
    return 0;
}
