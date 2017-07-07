#include "bytecodes.h"

//framewor functions tokens
#define CHECK 1

#define OWNERPIN_CLASS 9
#define IS_VALIDATED 4
#define RESET_AND_UNBLOCK 6
#define UPDATE 8
#define APPLET 3

#define REGISTER 1
#define SELECTING_APPLET 3
#define UTIL 16
#define ARRAY_COPY_NON_ATOMIC 2

#define APDU 10
#define GET_BUFFER 1
#define RECEIVE_BYTES 3
#define SEND_BYTES 4
#define SEND_BYTES_LONG 5
#define SET_INCOMMING_AND_RECEIVE 6
#define SET_OUTGOING 7
#define SET_OUTGOING_AND_SEND 8
#define SET_OUTGOING_LENGTH 9

#define ISO_EXCEPTION 7
#define THROW_IT 1



uint8_t frameworkAID[] = {0xa0, 0, 0, 0, 0x62, 1, 1};

static uint16_t classInstanceCount = 0; //the address of the next class to be created
static uint16_t createdArrayCount = 0; //the address of the next array to be created
static uint16_t memRefCount = 0; //the address of the next class reference to store in nvm


bool deepEqual(uint8_t *aid1, uint8_t *aid2, uint16_t length){
    for(uint16_t i = 0; i < length; i++){
        if(aid1[i] != aid2[i]){
            return false;
        }
    }
    return true;
}
void aload(Frame *currF, uint8_t index){
    JcvmValue value = currF->localVariables[index];
    if(value.type == refType){
        push(currF, value);
    }
    
}
void astore(Frame *currF, uint8_t index){
    JcvmValue reference = pop(currF);
    if(reference.type == refType){
        currF->localVariables[index] = reference;
    }
    
}
void baload(Frame *currF){
    //pop the index value from operand stack
    jcvm_short index =  (pop(currF)).value; 
    //pop the array reference from the stack
    jcvm_Reference arrayRef = (jcvm_Reference)(pop(currF).value);

    //read array header (type and length)
    uint8_t arrayHeader[3];
    uint16_t pos = 0;
    nvmRead(arrayRef, arrayHeader, 3);
    uint8_t arrayComponentsType = readU1(arrayHeader,&pos);

    //array components are of type byte
    if(arrayComponentsType == byteType){ 
        uint16_t arrayLength = readU2(arrayHeader,&pos);
        uint8_t value;
        //uint16_t address = arrayRef + 2 + index;
        uint16_t address = arrayRef + 8 + index;
        nvmRead(address, &value, 1);
        jcvm_byte val = (jcvm_byte)value;
        JcvmValue valueTopush = {shortType, (jcvm_short)val};
        push(currF, valueTopush);
    }
}
void bastore(Frame *currF){
    jcvm_short valueToStore = (pop(currF)).value;
    //pop the index value from operand stack
    jcvm_short index =  (pop(currF)).value; 
    //pop the array reference from the stack
    jcvm_Reference arrayRef = (jcvm_Reference)(pop(currF).value);

    //read array header (type and length)
    uint8_t arrayHeader[3];
    uint16_t pos = 0;
    nvmRead(arrayRef, arrayHeader, 3);
    uint8_t arrayComponentsType = readU1(arrayHeader,&pos);

    //array components are of type byte
    if(arrayComponentsType == byteType){ 
        uint16_t arrayLength = readU2(arrayHeader,&pos);
        uint8_t value = (uint8_t)valueToStore;
        uint16_t address = arrayRef + 8 + index;
        nvmWrite(address, &value, 1);
    }
}
void bspush(Frame* currF, jcvm_byte value){
    JcvmValue valueTopush = {shortType, (jcvm_short)value};
    push(currF, valueTopush);
}

