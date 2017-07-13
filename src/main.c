
#include <stdlib.h>

#include "jcvm/installer/installer.h"
#include "jcre/dispatcher.h"



CardApplet constantApplet;


int main(void){
   init_nvm(); //allocate memory for non volatile memory
    //CardApplet newApplet;
    iterate = 0;
    //installer
    VM vm;
    initVM(&vm);
    installer(&constantApplet);

    //card initialization
    cardInit();

    //main recieving apdus
    mainLoop(&vm);

    free_nvm(); //free memory for non volatile memory
    return 0;
}
