#include "nativeMethods.h"
#include "../apdus.h"

uint8_t bufferRcv[BUFFER_RCV_LEN_MAX];
uint8_t bufferSnd[BUFFER_SND_LEN_MAX];

static uint16_t apduRcvPtr = 5;//It indices the received apdu buffer
uint16_t apduSendPtr; //It indices the sending apdu buffer

static bool dataWithStatusFlag = false; //data with status flag --if there is just data or data+SW
static bool sendRcvCycleStarted = false;
static uint16_t sw; //status word
static uint8_t internCommand[5]; //command part of an apdu (CLA,INS,P1,P2, Lc)


void printArray(uint8_t *display,uint16_t len){
    printf("Array\t");
    for(uint16_t i=0;i<len;i++){
        printf("%d ",display[i]);
    }
    printf("\n"); 
}

void receiveATR(){
    printf("Receive PowerUp signal \n");
	printf("Sending ATR \n");
    //TODO print ATR 
}
void powerUp(){
    receiveATR();
    printf("Card is up \n");
}

uint8_t receive(){
    apduRcvPtr = 5;
    dataWithStatusFlag = false;

    int length = sizeof(credit_test_APDU[iterate])/sizeof(uint8_t);
    arrayCopy(credit_test_APDU[iterate], 0, bufferRcv, 0, length);
    printf("\niterate %d\n", iterate);
    printArray(credit_test_APDU[iterate],14);
    iterate++;
    /*if(iterate >= APDU_TEST_BUFF_LEN){
        iterate = 0;
    }*/
    return length;
}
void sendStatus(uint16_t sw){
    uint8_t bs[2];
    bs[0] = (uint8_t)(sw>>8);
    bs[1] = (uint8_t)sw;
    //if there is data to send with SW
    if(dataWithStatusFlag){
        bufferSnd[apduSendPtr] = bs[0];
        bufferSnd[apduSendPtr + 1] = bs[1];
        arrayCopy(bufferSnd, 0, sentFromCard, 0, apduSendPtr + 2 );
    } else {
        arrayCopy(bs, 0, sentFromCard, 0, 2);
    }
}

uint16_t t0RcvCommand(uint8_t *apduCommand){
    if(sendRcvCycleStarted){
        return 65535;
    }
    sendRcvCycleStarted = true;
    uint16_t receiveLen;
    receiveLen = receive();
    arrayCopy(bufferRcv, 0, apduCommand, 0, 5);
    arrayCopy(bufferRcv, 0, internCommand, 0, 5);
   return receiveLen;
}

//send sw and wait for next apdu
uint16_t t0SndStatusRcvCommand(uint8_t *apduCommand){
    sendStatus(sw);
    uint16_t receiveLen = receive();
    arrayCopy(bufferRcv, 0, apduCommand, 0, 5);
    arrayCopy(bufferRcv, 0, internCommand, 0, 5);
    return receiveLen;
}

//retrieves data form buffer
uint16_t t0RcvData(uint8_t *apduBuff, uint16_t offset, uint16_t length){
    uint16_t receiveLen = (uint16_t)(internCommand[4]&0xFF);
    uint16_t receiveSpace = length - offset;
    if(receiveLen > receiveSpace){
        receiveLen = receiveSpace;
    }
    arrayCopy(bufferRcv,apduRcvPtr,apduBuff,offset, receiveLen);
    apduRcvPtr += receiveLen;
    internCommand[4] -= (uint8_t)receiveLen;
    return receiveLen;
}

//copy to the outgoing apdu buffer
void t0SendData(uint8_t *apduBuff, uint16_t offset, uint16_t length){
    if(!dataWithStatusFlag){
        dataWithStatusFlag = true;
    }
    arrayCopy(apduBuff,offset,bufferSnd,apduSendPtr,length);
    apduSendPtr += length;
}

//copy the content of buffer in another array
void t0CopyToApduBuffer(uint8_t *apduBuff, uint16_t len){
    arrayCopy(bufferRcv,0,apduBuff, 0, len);
}

//set the status word to send
void t0SetStatus(uint16_t status){
    sw = status;
}

//apdu get response command
uint16_t t0SndGetResponse(){
    t0SndStatusRcvCommand(internCommand);
    return ((uint16_t)(internCommand[4] & 0xFF));
}