void dup(Frame *currF){
    JcvmValue interm = currF->operandStack[currF->opStackTop];
    //the value to push must be on 2 bytes (short or Ref type)
    if(interm.type == shortType || refType){
        push(currF, interm);
    }
}
void getFieldThis(Frame* currF, uint8_t index, AbstractApplet *pCA){
    //an entry into the constant pool of the package
    CpInfo pCI = pCA->pConstantPool.constantPool[index];
    //get the instance reference
    jcvm_Reference instanceRef = (jcvm_Reference)currF->localVariables[0].value;

    //parse the fields in the nvm to get the proper field.
    uint16_t startAddress = instanceRef + 8;
    uint8_t token;
    uint8_t type;
    nvmRead(startAddress, &token, 1);
    while(token != pCI.info[2]){
        startAddress += 8;
        nvmRead(startAddress, &token, 1);
    }
    nvmRead(startAddress + 1, &type, 1);

    //push the corresponding field value on the operandStack
    if (type == refType){
        uint8_t arrayValue[2];
        uint16_t iPos = 0;
        uint16_t value;
        nvmRead(startAddress + 2, arrayValue, 2);
        value = readU2(arrayValue, &iPos);
        JcvmValue valueTopush = {refType, value};
        push(currF, valueTopush);
    }
}

uint16_t getstaticFieldAddress(uint16_t index, AbstractApplet *pCA){
    CpInfo pCI = pCA->pConstantPool.constantPool[index];
    uint8_t byte1 = pCI.info[0];
    uint8_t offset = makeU2(pCI.info[1], pCI.info[2]);
    if(byte1 == 0){
        return offset;
    }
    return 65535; //it is an external static field -> error
}
void getstatic(Frame* currF, uint16_t index, AbstractApplet *pCA, uint16_t ins){
    uint16_t offset = getstaticFieldAddress(index, pCA);
    if(offset == 65535){
        return;
    }
    uint8_t ref[2];
    uint16_t iPos = 0;
    jcvm_Reference refValue;
    jcvm_byte byteValue;
    JcvmValue valueTopush;
    switch(ins){
        case 0x7B: //reference push
            nvmRead(FLASH_STATIC_IMAGE_ADDRESS + offset, ref, 2);
            refValue = readU2(ref, &iPos);
            valueTopush.type = refType;
            valueTopush.value = refValue;
            push(currF, valueTopush);
            break;
        case 0x7C: //byte push
            nvmRead(FLASH_STATIC_IMAGE_ADDRESS + offset, ref, 1);
            //byte must be extended to short
            byteValue = (jcvm_byte)ref[0];
            valueTopush.type = shortType;
            valueTopush.value = (jcvm_short)byteValue;
            push(currF, valueTopush);
            break;
        default :
            //other types (short, int)
            break;
    }
}

void goTo(Frame* currF, jcvm_byte branch, uint16_t *pPC){
    (*pPC) += (uint16_t)branch;
    (*pPC) -= 2;
}

void ifScmpeq(Frame* currF, jcvm_byte branch, uint16_t *pPC){
    jcvm_short value2 = pop(currF).value;
    jcvm_short value1 = pop(currF).value;
    if(value1 == value2){
        (*pPC) += (uint16_t)branch;
        (*pPC) -= 2;
    }
}

void ifScmpge(Frame* currF, jcvm_byte branch, uint16_t *pPC){
    jcvm_short value2 = pop(currF).value;
    jcvm_short value1 = pop(currF).value;
    if(value1 >= value2){
        (*pPC) += (uint16_t)branch;
        (*pPC) -= 2;
    }
}

void ifScmple(Frame* currF, jcvm_byte branch, uint16_t *pPC){
    jcvm_short value2 = pop(currF).value;
    jcvm_short value1 = pop(currF).value;
    if(value1 <= value2){
        (*pPC) += (uint16_t)branch;
        (*pPC) -= 2;
    }
}

void ifScmplt(Frame* currF, jcvm_byte branch, uint16_t *pPC){
    jcvm_short value2 = pop(currF).value;
    jcvm_short value1 = pop(currF).value;
    if(value1 < value2){
        (*pPC) += (uint16_t)branch;
        (*pPC) -= 2;
    }
}
void ifScmpne(Frame* currF, jcvm_byte branch, uint16_t *pPC){
    jcvm_short value2 = pop(currF).value;
    jcvm_short value1 = pop(currF).value;
    if(value1 != value2){
        (*pPC) += (uint16_t)branch;
        (*pPC) -= 2;
    }
}

