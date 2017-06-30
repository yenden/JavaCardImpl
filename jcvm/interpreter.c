#include <stdlib.h>
#include "interpreter.h"
#include "appletsMethods.h"
#include "bytecodes.h"

static uint16_t status = 0x9000; 
Map slookupswitchMap[20];

static bool leaveVM = false;

uint16_t getStatus(){
    return status;
}
void setStatus(uint16_t st){
    status = st;
}

void interpret(VM *vm, AbstractApplet *pCA, uint16_t *pPC){
    uint8_t *pByteCode = pCA->pMethod.pMethodInfo;
    Frame *currentFrame = &(vm->stackFrame[vm->frameTop]);
    leaveVM = false;
    uint16_t value;
    jcvm_byte bValue;
    uint8_t index;
    uint16_t indexInvok;
    uint8_t byte1;
    uint16_t shortValue;
    jcvm_short deflt;
    uint16_t npairs;
    jcvm_short  match;
    jcvm_short pairs;
    uint8_t atype;
    jcvm_short constant;

    while(!leaveVM){
        uint8_t bytecode = readU1(pByteCode,pPC);
        switch (bytecode){
            case 0x0:
                //no operation
                break;
            case 0x18:
                aload(currentFrame, 0); //aload_0
                break;
            case 0x19:
                aload(currentFrame, 1); //aload_1
                break;
            case 0x1A:
                aload(currentFrame, 2); //aload_2
                break;
            case 0x1B:
                aload(currentFrame, 3); //aload_3
                break;
            case 0x2D:
			    astore(currentFrame, 2); //astore_2
                break;
	        case 0x2E:
			    astore(currentFrame, 3); //astore_3
                break;
            case 0x25:
			    baload(currentFrame);
                break;
            case 0x38:
			    bastore(currentFrame);
                break;
            case 0x10:
			    byte1 = readU1(pByteCode, pPC);
                deflt = (jcvm_short)byte1;
			    bspush(currentFrame, &deflt);
                break;
            case 0x3D:
			    dup(currentFrame);
                break;
            case 0xAD: //getfieldAthis
			    index = readU1(pByteCode, pPC);
			    getFieldThis(currentFrame, index, pCA);
                break;
            case 0x7B: //getstatic_a
                value = readU2(pByteCode, pPC);
                getstatic(currentFrame, value, pCA, 0x7B);
                break;
            case 0x7C: //getstatic_b
                value = readU2(pByteCode, pPC);
                getstatic(currentFrame, value, pCA, 0x7C);
                break;
            case 0x70:
                bValue = readS1(pByteCode, pPC);
                goTo(currentFrame, bValue, pPC);
                break;
            case 0x6A:
                bValue = readS1(pByteCode, pPC);
                ifScmpeq(currentFrame, bValue, pPC);
                break;
            case 0x6D:
                bValue = readS1(pByteCode, pPC);
                ifScmpge(currentFrame, bValue, pPC);
                break;
            case 0x6F:
                bValue = readS1(pByteCode, pPC);
                ifScmple(currentFrame, bValue, pPC);
                break;
            case 0x6C:
                bValue = readS1(pByteCode, pPC);
                ifScmplt(currentFrame, bValue, pPC);
                break;
            case 0x6B:
                bValue = readS1(pByteCode, pPC);
                ifScmpne(currentFrame, bValue, pPC);
                break;
            case 0x60:
                bValue = readS1(pByteCode, pPC);
                ifeq(currentFrame, bValue, pPC);
                break;
            case 0x61:
                bValue = readS1(pByteCode, pPC);
			    ifne(currentFrame, bValue, pPC);
                break;
            case 0x8B:
                indexInvok = readU2(pByteCode, pPC);
                invokevirtual(currentFrame, indexInvok, pCA, vm);
                break;
            case 0x8C:
                indexInvok = readU2(pByteCode, pPC);
                invokespecial(currentFrame, indexInvok, pCA, vm);
                break;
            case 0x8D:
                indexInvok = readU2(pByteCode, pPC);
                invokestatic(currentFrame, indexInvok, pCA, vm);
                break;
            case 0x8F:
                indexInvok = readU2(pByteCode, pPC);
                vmNew(currentFrame, indexInvok, pCA);
                break;
            case 0x90:
                atype = readU1(pByteCode, pPC);
			    newArray(currentFrame, atype);
                break;
            case 0x87: //putfield_a
                index = readU1(pByteCode, pPC);
                putfield(currentFrame, index, pCA);
                break;
            case 0x80: //putstatic_b
                value = readU2(pByteCode, pPC);
                putstatic(currentFrame, value, pCA, 0x80);
                break;
            case 0x7A:
                popFrame(vm);
                return;
                break;
            case 0x5B:
			    s2b(currentFrame);
                break;
            case 0x41:
			    sadd(currentFrame);
                break;
            case 0x03:
                constant = 0;
                sconst(currentFrame, &constant);
                break;
            case 0x04:
                constant = 1;
                sconst(currentFrame, &constant);
                break;
            case 0x05:
                constant = 2;
                sconst(currentFrame, &constant);
                break;
            case 0x06:
                constant = 3;
                sconst(currentFrame, &constant);
                break;
            case 0x07:
                constant = 4;
                sconst(currentFrame, &constant);
                break;
            case 0x08:
                constant = 5;
                sconst(currentFrame, &constant);
                break;
            case 0x1F:
			    sload(currentFrame, 3); //sload_3
                break;
            case 0x75:
                deflt = readS2(pByteCode, pPC);
                npairs = readU2(pByteCode, pPC);
                for (uint16_t k = 0; k < npairs; k++) {
                    match = readS2(pByteCode, pPC);
                    pairs = readS2(pByteCode, pPC);
                    slookupswitchMap[k].key = &match;
                    slookupswitchMap[k].value = &pairs;
                }
                slookupswitch(currentFrame, deflt, npairs, pPC);
                break;
            case 0x49:
                srem(currentFrame);
                break;
            case 0x11:
                shortValue = readU2(pByteCode, pPC);
                sspush(currentFrame, &shortValue);
                break;
            case 0x32: //sstore_3
			    sstore(currentFrame, 3);
                break;
            case 0x43:
			    ssub(currentFrame);
                break;
            default:
                //other bytecodes
                //nothing
                break;
        }
    }
}
bool isExtended(uint8_t flag){
    return (flag & 0x80) == 0x80;
}
bool isAbstract(uint8_t flag){
    return (flag & 0x40) == 0x40;
}

void executeByteCode( VM *vm, AbstractApplet *pCA, uint16_t offset, bool invokerCond){
    uint16_t iPos = offset - 1;
    MethodComponent methcomp = pCA->pMethod;
    uint8_t flags = readU1(methcomp.pMethodInfo, &iPos);
    Frame *currFrame = &(vm->stackFrame[vm->frameTop]);
    uint8_t nargs;
    if(isExtended(flags)){
        iPos++;
		nargs = readU1(methcomp.pMethodInfo, &iPos);
		iPos++;
    } else {
        //if abstract
		uint16_t bitField = readU1(methcomp.pMethodInfo,&iPos);
		nargs = readHighShift(bitField);
    }

	if (invokerCond == true ){
		Frame *invokerframe = &(vm->stackFrame[vm->frameTop-1]);
		for (uint8_t i = nargs; i > 0; i--) {
			currFrame->localVariables[i-1] = pop(invokerframe);
		}
	}
	interpret(vm, pCA, &iPos); //the main executing method

}