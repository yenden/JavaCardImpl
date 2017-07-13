#include <stdio.h>
#include "myprintf.h"
#include <string.h>

void myprintf(uint8_t *arrayToprint, uint8_t length, uint8_t type){
    if(type == CHAR){ //char
        printf(arrayToprint);
    } else if(type == HEX){
        for(uint8_t i = 0; i < length; i++){
             printf("%x ", arrayToprint[i]);
        }
    }
    printf("\n");
}