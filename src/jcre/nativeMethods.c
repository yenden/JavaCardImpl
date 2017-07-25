#include "nativeMethods.h"
#include "../apdus.h"

static bool sendRcvCycleStarted = false;
static uint16_t sw; //status word

static uint8_t internCommand[5]; //command part of an apdu (CLA,INS,P1,P2, Lc)
uint8_t apduBuffer[BUFFER_RCV_LEN_MAX];


uint8_t Lc;
uint8_t Le;
uint8_t Lr;


//setParam set apdu parameter(Lr,Lc and Le)
void setParam(){
	Lc = internCommand[4];
	Le = 0;
	Lr = Le;
}

void sendStatus(uint16_t sw1sw2){
    uint8_t bs[2];
    bs[0] = (uint8_t)((sw1sw2 & 0xFF00)>>8);
    bs[1] = (uint8_t)(sw1sw2 & 0x00FF);

    //send status word
    arrayCopy(bs, 0, sentFromCard, 0, 2);
	
    //print
	uint8_t ptr[] = "Sending status word:";
	nadiaprintf(ptr, sizeof(ptr),CHAR);
	nadiaprintf(bs,2,HEX);
	nadiaprintf("\n",sizeof("\n"),CHAR);
}

//receive apdu
void receiveCommand(){
    arrayCopy(debit_test_APDU[iterate], 0, apduBuffer, 0, 5);
}
void receiveData(uint8_t *dataReceived, uint16_t receiveLen){
    arrayCopy(debit_test_APDU[iterate], 5, dataReceived, 0, receiveLen);
    iterate++;
}

uint16_t t0RcvCommand(uint8_t *apduHeader){
    if(sendRcvCycleStarted){
        return 65535;
    }
    sendRcvCycleStarted = true;
    receiveCommand();
    arrayCopy(apduBuffer, 0, apduHeader, 0, 5);
    arrayCopy(apduBuffer, 0, internCommand, 0, 5);

    setParam();
   return  0;
}

//send sw and wait for next apdu
uint16_t t0SndStatusRcvCommand(uint8_t *apduHeader){
    sendStatus(sw);
    receiveCommand();

    arrayCopy(apduBuffer, 0, apduHeader, 0, 5);
    arrayCopy(apduBuffer, 0, internCommand, 0, 5);
    setParam();
    return 0;
}

//retrieves data form buffer
uint16_t t0RcvData(uint8_t *apduHeader, uint16_t offset){
    uint8_t dataReceived[BUFFER_RCV_LEN_MAX];
    uint16_t receiveLen = (uint16_t)(internCommand[4]&0xFF);
    uint16_t receiveSpace = BUFFER_RCV_LEN_MAX - offset;
    if(receiveLen > receiveSpace){
        receiveLen = receiveSpace;
    }
    receiveData(dataReceived, receiveLen);
    arrayCopy(dataReceived,0,apduBuffer,offset,receiveLen);
    internCommand[4] -= (uint8_t)receiveLen;
    return receiveLen;
}

//copy to the outgoing apdu buffer
void t0SendData(uint8_t *data, uint16_t offset, uint16_t length){
    uint8_t dataToSend[BUFFER_RCV_LEN_MAX];
    arrayCopy(data,offset,dataToSend,0,length);

    uint8_t ptr[]="Sending response";
	nadiaprintf(ptr, sizeof(ptr),CHAR);
	nadiaprintf(dataToSend, length, HEX);
	//nadiaprintf("\n",sizeof("\n"),CHAR);
}

//set the status word to send
void t0SetStatus(uint16_t status){
    sw = status;
}
