#include "dispatcher.h"

#define MAX_APDU_LEN 20 //length of apdu in memory

MapAID_CardAPP appletTable[max_Applet];

uint8_t apduBuff[MAX_APDU_LEN];
uint8_t currentlySelectedApplet[16];
uint8_t iterate; //iteration in apdu test buffer


bool isCardInitFlag = false;

void initVM(VM *vm){
    vm->frameTop = -1;
    Frame frame;
    frame.opStackTop = -1;
    pushFrame(vm, frame);
}

/*   This function is used by the jcre to
    * retrive aid in the received apdu and select
    * the corresponding applet
*/
void selectApdu(uint8_t *apdu){
    uint8_t aidBytes[16];
    uint16_t receiveLen = t0RcvData(apdu,5,MAX_APDU_LEN);
    if(receiveLen == (uint16_t)apdu[4]){
        arrayCopy(apdu, 5, currentlySelectedApplet, 0, receiveLen);
    }
    uint8_t ptr[] = "Selecting aid";
    nadiaprintf(ptr, sizeof(ptr),CHAR);
    selectingAppletFlag = true;
}

/*Install is used by the JCRE to install 
 *the currently selected applet
*/
 void install(VM *vm){
     initVM(vm);
     callInstallMethod(vm, constantApplet);
     //nadiaprintf(&iterate,1,HEX);
 }

bool processAndForward(VM *vm){
    switch(apduBuff[OFFSET_INS]){
        case INS_SELECT : // ISO 7816-4 SELECT FIlE command
            selectApdu(apduBuff);
            break;
        case INS_INSTALL : //install command
        	install(vm);
            return false;
            break;
        default : 
            //nothing .... We don't support manage channel command
            break;
    }
    return true;
}


 //init the vm and store the reference of apdu array in its local variables.
 void initProcess(VM *vm){
    uint8_t arrayRef[2];
    uint16_t objReference;
    initVM(vm);
    
    //get the reference of the currently selected applet 
    //in the page of nvm
    for(uint8_t i =8; i < NvmSectorSize; i = i+ 24){
        uint8_t aid[16];
        nvmRead(FLASH_START_ADDRESS + i, aid, 16);
        if(deepEqual(aid, currentlySelectedApplet,16)){
            nvmRead(FLASH_START_ADDRESS + i - 8, arrayRef, 2);
            uint16_t j = 0;
            objReference = readU2(arrayRef, &j);
            break;
        }
    }
    //push class instance reference and apdu buff reference  
    JcvmValue refToput = {refType, objReference};
    vm->stackFrame[vm->frameTop].localVariables[0] = refToput;
    uint16_t apduBuffReference = FLASH_APDU_ARRAY_BASE;
    JcvmValue valueToput = {refType, apduBuffReference};
    vm->stackFrame[vm->frameTop].localVariables[1] = valueToput;
 }


/**This function inits apdu array for incoming and outgoing apdus
  *It will be called once at card initialization
*/
void initApduArrayInNvm(){
    uint8_t array[3];
    uint16_t len = MAX_APDU_LEN;
    array[0] = byteType;
    array[1] = makeU1High(len);
    array[2] = makeU1Low(len);
    nvmWrite(FLASH_APDU_ARRAY_BASE, array,3);
}

void cardInit(){
    for(uint8_t i = 0; i < 16; i++){
        currentlySelectedApplet[i] = 0;
    }
    initApduArrayInNvm();
    isCardInitFlag = true;
}


/*main receiving APDU  loop*/
void mainLoop(VM *vm){
    uint16_t sw = 0; //status word
    while(iterate <= 18){
        selectingAppletFlag = false;
        complete(apduBuff, sw);
        setStatus(0x9000);

        // dispatch to the currently selected applet
        if(processAndForward(vm)){  
            CardApplet *selectedApplet = NULL;
            for(uint8_t i = 0; i < max_Applet; i++){
                if(deepEqual(appletTable[i].aid, currentlySelectedApplet,16)){
                    selectedApplet = appletTable[i].cardApp;
                    initProcess(vm);
                    callProcessMethod(vm, selectedApplet);
                    sw = getStatus();
                    break;
                }
            }
            //if the applet is not in the JCRE's applet table
            if(selectedApplet == NULL){
                sw = SW_UNKNOWN;
            }
        } else { //install method case
            sw = getStatus();
        }
    }
    nadiaprintf("End",3,CHAR);
}

