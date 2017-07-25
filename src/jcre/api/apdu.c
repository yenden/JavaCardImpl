#include "apdu.h"


#define BUFFERSIZE 20 //max size of apdu buffer stored in memory
                      // this is not the size of the received apdu

static uint8_t apdu[5];
static bool sendInProgressFlag = false; 


bool selectingAppletFlag = false;


//Complete responds to the previous adpu and get next apdu
void complete(uint8_t *apduHeader, uint16_t status){
    uint8_t result;
    arrayfillNonAtomic(apduHeader, 0, BUFFERSIZE, 0);

    if(status == 0){ //first received APDU
        result = t0RcvCommand(apduHeader);
    } else {
        
        t0SetStatus(status); //set status word
        result = t0SndStatusRcvCommand(apduHeader); //send status and wait for the next
    }
 
    if (result != 0){
        uint8_t ptr[] ="imput/output error in compLete method\n";
        nadiaprintf(ptr, sizeof(ptr),CHAR);
    }
    arrayCopy(apduHeader, 0,apdu,0 ,5);
}

/*GetSelectingAppLetFlag*/
bool getSelectingAppletFlag(){
    return selectingAppletFlag;
}


/*SendBytes api framework function*/
void sendBytes(uint8_t *array, uint16_t bOff, uint16_t length){
    uint16_t len = length;
    uint16_t offset = bOff;
    t0SendData(array, offset, len);
    Lr -= (uint8_t)len;
	Le = Lr;
}

/*SendBytesLong : api function to send a long buffer*/
void sendBytesLong(uint16_t len, uint16_t bOff, uint8_t *outData, uint16_t bufLen){
    uint16_t length = len;
    uint16_t offset = bOff;
    //TODO checkArrayArgs(outData, bOff, length);
    uint16_t sendLen = bufLen;
    while(length > 0) {
        if(length < sendLen) {
            sendLen = length;
        }
        sendBytes(outData, bOff, sendLen);
        length -= sendLen;
        offset += sendLen;
    }
}

/*SetOutgoing api framework function*/
uint8_t setOutgoing(){
    return Le;
}

/*SetOutgoingLength api framework function*/
void setOutgoingLength(uint16_t length){
    Lr = (uint8_t) length;
}

/*setOutgoingAndSend : api function to send outgoing bytes*/
void setOutgoingAndSend(uint8_t *array, uint16_t len, uint16_t bOff){
    setOutgoing();
    setOutgoingLength(len);
    sendBytes(array, bOff, len);
}

/*receiveBytes api framework function*/
uint16_t receiveBytes(uint8_t *array, uint16_t offset, uint16_t len){
     uint16_t length = t0RcvData(apdu, offset);
     arrayCopy(apduBuffer,offset,array,0,length);
	return length;
}

/*setIncomingandreceive api framework function*/
uint16_t setIncomingAndReceive(uint8_t *array, uint16_t len){
    uint16_t length = t0RcvData(apdu, OFFSET_CData);
    arrayCopy(apduBuffer,OFFSET_CData,array,0,length);
    return length;
}

/*GetBuffer get the apdu buffer*/
void getBuffer(uint8_t *apduArr, uint16_t len){
   arrayCopy(apduBuffer, 0, apduArr, 0, len);
}



