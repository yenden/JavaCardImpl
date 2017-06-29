
#include "util.h"

//read unsigned element on one byte
uint8_t readU1(uint8_t* dataBuffer, uint16_t* iPos){
	uint8_t temp = dataBuffer[*iPos];
	(*iPos)++;
	return temp;
}

//read signed element on one byte
jcvm_byte readS1(uint8_t* dataBuffer, uint16_t* iPos){
	jcvm_byte temp = (jcvm_byte)(dataBuffer[*iPos]);
	(*iPos)++;
	return temp;
}

//read unsigned element on two bytes
uint16_t readU2(uint8_t* dataBuffer, uint16_t* iPos){
	uint16_t temp = (((uint16_t)dataBuffer[*iPos]&0x00FF) << 8) + (uint16_t)dataBuffer[*iPos+1];
	(*iPos)+=2;
	return temp;
}

//Read short
jcvm_short readS2(uint8_t* dataBuffer, uint16_t* iPos){
	jcvm_short temp = (((jcvm_short)dataBuffer[*iPos]) << 8) + (jcvm_short)dataBuffer[*iPos+1];
	(*iPos)+=2;
	return temp;
}
/*
uint32_t readU4(uint8_t* dataBuffer, uint16_t* iPos) {
	uint32_t temp = ((uint32_t)(dataBuffer[*iPos])<<24)+((uint32_t)dataBuffer[*iPos+1]<<16)+((uint32_t)(dataBuffer[*iPos+2])<<8)+(uint32_t)dataBuffer[*iPos+3];
	(*iPos) += 4;	
	return temp;
}

int32_t  readS4(uint8_t* dataBuffer, uint16_t* iPos){
	int32_t temp = ((int32_t)(dataBuffer[*iPos])<<24)+((int32_t)dataBuffer[*iPos+1]<<16)+((int32_t)(dataBuffer[*iPos+2])<<8)+(int32_t)dataBuffer[*iPos+3];
	(*iPos) += 4;	
	return temp;
}*/

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
/*
int32_t makeInt(int16_t short1,int16_t short2){
	return short1*0x10000+short2;
}
int16_t getShortHigh(int32_t value){
	return value/0x10000;
}

int16_t  getShortLow(int32_t value){
	return value % 0x10000;
}
*/
uint8_t makeU1High(uint16_t data){
	uint8_t byte1 = (uint8_t)((data & 0xFF00) >> 8);
	return byte1;
}
uint8_t makeU1Low(uint16_t data){
	uint8_t byte2 = (uint8_t)(data & 0x00FF);
	return byte2;
}