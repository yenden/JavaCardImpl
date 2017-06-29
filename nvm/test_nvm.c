#include "nvm.h"
#include "../applet.h"
#include "../jcvm/util.h"


void printArray(uint8_t *display,uint16_t len){
    for(uint16_t i=0;i<len;i++){
        printf("%d ",display[i]);
    }
    printf("\n"); 
}
void fillIterationInWord(uint8_t *array, uint8_t i,uint16_t length){
    for (int j = 0; j < length; j++) {
		array[j] = i;
	}
}
int main(void){
    uint32_t address = 0;
    uint8_t value = 0;
    string err =NULL;
    uint8_t arrayLoop[7];
    uint8_t destArrayLoop[7];

    init_nvm();

    for(int loop=0;loop<0x10;loop++){
        fillIterationInWord(arrayLoop,loop,7);
        printf("LOOP: %d\n", loop);

        printf("Run NvmProgram\n");
        err = nvmWrite(address, arrayLoop,7);
	    printf("%s \n",err);

        printf("Run NvmReader \n");
		err = nvmRead(address,destArrayLoop,7);
        printf("%s \n",err);
        printArray(destArrayLoop,7);
        
		address++;
    }

    return 0;
}