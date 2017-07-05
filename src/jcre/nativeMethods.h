#ifndef _nativeMethods_h
#define _nativeMethods_h

#include <stdbool.h>
#include <stdio.h>
#include "api/utilApi.h"
#include "../client.h"



#define BUFFER_RCV_LEN_MAX 25 //must be 255
#define BUFFER_SND_LEN_MAX 25 //must be 255

extern uint16_t apduSendPtr;
extern uint8_t bufferRcv[BUFFER_RCV_LEN_MAX];

void receiveATR();
void powerUp();
uint16_t t0RcvCommand(uint8_t *apduCommand);
uint16_t t0SndStatusRcvCommand(uint8_t *apduCommand);
uint16_t t0RcvData(uint8_t *apduBuff, uint16_t offset, uint16_t length);
void t0SendData(uint8_t *apduBuff, uint16_t offset, uint16_t length);
void t0CopyToApduBuffer(uint8_t *apduBuff, uint16_t len);
uint16_t t0SndGetResponse();
void t0SetStatus(uint16_t status);


#endif