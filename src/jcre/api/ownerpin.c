#include "ownerpin.h"


#define VALIDATED 0
#define NUMFLAGS 1

#define OFFSET_TRY_LIMIT 0
#define OFFSET_MAX_PINSIZE 8
#define OFFSET_PINSIZE 16
#define OFFSET_FLAGS 24
#define OFFSET_TRIES_LEFT 32
#define OFFSET_PIN_ARRAY 40

bool getValidatedFlag(uint16_t ownerpinAddress){
    uint8_t result; 
    nvmRead(ownerpinAddress + OFFSET_FLAGS, &result, 1);
    if(result == 0){
        return false;
    }
    return true;
}
void setValidatedFlag(uint8_t *array, bool value){
    uint8_t result;
    if(value == false){
        result = 0;
    } else {
        result = 1;
    }
    //copy to Flags array
   array[OFFSET_FLAGS] = result;
}

void resetTriesRemaining(uint8_t *array){
    uint8_t result = array[OFFSET_TRY_LIMIT];
    //copy it to triesLeft
    array[OFFSET_TRIES_LEFT] = result;
}

void decrementTriesRemaining(uint8_t *array){
     // triesLeft
    array[OFFSET_TRIES_LEFT] --;
    
}

uint16_t initOwnerPin(uint16_t ownerpinAddress, uint8_t tryLimit, uint8_t maxPINSize){
    if((tryLimit < 1) || (maxPINSize < 1)){
        return ILLEGAL_VALUE;
    }
    uint8_t size = 5*8;
    uint8_t arrayOwnerPIN[size];
    arrayOwnerPIN[OFFSET_TRY_LIMIT] = tryLimit;
    arrayOwnerPIN[OFFSET_MAX_PINSIZE] = maxPINSize;
    arrayOwnerPIN[OFFSET_PINSIZE] = maxPINSize;

    setValidatedFlag(arrayOwnerPIN, false);
    resetTriesRemaining(arrayOwnerPIN);

    nvmWrite(ownerpinAddress, arrayOwnerPIN, size);
    return 0;
}

uint8_t getTriesRemaining(uint16_t ownerpinAddress){
    uint8_t result;
    nvmRead(ownerpinAddress + OFFSET_TRIES_LEFT, &result, 1);
    return result;
}

bool isValidated(uint16_t ownerpinAddress){
   return getValidatedFlag(ownerpinAddress);
}

//unblock 
void resetAndUnblock(uint16_t ownerpinAddress){
    uint8_t maxPINSize = 0;
    nvmRead(ownerpinAddress + OFFSET_MAX_PINSIZE, &maxPINSize, 1); 
    uint8_t size = 5*8 + maxPINSize/8;
    if(maxPINSize%8 != 0){
        size++;
    }
    uint8_t arrayOwnerPIN[size];
    nvmRead(ownerpinAddress, arrayOwnerPIN, size);
    resetTriesRemaining(arrayOwnerPIN);
    setValidatedFlag(arrayOwnerPIN, false);

    nvmWrite(ownerpinAddress, arrayOwnerPIN, size);
}
void reset(uint16_t ownerpinAddress){
    if(isValidated(ownerpinAddress)){
        resetAndUnblock(ownerpinAddress);
    }
}

//check if the code pin is correct
bool check(uint16_t ownerpinAddress, uint8_t *pin, uint16_t offset, uint8_t length){
    bool noMoreTries = false;
    uint8_t maxPINSize = 0;
    uint8_t pinSize = 0;
    nvmRead(ownerpinAddress + OFFSET_MAX_PINSIZE, &maxPINSize, 1); 
    uint8_t size = 5*8 + (maxPINSize/8)*8;
    if((maxPINSize%8 != 0)&&(maxPINSize > 8)){
        size +=8;
    }
    uint8_t arrayOwnerPIN[size];
    nvmRead(ownerpinAddress, arrayOwnerPIN, size);

    setValidatedFlag(arrayOwnerPIN, false);
    if (getTriesRemaining(ownerpinAddress) == 0){
        noMoreTries = true;
    } else {
        decrementTriesRemaining(arrayOwnerPIN);
    }
    if(length > 0){
        pinSize = arrayOwnerPIN[OFFSET_PINSIZE];
        if((length != pinSize) || (noMoreTries == true)){
            return false;
        }
    }
    uint8_t n = arrayCompare(pin, offset, arrayOwnerPIN, OFFSET_PIN_ARRAY, length);
    if(n==0 && length == pinSize){
        setValidatedFlag(arrayOwnerPIN, true);
        resetTriesRemaining(arrayOwnerPIN);
        return true;
    }
    return false;
}

//update the code PIn in NVM 
uint8_t update(uint16_t ownerpinAddress, uint8_t *pin, uint16_t offset, uint8_t  length){
    uint8_t maxPINSize = 0;
    nvmRead(ownerpinAddress + OFFSET_MAX_PINSIZE, &maxPINSize, 1); 
    uint8_t size = 5*8 + (maxPINSize/8)*8;
    if((maxPINSize%8 != 0)&&(maxPINSize > 8)){
        size += 8;
    }
    uint8_t arrayOwnerPIN[size];
    nvmRead(ownerpinAddress, arrayOwnerPIN, size);

    if(length > maxPINSize){
        return ILLEGAL_VALUE;
    }
    arrayCopy(pin, offset, arrayOwnerPIN, OFFSET_PIN_ARRAY, length);
    arrayOwnerPIN[OFFSET_PINSIZE] = length;
    arrayOwnerPIN[OFFSET_TRIES_LEFT] = arrayOwnerPIN[OFFSET_TRY_LIMIT];
    setValidatedFlag(arrayOwnerPIN, false);

    nvmWrite(ownerpinAddress, arrayOwnerPIN, size);
    return 0;
}