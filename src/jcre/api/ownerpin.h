#ifndef _ownerpin_h
#define _ownerpin_h

#include <stdbool.h>
#include <stdint.h>

uint16_t update(uint16_t ownerpinAddress, uint8_t *pin, uint16_t offset, uint8_t  length);
bool check(uint16_t ownerpinAddress, uint8_t *pin, uint16_t offset, uint8_t length);
bool isValidated(uint16_t ownerpinAddress);
uint16_t initOwnerPin(uint16_t ownerpinAddress, uint8_t tryLimit, uint8_t maxPINSize);

#endif