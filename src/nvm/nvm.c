
#include "nvm.h"

#define null 0

typedef struct NvmWord {
    uint8_t wordArray[NvmWordSize];
}NvmWord;

typedef struct NvmSector{
    NvmWord sectorArray[NvmSectorSize/NvmWordSize];
}NvmSector;


typedef struct NvmImage {
    NvmSector nvmContent[NvmTotalSize/NvmSectorSize];
} NvmImage;

typedef struct NvmAddrProperties{
    uint16_t sectorId;
    uint16_t wordId;
    uint8_t offset;
}NvmAddrProperties;

//one image for all the program
static NvmImage *nvmImage;

//convert address to word
void nvmIdentifyAddr(const uint16_t address, NvmAddrProperties *addressId){
    if (address ==0) {
        addressId->sectorId = 0;
        addressId->wordId = 0;
        addressId->offset = 0;
    } else {
        addressId->sectorId = (uint16_t)(address/ (uint16_t)NvmSectorSize);
        addressId->wordId = (uint16_t)((address - (uint16_t)NvmSectorSize*addressId->sectorId) / (uint16_t)NvmWordSize);
        addressId->offset = (uint8_t)(address - (uint16_t)(NvmSectorSize * addressId->sectorId)- (uint16_t)((uint16_t)NvmWordSize * addressId->wordId));
    }
}

//checkers
string nvmCheckAccess(const uint16_t address){
    if (address > NvmTotalSize){
        return "ERR>NVM_CHECK_ACCESS_INVALID_ADDRESS";
    }
    return null;
}
string nvmCheckSectorAccess(uint16_t length){
    if (length > NvmSectorSize){
        return "ERR>NVM_CHECK_SECTOR_ACCESS_INVALID";
    }
    return null;
}
//erase methods
void nvmEraseWord(NvmWord *nvmWord){
    uint16_t size = sizeof(nvmWord->wordArray)/sizeof(uint8_t);
    for(uint16_t index = 0; index < size; index++){
        nvmWord->wordArray[index] = NvmErasedValue;
    }
}
void nvmEraseSector(NvmSector *nvmSector){
    uint16_t size = sizeof(nvmSector->sectorArray)/sizeof(NvmWord);
    for(uint16_t index = 0; index < size; index++){
        nvmEraseWord(&nvmSector->sectorArray[index]);
    }   
}
string nvmErase(const uint16_t address){
    string err = nvmCheckAccess(address);
    if (err != null){
        return err;
    }
    NvmAddrProperties addressProperties={0,0,0};
    NvmAddrProperties *p = &addressProperties;
    //identify the proper elements
	nvmIdentifyAddr(address, p);
    nvmEraseSector(&nvmImage->nvmContent[addressProperties.sectorId]);
	return null;
}
uint8_t changeBuff[sizeof(NvmImage)];
//allocate the image
void init_nvm(){
   // nvmImage = (NvmImage *)malloc(sizeof(NvmImage));
    nvmImage =  &changeBuff[0];
    //for the first use erase all the sectors
    uint16_t j = 0;
    for(uint16_t i = 0; i < (NvmTotalSize/NvmSectorSize); i++){
         nvmErase(FLASH_START_ADDRESS + j);
         j += NvmSectorSize;
    }
}
void free_nvm(){
    free(nvmImage);
}

//read method
string nvmRead(const uint16_t address, uint8_t *destBuffer, uint16_t length){
    string err = nvmCheckAccess(address);
    if (err !=null){
            return err;
    }
    err = nvmCheckSectorAccess(length);
    if (err !=null){
            return err;
    }
    NvmAddrProperties addressProperties={0,0,0};
    NvmAddrProperties *p = &addressProperties;
    uint16_t addr = address;
    //identify the proper sector and offset ids
    nvmIdentifyAddr(addr, p);

    //read data in destBuffer
    for(uint16_t i=0;i<length;i++){
        destBuffer[i]=nvmImage->nvmContent[addressProperties.sectorId].sectorArray[addressProperties.wordId].wordArray[addressProperties.offset];
        addr++;
        nvmIdentifyAddr(addr, p);  
    }
    return null;
}

void copyToSector(NvmSector *srcSector, NvmSector *destSector){
    for(uint16_t i=0;i< (NvmSectorSize/NvmWordSize);i++){
        for(uint16_t j=0;j< NvmWordSize;j++){
            destSector->sectorArray[i].wordArray[j] = srcSector->sectorArray[i].wordArray[j];
        }
    }
}

//write method
string nvmWrite(const uint16_t address, uint8_t *srcBuffer, uint16_t len){
    string err = nvmCheckAccess(address);
    if (err!=null){
        return err;
    }
    NvmAddrProperties addressProperties={0,0,0};
    NvmAddrProperties *p = &addressProperties;
    //identify the proper elements
    nvmIdentifyAddr(address, p);

    //copy the content of the Nvm sector in an intermediate sector
    NvmSector intermediateSector;
    copyToSector(&(nvmImage->nvmContent[p->sectorId]),&intermediateSector);
    uint16_t addr = address;

    //copy the range of source data into the intermediate sector
    for(uint16_t i=0;i<len;i++){
        intermediateSector.sectorArray[p->wordId].wordArray[p->offset] = srcBuffer[i];
        addr++;
        nvmIdentifyAddr(addr, p);  
    }
    //erase the sector
    nvmErase(address);
    nvmIdentifyAddr(address, p);

    //copy the content of the intermediateSector in the Nvm sector
    copyToSector(&intermediateSector,&(nvmImage->nvmContent[p->sectorId]));
    
	return null;
}

