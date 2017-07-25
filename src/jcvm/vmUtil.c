#include "vmUtil.h"
#include "../nadiaprintf.h"

#define null 0

void push(Frame *frame, JcvmValue value){
    if (frame->opStackTop + 1 == max_opStack) {
        return ;
    }
    frame->opStackTop++;
    frame->operandStack[frame->opStackTop] = value;
}

void pushFrame(VM *vm, Frame frame){
    if(vm->frameTop+1 == max_frame){
        return;
    }
    vm->frameTop++;
    vm->stackFrame[vm->frameTop] = frame;
}

JcvmValue pop(Frame *frame){
    JcvmValue value;
    if(frame->opStackTop == -1){
        return value ;
    }
    value = frame->operandStack[frame->opStackTop];
    frame->opStackTop--;
    return value;
}

Frame* popFrame(VM *vm){
    if(vm->frameTop == -1){
        return null;
    }
    uint8_t last = vm->frameTop;
    vm->frameTop--;
    return &(vm->stackFrame[last]);
}

