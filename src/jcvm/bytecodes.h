#ifndef _bytecodes_h
#define _bytecodes_h

#include "../nvm/nvm.h"
#include "../addresses.h"
#include "../jcre/api/apdu.h"
#include "../jcre/api/ownerpin.h"
#include "appletsMethods.h"


void aload(Frame *currF, uint8_t index);
void astore(Frame* currF, uint8_t index);
void baload(Frame *currF);
void bastore(Frame *currF);
void bspush(Frame* currF, jcvm_byte value);
void dup(Frame *currF);

void getFieldThis(Frame* currF, uint8_t index, AbstractApplet *pCA);
void getstatic(Frame* currF, uint16_t index, AbstractApplet *pCA, uint16_t ins);
void goTo(Frame* currF, jcvm_byte branch, uint16_t *pPC);

void ifScmpeq(Frame* currF, jcvm_byte branch, uint16_t *pPC);
void ifScmpge(Frame* currF, jcvm_byte branch, uint16_t *pPC);
void ifScmple(Frame* currF, jcvm_byte branch, uint16_t *pPC);
void ifScmplt(Frame* currF, jcvm_byte branch, uint16_t *pPC);
void ifScmpne(Frame* currF, jcvm_byte branch, uint16_t *pPC);
void ifeq(Frame* currF, jcvm_byte branch, uint16_t *pPC);
void ifne(Frame* currF, jcvm_byte branch, uint16_t *pPC);

void invokevirtual(Frame* currF, uint16_t index, AbstractApplet *pCA, VM *vm);
void invokespecial(Frame* currF, uint16_t index, AbstractApplet *pCA, VM *vm);
void invokestatic(Frame* currF, uint16_t index, AbstractApplet *pCA, VM *vm);
void vmNew(Frame* currF, uint16_t index, AbstractApplet *pCA);
void newArray(Frame* currF, uint8_t atype);
void putfield(Frame* currF, uint8_t index, AbstractApplet *pCA);
void putstatic(Frame* currF, uint8_t index, AbstractApplet *pCA, uint16_t ins);

void s2b(Frame* currF);
void sadd(Frame* currF);
void sload(Frame* currF, uint8_t index);
void srem(Frame* currF);
void ssub(Frame* currF);

void slookupswitch(Frame* currF, uint16_t deflt, uint16_t npairs, uint16_t *pPC);

void sspush(Frame *currF, jcvm_short value);
void sconst(Frame *currF, jcvm_short value);
void sstore(Frame *currF, uint8_t value);

bool deepEqual(uint8_t *aid1, uint8_t *aid2, uint16_t length);

#endif