
#include "base.h"

uint8_t readU1(uint8_t* dataBuffer, int* iPos){
	uint8_t temp = dataBuffer[*iPos];
	(*iPos)++;
	return temp;
}
int8_t readS1(uint8_t* dataBuffer, int* iPos){
	int8_t temp = (int8_t)(dataBuffer[*iPos]);
	(*iPos)++;
	return temp;
}

uint16_t readU2(uint8_t* dataBuffer, int* iPos){
	uint16_t temp = (((uint16_t)dataBuffer[*iPos]&0x00FF) << 8) + (uint16_t)dataBuffer[*iPos+1];
	(*iPos)+=2;
	return temp;
}

int16_t readS2(uint8_t* dataBuffer, int* iPos){
	int16_t temp = (((int16_t)dataBuffer[*iPos]) << 8) + (int16_t)dataBuffer[*iPos+1];
	(*iPos)+=2;
	return temp;
}

uint32_t readU4(uint8_t* dataBuffer, int* iPos) {
	uint32_t temp = ((uint32_t)(dataBuffer[*iPos])<<24)+((uint32_t)dataBuffer[*iPos+1]<<16)+((uint32_t)(dataBuffer[*iPos+2])<<8)+(uint32_t)dataBuffer[*iPos+3];
	(*iPos) += 4;	
	return temp;
}

int32_t  readS4(uint8_t* dataBuffer, int* iPos){
	int32_t temp = ((int32_t)(dataBuffer[*iPos])<<24)+((int32_t)dataBuffer[*iPos+1]<<16)+((int32_t)(dataBuffer[*iPos+2])<<8)+(int32_t)dataBuffer[*iPos+3];
	(*iPos) += 4;	
	return temp;
}

uint8_t readHigh(uint8_t byte){
	return byte & 0xF0;
}

uint8_t readLow(uint8_t byte){
	return byte & 0x0F;
}

uint8_t readHighShift(uint8_t byte){
	return (byte & 0xF0)>>4;
}

uint16_t  makeU2(uint8_t byte1, uint8_t byte2){
	return byte1*0x100+byte2;
}

int32_t makeInt(int16_t short1,int16_t short2){
	return short1*0x10000+short2;
}

int16_t getShortHigh(int32_t value){
	return value/0x10000;
}

int16_t  getShortLow(int32_t value){
	return value % 0x10000;
}