void ifeq(Frame* currF, jcvm_byte branch, uint16_t *pPC){
    jcvm_short value =(pop(currF)).value;
    if(value == 0){
        (*pPC) += (uint16_t)branch;
        (*pPC) -= 2;
    }
}
void ifne(Frame* currF, jcvm_byte branch, uint16_t *pPC){
    jcvm_short value = pop(currF).value;
    if(value != 0){
        (*pPC) += (uint16_t)branch;
        (*pPC) -= 2;
    }
}
void ifge(Frame* currF, jcvm_byte branch, uint16_t *pPC){
    jcvm_short value = pop(currF).value;
    if(value >= 0){
        (*pPC) += (uint16_t)branch;
        (*pPC) -= 2;
    }
}


void putfield(Frame* currF, uint8_t index, AbstractApplet *pCA){
    jcvm_short svalue = pop(currF).value;
    jcvm_Reference instanceRef = (jcvm_Reference)(pop(currF).value);
    CpInfo pCI = pCA->pConstantPool.constantPool[index];
    
    //parse the fields in the nvm to get the proper field.
    uint16_t startAddress = instanceRef + 8;
    uint8_t token;
    uint8_t type;

    nvmRead(startAddress, &token, 1);
    nvmRead(startAddress + 1, &type, 1);
    while(token != pCI.info[2]){
        startAddress += 8;
        nvmRead(startAddress, &token, 1);
        nvmRead(startAddress + 1, &type, 1);
    }
    //put the corresponding field value in the field
    if (type == refType){
        uint8_t arrayValue[2];
        arrayValue[0] = makeU1High((uint16_t)svalue);
        arrayValue[1] = makeU1Low((uint16_t)svalue);
        nvmWrite(startAddress + 2, arrayValue, 2);
    }
    //TODO other types
}

void putstatic(Frame* currF, uint8_t index, AbstractApplet *pCA, uint16_t ins){
    uint16_t offset = getstaticFieldAddress(index, pCA);

    if(ins == 0x80){ //putstatic byte
        jcvm_short svalue = pop(currF).value;
        uint8_t valueToPut = (uint8_t)svalue;
        nvmWrite(FLASH_STATIC_IMAGE_ADDRESS + offset, &valueToPut, 1);
    }
}

void s2b(Frame* currF){
    jcvm_short svalue = pop(currF).value;
    jcvm_byte byteValue = (jcvm_byte)svalue;
    JcvmValue valueTopush = {shortType, (jcvm_short)byteValue};
    push(currF, valueTopush);
}
void sadd(Frame* currF){
    jcvm_short svalue1 = pop(currF).value;
    jcvm_short svalue2 = pop(currF).value;
    jcvm_short result = svalue1 + svalue2;
    JcvmValue valueTopush = {shortType, result};
    push(currF, valueTopush);
}

void sload(Frame* currF, uint8_t index){
    JcvmValue value = currF->localVariables[index];
    push(currF, value);
}
void srem(Frame* currF){
    jcvm_short svalue1 = pop(currF).value;
    jcvm_short svalue2 = pop(currF).value;
    jcvm_short result = svalue1 - (svalue1 / svalue2) * svalue2;
    JcvmValue valueTopush = {shortType, result};
    push(currF, valueTopush);

}
void ssub(Frame* currF){
    jcvm_short svalue1 = pop(currF).value;
    jcvm_short svalue2 = pop(currF).value;
    jcvm_short result = svalue1 - svalue2;
    JcvmValue valueTopush = {shortType, result};
    push(currF, valueTopush);
}

void sspush(Frame *currF, jcvm_short value){ 
    JcvmValue valueTopush = {shortType, value};
    push(currF, valueTopush);
}
void sconst(Frame *currF, jcvm_short value){
    JcvmValue valueTopush = {shortType, value};
    push(currF, valueTopush);
}
void sstore(Frame *currF, uint8_t index){
    JcvmValue valueToPut = pop(currF);
    currF->localVariables[index] = valueToPut;
}
void slookupswitch(Frame* currF, uint16_t deflt, uint16_t npairs, uint16_t *pPC){
    jcvm_short keyFromStack = pop(currF).value;
    jcvm_short keyFromMap;
    jcvm_short offset;
    bool isPresent = false;
    for(uint16_t k = 0; k < npairs; k++){
        keyFromMap = slookupswitchMap[k].key;
        if(keyFromMap == keyFromStack){
            isPresent = true;
            offset = slookupswitchMap[k].value;
            break;
        }
    }
    if(isPresent){
        (*pPC) += (uint16_t)offset;
    } else {
        (*pPC) += (uint16_t)deflt;
    }
    (*pPC) = (*pPC) - (uint16_t)(5+4*npairs);
}



