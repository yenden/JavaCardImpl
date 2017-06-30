#ifndef _vmUtil_h
#define _vmUtil_h

#include <stdio.h>
#include "util.h"

#define max_frame 5
#define max_localVar 2
#define max_opStack 6

typedef struct Map{
    void *key;
    void *value;
}Map;

extern Map slookupswitchMap[20];

enum JcvmType{byteType, shortType, refType};

typedef struct JcvmValue{
    enum JcvmType type;
    void *value;
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