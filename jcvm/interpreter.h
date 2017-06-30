#ifndef _interpreter_h
#define _interpreter_h

#include <stdbool.h>
#include "util.h"
#include "vmUtil.h"
#include "installer/cardApplet.h"

uint16_t getStatus();
void setStatus(uint16_t st);
void interpret(VM *vm, AbstractApplet *pCA, uint16_t *pPC);
void executeByteCode( VM *vm, AbstractApplet *pCA, uint16_t offset, bool invokerCond);

#endif