void createClassInstance(Frame* currF, AbstractApplet *pCA, uint8_t classToken, uint16_t offset){
    ClassDescriptorInfo classInf;
    for(uint8_t i = 0; i < pCA->pDescriptor.classCount; i++){
        if(pCA->pDescriptor.classes[i].token == classToken){
            classInf = pCA->pDescriptor.classes[i];
            break;
        }
    }
    //found the number of non static fields (instance fields)
    uint8_t instanceFieldsCount = 0;
    for(uint16_t i = 0; i < classInf.fieldCount; i++){
        if((classInf.fields[i].pAF&ACC_STATIC) != ACC_STATIC){
            instanceFieldsCount++;
        }
    }

    //array which represent the class to store in nvm
    uint8_t javaClass[instanceFieldsCount*8 + 8];
    
    // fill the array 
    uint16_t superClassRef = pCA->pClass.pClasses[offset - 2].superClassRef;
    uint16_t classRef = classInf.thisClassRef;
    uint8_t declaredInstanceSize = pCA->pClass.pClasses[offset - 2].declaredInstanceSize;
    javaClass[0] = makeU1High(classRef);
    javaClass[1] = makeU1Low(classRef);
    javaClass[2] = makeU1High(superClassRef);
    javaClass[3] = makeU1Low(superClassRef);
    javaClass[4] = declaredInstanceSize;
    javaClass[5] = instanceFieldsCount;

    //initialize fields with their default number in the array
    uint8_t j = 8;
    for(uint8_t i = 0; i < classInf.fieldCount; i++){
        if((classInf.fields[i].pAF&ACC_STATIC) != ACC_STATIC){
            javaClass[j] = classInf.fields[i].token;
            switch(classInf.fields[i].pFieldType){
                case 0x8002: //bool
                    javaClass[j+1] = (uint8_t)byteType; //field type 0 for byte and bool
                    javaClass[j+2] = (uint8_t)0;
                    break;
                case 0x8003: //byte
                    javaClass[j+1] = (uint8_t)byteType; //fieldtype
                    javaClass[j+2] = (uint8_t)0;
                    break;
                case 0x8004: //short
                    javaClass[j+1] = (uint8_t)shortType; //fieldtype
                    javaClass[j+2] = (uint8_t)0;
                    javaClass[j+3] = (uint8_t)0;
                    break;
                case 0x8005: //int
                   //does not support this
                    break;
                default : //reference type
                    javaClass[j+1] = (uint8_t)refType; //fieldtype
                    javaClass[j+2] = (uint8_t)0;
                    javaClass[j+3] = (uint8_t)0;
                    break;
            }
            j += 8;    
        }    
    }
    uint16_t javaClassAddress = FLASH_JAVA_INSTANCES_BASE + classInstanceCount;
    nvmWrite(javaClassAddress, javaClass, instanceFieldsCount*8 + 8);
    JcvmValue valueTopush = {refType, javaClassAddress};
    push(currF, valueTopush);
}

void writeInstanceReferenceInMemory(AbstractApplet *pCA){
    uint8_t array[24];
    uint16_t instanceRef = FLASH_JAVA_INSTANCES_BASE + classInstanceCount;

    PackageInfo pckg = pCA->pHeader.pThisPackage;
   uint8_t aidLength = pckg.AIDLength;

    array[0] = makeU1High(instanceRef);
    array[1] = makeU1Low(instanceRef);
    array[2] = 16;//aidLength;
   // printf("aid %d", aidLength);
    uint8_t k = 8;
    for(uint8_t i = 0; i < aidLength ; i++){
        array[k] = pckg.AID[i];
        k++;
    }
     for(uint8_t i = aidLength; i < 16 ; i++){
         array[k] = 0;
         k++;
     }

    nvmWrite(FLASH_START_ADDRESS + memRefCount, array, 24);
    memRefCount += 24;
    classInstanceCount += NvmSectorSize;
}


