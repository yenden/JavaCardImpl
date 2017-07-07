#include <stdio.h>
#include <stdlib.h>

#include "jcvm/installer/installer.h"
#include "jcre/dispatcher.h"
#include "../applet.h"


CardApplet constantApplet;


int main(void){
    init_nvm(); //allocate memory for non volatile memory
    iterate = 0;
    //installer
   // CardApplet newApplet;
    VM vm;
    initVM(&vm);
    installer(applet, applet_buff_length, &constantApplet);

    //card initialization
    cardInit();

    //main recieving apdus
    mainLoop(&vm);
    // callInstallMethod(&vm, &newApplet);
    free_nvm(); //free memory for non volatile memory
    return 0;
}
