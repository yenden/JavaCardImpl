#include "appletsMethods.h"

//method install of the Applet
void callInstallMethod( VM *vm, CardApplet *cardApp){
    printf("Start installing \n");
    uint16_t offset = cardApp->pApplet.applets[0].installMethodOffset;
	executeByteCode(vm, &cardApp->absApp, offset, false);
	printf("Install finished!\n");
}

//process method 
void callProcessMethod(VM *vm, CardApplet *cardApp){
    uint16_t iPos = 0;
    printf("Start processing the apdu ...");
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
    printf("Process finished!\n");
    
}