void newArray(Frame* currF, uint8_t atype){
    jcvm_short count = pop(currF).value;
    uint8_t maxLength = NvmSectorSize - 8; // the first byte for type and length
    uint16_t length;
    uint8_t array[maxLength];
    switch(atype){
        case 10:
            //boolean
            array[0] = byteType;
            array[1] = makeU1High((uint16_t)count);
            array[2] = makeU1Low((uint16_t)count);
            length = (uint16_t)count;
            for(uint16_t i = 8; i < 8 + length ; i++){
                array[i] = 0;
            }
            break;
        case 11:
            //byte
            array[0] = byteType;
            array[1] = makeU1High((uint16_t)count);
            array[2] = makeU1Low((uint16_t)count);
            length = (uint16_t)count;
            for(uint16_t i = 8; i < 8 + length; i++){
                array[i] = 0;
            }
            break;
        case 12:
            //short
            array[0] = shortType;
            array[1] = makeU1High((uint16_t)count);
            array[2] = makeU1Low((uint16_t)count);
            length = 2*((uint16_t)count);
            for(uint16_t i = 8; i < 8 + length; i++){
                array[i] = 0;
            }
            break;
        case 13:
            //int
            break;
    }
    uint16_t arrayAddress = createdArrayCount + FLASH_ARRAY_BASE;
    nvmWrite(arrayAddress, array, length);
    JcvmValue valueTopush = {refType, arrayAddress};
    push(currF, valueTopush);
    createdArrayCount += NvmSectorSize;
}

void callSelectingApplet(Frame *currF){
    JcvmValue val = pop(currF); //not useful but need to pop it
    JcvmValue valueTopush;
    valueTopush.type = shortType;
    jcvm_short value;
    if(selectingAppletFlag == true){
        value = 1; 
    } else{
        value = 0;
    }
    valueTopush.value = value;
    push(currF, valueTopush);
}

