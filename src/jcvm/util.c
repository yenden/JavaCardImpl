
#include "util.h"

//read unsigned element on one byte
uint8_t readU1(const uint8_t *dataBuffer, uint16_t* iPos){
	uint8_t temp = dataBuffer[*iPos];
	(*iPos)++;
	return temp;
}

//read signed element on one byte
jcvm_byte readS1(const uint8_t *dataBuffer, uint16_t* iPos){
	jcvm_byte temp = (jcvm_byte)(dataBuffer[*iPos]);
	(*iPos)++;
	return temp;
}

//read unsigned element on two bytes
uint16_t readU2(const uint8_t *dataBuffer, uint16_t* iPos){
	uint16_t temp = (((uint16_t)dataBuffer[*iPos]&0x00FF) << 8) + (uint16_t)dataBuffer[*iPos+1];
	(*iPos)+=2;
	return temp;
}

//Read short
jcvm_short readS2(const uint8_t *dataBuffer, uint16_t* iPos){
	jcvm_short temp = (((jcvm_short)dataBuffer[*iPos]) << 8) + (jcvm_short)dataBuffer[*iPos+1];
	(*iPos)+=2;
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

uint8_t makeU1High(uint16_t data){
	uint8_t byte1 = (uint8_t)((data & 0xFF00) >> 8);
	return byte1;
}
uint8_t makeU1Low(uint16_t data){
	uint8_t byte2 = (uint8_t)(data & 0x00FF);
	return byte2;
}