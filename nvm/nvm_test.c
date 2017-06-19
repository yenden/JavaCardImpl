
#include "nvm.c"
void printArray(uint8_t *display,int len){
    for(int i=0;i<len;i++){
        printf("%d ",display[i]);
    }
    printf("\n"); 
}
void readLoop(uint32_t address){
    uint8_t value;
    string err;
    uint8_t display[16];
    address = 0;
    printf("Read NVM from offset 0\t");
    for(int loop=0;loop<16;loop++){
        value=nvmReader(address+(uint32_t)loop);
        display[loop]=value;
    }
    printArray(display,16);
}
void fillIterationInWord(uint8_t array[],int i){
    for (int j = 0; j < 5; j++) {
		array[j] = (uint8_t)i;
	}
}
int main(void){
    uint32_t address = 0;
    uint8_t value = 0;
    string err =NULL;
    uint8_t arrayLoop[5];
    init();
     for(int loop=0;loop<0x10;loop++){
        fillIterationInWord(arrayLoop,loop);
        printf("LOOP: %d\n", loop);
		printf("%d \n",value);
		printf("Run NvmReader \n");
		value = nvmReader(address);
        printf("%d \n",value);
		printf("Run NvmErase \n");
		err = nvmErase(address);
		printf("%s \n",err);
		printf("Run NvmReader\n");
		value = nvmReader(address);
		printf("%d \n",value);
		printf("Run NvmProgram\n");
		err = nvmProgramWord(address, arrayLoop,5);
	    printf("%s \n",err);
		printf("Run NvmReader\n");
		value = nvmReader(address);
		printf("%d \n",value);
		printf("Run NvmProgram\n");
		err = nvmProgramByte(address+1, (uint8_t)loop);
		printf("%s \n",err);
		readLoop(address);
		address++;
	}
	//write to one more address. Should fail
	printf("Run NvmReader\n");
	value = nvmReader(address);
	printf("%d \n",value);
	printf("Run NvmProgram\n");
	err = nvmProgramByte(address, 0xAA);
	printf("%s \n",err);
	printf("Run NvmReader\n");
	value= nvmReader(address);
	printf("%d \n",value);

    return 0;
}