void callOwnerPINInit(Frame *currF){
    uint8_t maxPINSize = (uint8_t)(pop(currF).value);
    uint8_t tryLimit = (uint8_t)(pop(currF).value);
    jcvm_Reference ownerPinRef = (jcvm_Reference)(pop(currF).value);
    initOwnerPin(ownerPinRef, tryLimit, maxPINSize);
}
void callOwnerPINCheck(Frame *currF){
    uint8_t length = (uint8_t)(pop(currF).value);
    uint16_t offset = (uint8_t)(pop(currF).value);
    jcvm_Reference arrayRef = (jcvm_Reference)(pop(currF).value);
    jcvm_Reference ownerPINRef = (jcvm_Reference)(pop(currF).value);

    uint16_t iPos = 0;
    uint8_t Len[2];
    nvmRead(arrayRef + 1, Len, 2);
    uint16_t arrayLen = readU2(Len, &iPos);
    uint8_t array[arrayLen];

    nvmRead(arrayRef + 8, array, arrayLen);
    bool cond = check(ownerPINRef, array, offset, length);
    jcvm_short value;
    if(cond){
        value = 1;
    } else {
       value = 0;
    }
    JcvmValue valueTopush = {shortType, value};
    push(currF, valueTopush);
}
void callIsValidated(Frame *currF){
    jcvm_Reference ownerPINRef = (jcvm_Reference)(pop(currF).value);
    jcvm_short value;
    if(isValidated(ownerPINRef)){
        value = 1;
    } else {
        value = 0;
    }
    JcvmValue valueTopush = {shortType, value};
    push(currF, valueTopush);
}
void callResetAndUnblock(Frame *currF){
     jcvm_Reference ownerPINRef = (jcvm_Reference)(pop(currF).value);
     resetAndUnblock(ownerPINRef);
}
void callUpdateOwnerPIN(Frame *currF){
    uint8_t length = (uint8_t)(pop(currF).value);
    uint16_t offset = (uint8_t)(pop(currF).value);
    jcvm_Reference arrayRef = (jcvm_Reference)(pop(currF).value);
    jcvm_Reference ownerPINRef = (jcvm_Reference)(pop(currF).value);
    
    uint16_t iPos = 0;
    uint8_t Len[2];
    nvmRead(arrayRef + 1, Len, 2);
    uint16_t arrayLen = readU2(Len, &iPos);
    uint8_t array[arrayLen];

    nvmRead(arrayRef + 8, array, arrayLen);
    uint8_t n = update(ownerPINRef, array, offset, length);
    if(n == 1 ){ //illegal value
        setStatus(n);
        leaveVM = true;
    }
}
void callRegister(Frame *currF){
    uint16_t iPos =0;
    uint8_t numb = 0;
    jcvm_Reference ref = (jcvm_Reference)(pop(currF).value);
    for(uint8_t i = 0; i < 256; i = i + 24){
        uint8_t arrayRef[2];
        nvmRead(FLASH_START_ADDRESS + i, arrayRef, 2);
        jcvm_Reference refFromMem = readU2(arrayRef, &iPos);
        if(refFromMem == ref){
            uint8_t aid[16];
            nvmRead(FLASH_START_ADDRESS + i + 8, aid, 16);
            arrayCopy(aid, 0, appletTable[numb].aid, 0, 16);
            appletTable[numb].cardApp = constantApplet;
            return;
        }
        numb++;
    }
}
void callGetBuffer(Frame *currF){
    jcvm_Reference arrayRef = (jcvm_Reference)(pop(currF).value);
    uint16_t iPos = 0;
    uint8_t Len[2];
    nvmRead(arrayRef + 1, Len, 2);
    uint16_t arrayLen = readU2(Len, &iPos);
    uint8_t array[arrayLen];

    getBuffer(array, arrayLen);
    nvmWrite(arrayRef + 8, array, arrayLen);
    JcvmValue valueTopush = {refType, (jcvm_short)arrayRef};
    push(currF, valueTopush);
}
void callThrowIt(Frame *currF){
    jcvm_short status = pop(currF).value;
    setStatus((uint16_t)status);
    leaveVM = true;
}
void callReceivebytes(Frame *currF){
    //Only APDUs case 3 and 4 are expected to call this method.
    uint16_t offset = (uint8_t)(pop(currF).value);
    jcvm_Reference arrayRef = (jcvm_Reference)(pop(currF).value);
    
    uint16_t iPos = 0;
    uint8_t Len[2];
    nvmRead(arrayRef + 1, Len, 2);
    uint16_t arrayLen = readU2(Len, &iPos);
    uint8_t array[arrayLen];
    uint16_t receiveLen = receiveBytes(array, offset, arrayLen);
    nvmWrite(arrayRef + 8, array, arrayLen);
    JcvmValue valueTopush = {shortType, (jcvm_short)receiveLen};
    push(currF, valueTopush);
}
void callSetIncommingAndReceive(Frame *currF){
    jcvm_Reference arrayRef = (jcvm_Reference)(pop(currF).value);
    uint16_t iPos = 0;
    uint8_t Len[2];
    nvmRead(arrayRef + 1, Len, 2);
    uint16_t arrayLen = readU2(Len, &iPos);
    uint8_t array[arrayLen];

    uint16_t receiveLen = setIncomingAndReceive(array, arrayLen);
    nvmWrite(arrayRef + 8, array, arrayLen);

    JcvmValue valueTopush = {shortType, (jcvm_short)receiveLen};
    push(currF, valueTopush);
}

void callSetOutgoing(Frame *currF){
    jcvm_Reference arrayRef = (jcvm_Reference)(pop(currF).value);
    uint8_t expLen = setOutgoing();
    JcvmValue valueTopush = {shortType, (jcvm_short)((jcvm_byte)expLen)};
    push(currF, valueTopush);
}

