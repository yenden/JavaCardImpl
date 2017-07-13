#include "appletsMethods.h"

//method install of the Applet
void callInstallMethod( VM *vm, CardApplet *cardApp){
    uint8_t ptr[] = "Start installing.";
    myprintf(ptr, sizeof(ptr),CHAR);
    uint16_t offset = cardApp->pApplet.applets[0].installMethodOffset;
	executeByteCode(vm, &cardApp->absApp, offset, false);
    uint8_t ptr2[] = "Install finished!";
    myprintf(ptr2, sizeof(ptr2),CHAR);
}

//process method 
void callProcessMethod(VM *vm, CardApplet *cardApp){
    uint16_t iPos = 0;
    uint8_t ptr[] = "Start processing the apdu ...";
    myprintf(ptr, sizeof(ptr),CHAR);
    JcvmValue instanceClass = vm->stackFrame[vm->frameTop].localVariables[0];
    if(instanceClass.type != refType){
        return;
    } 
    jcvm_Reference instanceClassRef = (jcvm_Reference)(instanceClass.value);   
    uint8_t arrayRef[2];
    nvmRead(instanceClassRef, arrayRef, 2);
    uint16_t classRef = readU2(arrayRef, &iPos);
    uint8_t classCount = cardApp->absApp.pDescriptor.classCount;
    uint16_t processMethodOffset = 0;

    //searching process method offset in methodComponent
    for(uint8_t i = 0; i < classCount; i++){
        if(cardApp->absApp.pDescriptor.classes[i].thisClassRef == classRef){
            ClassDescriptorInfo thisClass = cardApp->absApp.pDescriptor.classes[i];
            for(uint16_t j = 0; j < thisClass.methodCount; j++){
                if(thisClass.methods[j].token == 7){
                    processMethodOffset = thisClass.methods[j].methodOffset;
                    break;
                }
            }
            break;
        }
    } 

    //execute process method
    executeByteCode(vm, &cardApp->absApp, processMethodOffset,false);
    uint8_t ptr2[] = "Process finished!";
    myprintf(ptr2, sizeof(ptr2),CHAR);
    
}