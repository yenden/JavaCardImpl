#ifndef _apdu_h
#define _apdu_h

#include <stdbool.h>
#include <stdint.h>

extern bool selectingAppletFlag;

void complete(uint8_t *apduBuff, uint16_t status);
bool getSelectingAppletFlag();
uint16_t send61xx(uint16_t length);
void sendBytesLong(uint16_t len, uint16_t bOff, uint8_t *outData, uint16_t bufLen, uint8_t *apduBuff);
void setOutgoingAndSend(uint8_t *array, uint16_t len, uint16_t bOff);
uint16_t receiveBytes(uint8_t *array, uint16_t offset, uint16_t len);
uint16_t setIncomingAndReceive(uint8_t *array, uint16_t len);
uint8_t setOutgoing();
void setOutgoingLength(uint16_t length);
void getBuffer(uint8_t *apduArr, uint16_t len);

#endif