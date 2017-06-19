#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "nvm.h"

#define NvmTotalSize 0x8000 //32kb
#define NvmErasedValue 0xFF
#define NvmWordSize 8
#define NvmSectorSize 256 //4096




typedef struct NvmWord NvmWord;
struct NvmWord {
    uint8_t wordArray[NvmWordSize];
    bool wordErasedStatus;
};

typedef struct NvmSector NvmSector;
struct NvmSector{
    NvmWord sectorArray[NvmSectorSize/NvmWordSize];
};

typedef struct NvmImage NvmImage;
struct NvmImage {
    NvmSector nvmContent[NvmTotalSize/NvmSectorSize];
};

static NvmImage *nvmImage;
void init(){
    nvmImage = (NvmImage *)malloc(sizeof(NvmImage));
}

typedef struct NvmAddrProperties NvmAddrProperties;
struct NvmAddrProperties{
    uint16_t sectorId;
    uint16_t wordId;
    uint8_t offset;
};

//convert address to word
void nvmIdentifyAddr(uint32_t address, NvmAddrProperties *addressId){
    if (address ==0) {
        addressId->sectorId = 0;
        addressId->wordId = 0;
        addressId->offset = 0;
    } else {
        addressId->sectorId = (uint16_t)(address/ (uint32_t)NvmSectorSize);
        addressId->wordId = (uint16_t)((address - (uint32_t)NvmSectorSize*addressId->sectorId) / (uint32_t)NvmWordSize);
        addressId->offset = (uint8_t)(address - (uint32_t)(NvmSectorSize * addressId->sectorId)- (uint32_t)((uint16_t)NvmWordSize * addressId->wordId));
    }
}

//checkers
string nvmCheckAccess(uint32_t address){
    if (address > NvmTotalSize){
        return "ERR>NVM_CHECK_ACCESS_INVALID_ADDRESS";
    }
    return NULL;
}
string nvmCheckErasedState(uint32_t address){
    NvmAddrProperties addressProperties={0,0,0};
    NvmAddrProperties *p = &addressProperties; 
    //identify the proper elements
    nvmIdentifyAddr(address, p);
    //check that the word is in erased state
    if (nvmImage->nvmContent[addressProperties.sectorId].sectorArray[addressProperties.wordId].wordErasedStatus != true ){
		return "ERR>NVM_ERASED_STATE_INVALID";
	}
	return NULL;
}

//methods
uint8_t nvmReader(uint32_t address){
    string err = nvmCheckAccess(address);
    if (err !=NULL){
        //todo
        return 0xFF;
    }
    NvmAddrProperties addressProperties={0,0,0};
    NvmAddrProperties *p = &addressProperties;
    //identify the proper elements
    nvmIdentifyAddr(address, p);
    uint8_t data;
    data = nvmImage->nvmContent[addressProperties.sectorId].sectorArray[addressProperties.wordId].wordArray[addressProperties.offset];
	return data;
}
void nvmEraseWord(NvmWord *nvmWord){
    nvmWord->wordErasedStatus = true;
    int size = sizeof(nvmWord->wordArray)/sizeof(uint8_t);
    for(int index = 0; index < size; index++){
        nvmWord->wordArray[index] = NvmErasedValue;
    }
}
void nvmEraseSector(NvmSector *nvmSector){
    int size = sizeof(nvmSector->sectorArray)/sizeof(NvmWord);
    for(int index = 0; index < size; index++){
        nvmEraseWord(&nvmSector->sectorArray[index]);
    }   
}
string nvmErase(uint32_t address){
    string err = nvmCheckAccess(address);
    if (err != NULL){
        return err;
    }
    NvmAddrProperties addressProperties={0,0,0};
    NvmAddrProperties *p = &addressProperties;
    //identify the proper elements
	nvmIdentifyAddr(address, p);
    nvmEraseSector(&nvmImage->nvmContent[addressProperties.sectorId]);
	//printf("Erased value is : ", nvmImage->NvmContent[addressProperties.sectorId])
	printf("ERASED OK\n");
	return NULL;

}
string nvmProgramByte(uint32_t address,uint8_t value){
    string err = nvmCheckAccess(address);
    if (err!=NULL){
        return err;
    }
    NvmAddrProperties addressProperties={0,0,0};
    NvmAddrProperties *p = &addressProperties;
    //identify the proper elements
	nvmIdentifyAddr(address, p);

    err = nvmCheckErasedState(address);
    if (err != NULL){
        return err;
    }
    nvmImage->nvmContent[addressProperties.sectorId].sectorArray[addressProperties.wordId].wordArray[addressProperties.offset] = value;
	nvmImage->nvmContent[addressProperties.sectorId].sectorArray[addressProperties.wordId].wordErasedStatus = false;
	return NULL;
}
string nvmCheckWordLength(uint8_t *value,int len, uint8_t offset){
    if (len > (int)(NvmWordSize-offset)){
        return "ERR>INCORRECT LENGTH";
    }
    return NULL;
}
string nvmProgramWord(uint32_t address, uint8_t *value, int len){
    string err = nvmCheckAccess(address);
    if (err!=NULL){
        return err;
    }
    NvmAddrProperties addressProperties={0,0,0};
    NvmAddrProperties *p = &addressProperties;
    //identify the proper elements
    nvmIdentifyAddr(address, p);

    err = nvmCheckErasedState(address);
    if (err!=NULL){
        return err;
    }
    err = nvmCheckWordLength(value, len,addressProperties.offset);
	if (err!=NULL){
        return err;
    }
   
    for(int i=0;i< len;i++){
        nvmImage->nvmContent[addressProperties.sectorId].sectorArray[addressProperties.wordId].wordArray[addressProperties.offset+(uint8_t)i] = value[i];
    }
    nvmImage->nvmContent[addressProperties.sectorId].sectorArray[addressProperties.wordId].wordErasedStatus = false;
	return NULL;
}

