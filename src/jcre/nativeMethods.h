#ifndef _nativeMethods_h
#define _nativeMethods_h

#include <stdbool.h>
#include "api/utilApi.h"
#include "../client.h"
#include"../nadiaprintf.h"


#define BUFFER_RCV_LEN_MAX 20 //must be 255
//#define BUFFER_SND_LEN_MAX 25 //must be 255

extern uint8_t Lc; //Lc is the data Length send in the apdu
extern uint8_t Le; //Le expected Length in the response
extern uint8_t Lr; //Lr apdu response Length

extern uint8_t apduBuffer[BUFFER_RCV_LEN_MAX];

/*extern uint16_t apduSendPtr;
extern uint8_t bufferRcv[BUFFER_RCV_LEN_MAX];*/
extern uint8_t iterate;

void receiveATR();
void powerUp();
uint16_t t0RcvCommand(uint8_t *apduCommand);
uint16_t t0SndStatusRcvCommand(uint8_t *apduCommand);
uint16_t t0RcvData(uint8_t *apduHeader, uint16_t offset);
void t0SendData(uint8_t *apduBuff, uint16_t offset, uint16_t length);
//void t0CopyToApduBuffer(uint8_t *apduBuff, uint16_t len);
//uint16_t t0SndGetResponse();
void t0SetStatus(uint16_t status);


#endif