void callSetOutgoingLength(Frame *currF){
    jcvm_short len = pop(currF).value;
    pop(currF);
    setOutgoingLength((uint16_t)len);
}
void callSendBytesLong(Frame *currF){
    uint16_t len = (uint16_t)(pop(currF).value);
    uint16_t bOff = (uint16_t)(pop(currF).value);
    jcvm_Reference arrayRef = (jcvm_Reference)(pop(currF).value);
    jcvm_Reference apduArrayRef = (jcvm_Reference)(pop(currF).value);
    
    uint16_t iPos = 0;
    uint8_t Len[2];
    nvmRead(arrayRef + 1, Len, 2);
    uint16_t arrayLen = readU2(Len, &iPos);
    uint8_t outData[arrayLen];
    nvmRead(arrayRef + 8, outData, arrayLen);

    iPos = 0;
    nvmRead(apduArrayRef + 1, Len, 2);
    uint16_t apduArrayLen = readU2(Len, &iPos);
    uint8_t apduArray[apduArrayLen];
    nvmRead(apduArrayRef + 8, apduArray, apduArrayLen);

    sendBytesLong(len, bOff, outData, apduArrayLen, apduArray);
}

void callSetOutgoingAndSend(Frame *currF){
    uint16_t len = (uint16_t)(pop(currF).value);
    uint16_t bOff = (uint16_t)(pop(currF).value);
    jcvm_Reference apduArrayRef = (jcvm_Reference)(pop(currF).value);
    
    uint16_t iPos = 0;
    uint8_t Len[2];
    nvmRead(apduArrayRef + 1, Len, 2);
    uint16_t apduArrayLen = readU2(Len, &iPos);
    uint8_t apduArray[apduArrayLen];
    nvmRead(apduArrayRef + 8, apduArray, apduArrayLen);

    setOutgoingAndSend(apduArray, len, bOff);

}

void callFrameworkMethods(Frame *currF, uint8_t classToken, uint8_t methodToken){

    switch(classToken){
        case APPLET:
            if(methodToken == REGISTER){
                callRegister(currF);
            } else if(methodToken == SELECTING_APPLET){
                callSelectingApplet(currF);
            } else if(methodToken == 0){ //init
                pop(currF);
            }
            break;
        case APDU:
            switch(methodToken){
                case GET_BUFFER:
                    callGetBuffer(currF);
                    break;
                case RECEIVE_BYTES:
                    callReceivebytes(currF);
                    break;
                case SEND_BYTES:
                    //callSendBytes(&currF);
                    break;
                case SEND_BYTES_LONG:
                    callSendBytesLong(currF);
                    break;
                case SET_INCOMMING_AND_RECEIVE:
                    callSetIncommingAndReceive(currF);
                    break;
                case SET_OUTGOING:
                    callSetOutgoing(currF);
                    break;
                case SET_OUTGOING_AND_SEND:
                    callSetOutgoingAndSend(currF);
                    break;
                case SET_OUTGOING_LENGTH:
                    callSetOutgoingLength(currF);
                    break;
                default:
                    //nothing
                    break;
            }
            break;
        case OWNERPIN_CLASS :
            switch(methodToken){
                case 0: //init
                    callOwnerPINInit(currF);
                    break;
                case CHECK:
                    callOwnerPINCheck(currF);
                    break;
                case IS_VALIDATED:
                    callIsValidated(currF);
                    break;
                case RESET_AND_UNBLOCK:
                    callResetAndUnblock(currF);
                    break;
                case UPDATE:
                    callUpdateOwnerPIN(currF);
            }
            break;
        case ISO_EXCEPTION:
            if(methodToken == THROW_IT){
                callThrowIt(currF);
            }
            break;
    }

}

void createFrameworkClass(Frame *currF, uint8_t classToken){
    static uint8_t ownNum = 0;
    uint8_t array[24];
    JcvmValue valueTopush;
    jcvm_Reference ref = FLASH_JAVA_INSTANCES_BASE + classInstanceCount;
    switch(classToken){
        case OWNERPIN_CLASS:
            array[0] = makeU1High(ref);
            array[1] = makeU1Low(ref);
            array[2] = 16;
            arrayCopy(frameworkAID, 0, array, 8, 7);
            array[15] = ownNum;

            nvmWrite(FLASH_START_ADDRESS + memRefCount, array, 24);

            valueTopush.type = refType;
            valueTopush.value = ref;
            push(currF, valueTopush);
            break;
    }
    ownNum++;
    memRefCount += 24;
    classInstanceCount += NvmSectorSize;
}




