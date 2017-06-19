
#include "base.h"

u1 readU1(u1* dataBuffer, int* iPos){
	u1 temp = dataBuffer[*iPos];
	(*iPos)++;
	return temp;
}
s1 readS1(u1* dataBuffer, int* iPos){
	s1 temp = (s1)(dataBuffer[*iPos]);
	(*iPos)++;
	return temp;
}

u2 readU2(u1* dataBuffer, int* iPos){
	u2 temp = (((u2)dataBuffer[*iPos]&0x00FF) << 8) + (u2)dataBuffer[*iPos+1];
	(*iPos)+=2;
	return temp;
}

s2 readS2(u1* dataBuffer, int* iPos){
	s2 temp = (((s2)dataBuffer[*iPos]) << 8) + (s2)dataBuffer[*iPos+1];
	(*iPos)+=2;
	return temp;
}

u4 readU4(u1* dataBuffer, int* iPos) {
	u4 temp = ((u4)(dataBuffer[*iPos])<<24)+((u4)dataBuffer[*iPos+1]<<16)+((u4)(dataBuffer[*iPos+2])<<8)+(u4)dataBuffer[*iPos+3];
	(*iPos) += 4;	
	return temp;
}

s4  readS4(u1* dataBuffer, int* iPos){
	s4 temp = ((s4)(dataBuffer[*iPos])<<24)+((s4)dataBuffer[*iPos+1]<<16)+((s4)(dataBuffer[*iPos+2])<<8)+(s4)dataBuffer[*iPos+3];
	(*iPos) += 4;	
	return temp;
}

u1 readHigh(u1 data){
	return data & 0xF0;
}

u1 readLow(u1 data){
	return data & 0x0F;
}

u1 readHighShift(u1 data){
	return (data & 0xF0)>>4;
}

u2  makeU2(u1 byte1, u1 byte2){
	return byte1*0x100+byte2;
}

s4 makeInt(s2 short1,s2 short2){
	return short1*0x10000+short2;
}

s2 getShortHigh(s4 value){
	return value/0x10000;
}

s2  getShortLow(s4 value){
	return value % 0x10000;
}


