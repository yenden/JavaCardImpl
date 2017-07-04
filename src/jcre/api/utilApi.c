#include "utilApi.h"

//ArrayCopy copies an array in another 
uint16_t arrayCopy(uint8_t *scrArray, uint16_t srcOff, uint8_t *destArray, uint16_t destOff, uint16_t length){
    for(uint16_t i = 0; i < length; i++){
        destArray[destOff + i] = scrArray[srcOff + i];
    }
    return destOff + length;
}

//ArrayCopyNonAtomic copies an array in another 
uint16_t arrayCopyNonAtomic(uint8_t *srcArray, uint16_t srcOff, uint8_t *destArray, uint16_t destOff, uint16_t length){
    arrayCopy(srcArray, srcOff, destArray,  destOff, length);
}

//ArrayfillNonAtomic fiels an array with a value
uint16_t arrayfillNonAtomic(uint8_t *bArray, uint16_t bOff, uint16_t bLen, uint8_t bValue){
    if(bLen < 0){
        return 0;
    }
    uint16_t len = bLen;
    uint16_t offset = bOff;
    while(len > 0){
        bArray[offset] = bValue;
        offset++;
        len--;
    }
    return offset + len;
}

//ArrayCompare compare two parts of two arrays
uint16_t arrayCompare(uint8_t *src, uint16_t srcOff, uint8_t *dest, uint16_t destOff, uint16_t length){
    if(length < 0){
        return 65534;
    }
    for(uint16_t i = 0; i < length; i++){
        if(src[srcOff + i] != dest[destOff + i]){
            if (src[srcOff + i] < dest[destOff + i]){
                return 65535; 
            }
            return 1;
        }
    }
    return 0;
}

//CheckArrayArgs checks the offset and the length of an array
uint8_t checkArrayArgs(uint8_t *bArray, uint16_t bArrayLen, uint16_t offset, uint16_t length){
    uint16_t len = bArrayLen -1;
    if ((offset < 0) &&( offset > len) && (length <= len - offset)) {
        return 1;
    }
    return 0;
}