void invokevirtual(Frame* currF, uint16_t index, AbstractApplet *pCA, VM *vm){
    CpInfo pCI = pCA->pConstantPool.constantPool[index];
    uint8_t byte1 = pCI.info[0];

    if ((byte1&0x80) == 0x80){ //external package
        uint8_t packageIndex = byte1 & 0x7F;
        PackageInfo pPI = pCA->pImport.packages[packageIndex];
        uint8_t length = sizeof(frameworkAID) /sizeof(uint8_t);
        
        //If it is a call to package FrameWork
        if (deepEqual(pPI.AID, frameworkAID, length)){
            uint8_t classToken = pCI.info[1];
            uint8_t token = pCI.info[2];

            //call appropriate virtual method
            callFrameworkMethods(currF, classToken, token);
        } else {
            printf("Error: cannot invoke virtual package not found\n");
        }
    } else {
        //It is an internal class library
        uint16_t offset = makeU2(pCI.info[0], pCI.info[1]);
        uint8_t token = pCI.info[2];
        ClassInfo pcLInf = pCA->pClass.pClasses[offset - 2];
        Frame newFrame;
        newFrame.opStackTop = -1;
        pushFrame(vm, newFrame);
        uint8_t index;
        if((token&0x80) == 0x80){ //call private method
            index = token - pcLInf.packageMethodTableBase;
            executeByteCode(vm, pCA, pcLInf.packageMethodTable[index], true);
        } else { 
            //call public method
            index = token - pcLInf.publicMethodTableBase;
            executeByteCode(vm, pCA, pcLInf.publicMethodTable[index], true);
        }
    }
}

void invokespecial(Frame* currF, uint16_t index, AbstractApplet *pCA, VM *vm){
    CpInfo pCI = pCA->pConstantPool.constantPool[index];
    uint8_t byte1 = pCI.info[0];
    if (pCI.tag == 0x06){ //call of a static method
        if (byte1 == 0x00){ //internal package method
            uint16_t offset = makeU2(pCI.info[1], pCI.info[2]);
            Frame newFrame;
            newFrame.opStackTop = -1;
            pushFrame(vm, newFrame);
            executeByteCode(vm, pCA, offset, true);
        } else {
            //external package
            uint8_t packageIndex = byte1&0x7F;
            PackageInfo pPI = pCA->pImport.packages[packageIndex];
            uint8_t length = sizeof(frameworkAID) /sizeof(uint8_t);
            //If it is a call to package FrameWork
            if (deepEqual(pPI.AID, frameworkAID, length)){
                uint8_t classToken = pCI.info[1];
                uint8_t token = pCI.info[2];

                //call appropriate virtual method
                callFrameworkMethods(currF, classToken, token);
            } else {
                printf("Error: cannot invoke special method, package not found\n");
            }
        }

    }
    if (pCI.tag == 0x04){
        //super method call
        invokevirtual(currF, index, pCA, vm);
    }
}
void invokestatic(Frame* currF, uint16_t index, AbstractApplet *pCA, VM *vm){
    invokespecial(currF, index, pCA, vm);
}
void vmNew(Frame* currF, uint16_t index, AbstractApplet *pCA){
    CpInfo pCI = pCA->pConstantPool.constantPool[index];
    uint8_t byte1 = pCI.info[0];
    if ((byte1&0x80) == 0x80){ //external library
        uint8_t packageIndex = byte1 & 0x7F;
        PackageInfo pPI = pCA->pImport.packages[packageIndex];
        uint8_t length = sizeof(frameworkAID) /sizeof(uint8_t);
        
        //If it is a call to package FrameWork
        if (deepEqual(pPI.AID, frameworkAID, length)){
            uint8_t classToken = pCI.info[1];
            createFrameworkClass(currF, classToken);
            return;
        }
        printf("Error: cannot create class package not found");
    } else {
        //It is an internal class library
        uint16_t offset = makeU2(pCI.info[0], pCI.info[1]);
        uint8_t token = pCI.info[2];
        createClassInstance(currF, pCA, token, offset);
    }
    writeInstanceReferenceInMemory(pCA);
}
