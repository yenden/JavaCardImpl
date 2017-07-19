#if 1
#include <stdio.h>
#endif

#include "nadiaprintf.h"
#if 0
#include "../../myPrintf.h"
#endif

#include <string.h>

void nadiaprintf(uint8_t *arrayToprint, uint8_t length, uint8_t type){
#if 1
	if(type == CHAR){ //char
        printf(arrayToprint);
    } else if(type == HEX){
        for(uint8_t i = 0; i < length; i++){
             printf("%x ", arrayToprint[i]);
        }
    }
    printf("\n");
#endif
#if 0
    myPrintf(arrayToprint,length, type);
#endif
}
