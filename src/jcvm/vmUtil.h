#ifndef _vmUtil_h
#define _vmUtil_h

#include <stdio.h>
#include "util.h"

#define max_frame 5
#define max_localVar 6
#define max_opStack 6

typedef struct Map{
    jcvm_short key;
    jcvm_short value;
}Map;

extern Map slookupswitchMap[20];

enum JcvmType{byteType, shortType, refType};

typedef struct JcvmValue{
    enum JcvmType type;
    jcvm_short value;
}JcvmValue;

typedef struct Frame{
    JcvmValue localVariables[max_localVar];
    JcvmValue operandStack[max_opStack];
    jcvm_short opStackTop;
}Frame;

typedef struct VM {
    jcvm_short frameTop;
    Frame stackFrame[max_frame];
}VM;


void push(Frame *frame, JcvmValue value);
void pushFrame(VM *vm, Frame frame);
JcvmValue pop(Frame *frame);
Frame* popFrame(VM *vm);


#endif