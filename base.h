#ifndef _base_h
#define _base_h

typedef unsigned char u1; //for byte
typedef unsigned short u2;
typedef unsigned int u4;

typedef signed char s1;
typedef signed short s2;
typedef signed int s4;

u1 readU1(u1* dataBuffer, int* iPos);
s1 readS1(u1* dataBuffer, int* iPos);
u2 readU2(u1* dataBuffer, int* iPos);
s2 readS2(u1* dataBuffer, int* iPos);
u4 readU4(u1* dataBuffer, int* iPos);
s4  readS4(u1* dataBuffer, int* iPos);
u1 readHigh(u1 data);
u1 readLow(u1 data);
u1 readHighShift(u1 data);
u2  makeU2(u1 byte1, u1 byte2);
s4 makeInt(s2 short1,s2 short2);
s2 getShortHigh(s4 value);
s2  getShortLow(s4 value);
#endif