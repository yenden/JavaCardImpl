#include "apdu.h"


#define BUFFERSIZE 20 //max size of apdu buffer stored in memory
                      // this is not the size of the received apdu

//Lc is the data Length send in the apdu
static uint8_t Lc; 
//Le expected Length in the response
static uint8_t Le; 
//Lr apdu response Length
static uint8_t Lr; 
//SendInProgressFlag flag whiLe sending is not finished
static bool sendInProgressFlag = false; 


bool selectingAppletFlag = false;


/*setParam set apdu parameter(Lr,Lc and Le)
 *purpose: know if all data has been sent
 *or there is data remaining
 */
void setParam(uint16_t n){
    if( n < 4){
        //print error
        uint8_t ptr[] = "Error; apdu length must be > 4";
        nadiaprintf(ptr,sizeof(ptr),CHAR);
    } else if(n == 4){ //apdu case 1 ---CLA|INS|P1|P2---
        Le = 0;
        Lc = 0; 
    } else if(n == 5){ //apdu case 2 ---CLA|INS|P1|P2|Le---
		Lc = 0;
		Le = bufferRcv[4];
    } else if( n == 5+bufferRcv[4] ){ //apdu case 3 ---CLA|INS|P1|P2|Lc|Data---
		Lc = bufferRcv[4];
		Le = 0;
	} else { //apdu case 4 ---CLA|INS|P1|P2|Lc|Data|Le---
		Lc = bufferRcv[4];
		Le = bufferRcv[n-1];
	}
	if (Le == 0x7F) {
		Le = 0;
	}
	Lr = Le;
}

//Complete responds to the previous adpu and get next apdu
void complete(uint8_t *apduBuff, uint16_t status){
    uint8_t result;
    arrayfillNonAtomic(apduBuff, 0, BUFFERSIZE, 0);

    if(status == 0){ //first received APDU
        result = t0RcvCommand(apduBuff);
    } else {
        
        t0SetStatus(status); //set status word
        //printing
        uint8_t ptr2[2];
        ptr2[0] = (status & 0xFF00) >> 8;
        ptr2[1] = (status & 0x00FF);
        uint8_t ptr[] = "Sending status word:";
        nadiaprintf(ptr, sizeof(ptr),CHAR);
        nadiaprintf(ptr2,2,HEX);
        nadiaprintf("\n",sizeof("\n"),CHAR);

        result = t0SndStatusRcvCommand(apduBuff); //send status and wait for the next
    }
    if (result == 0){
        uint8_t ptr[] ="imput/output error in compLete method\n";
        nadiaprintf(ptr, sizeof(ptr),CHAR);
    }
    setParam(result);
    apduSendPtr = 0;
}

/*GetSelectingAppLetFlag*/
bool getSelectingAppletFlag(){
    return selectingAppletFlag;
}

/* send61xx sends sw for data remaining
* It send the status and waits for getResponse apdu
*/
uint16_t send61xx(uint16_t length){
    uint16_t expLen = length;
    uint16_t newLen;
    do{
        // Set SW1SW2 as 61xx (data remaining).
        t0SetStatus(0x6100 + (length&0x00FF));
        newLen = t0SndGetResponse();

        //0xC0xx <=>invalid getResponse apdu
		if(newLen > 0 && (newLen>>8 != 0xC0)) {
			Le = (uint8_t)newLen;
			expLen = (uint16_t)Le;
		}
    }while(expLen > length);

    sendInProgressFlag = false;
    return expLen;
}

/*SendBytes api framework function*/
void sendBytes(uint8_t *array, uint16_t bOff, uint16_t length){
    uint16_t len = length;
    uint16_t offset = bOff;
    uint16_t temp;
    while(len >0 ){
        temp = len;
    	// Need to force GET RESPONSE for Case 4 & for partial blocks
		if( (len != (uint16_t)Lr) ||( Lr != Le )|| sendInProgressFlag ){
			temp = send61xx(len); //send data remainig status
		}
		t0SendData(array, offset, temp);
		sendInProgressFlag = true;
		offset += temp;
		len -= temp;
		Lr -= (uint8_t)temp;
		Le = Lr;
	}
	sendInProgressFlag = false;
}

/*SendBytesLong : api function to send a long buffer*/
void sendBytesLong(uint16_t len, uint16_t bOff, uint8_t *outData, uint16_t bufLen, uint8_t *apduBuff){
    uint16_t length = len;
    uint16_t offset = bOff;
    //TODO checkArrayArgs(outData, bOff, length);
    uint16_t sendLen = bufLen;
    while(length > 0) {
        if(length < sendLen) {
            sendLen = length;
        }
        arrayCopy(outData, bOff, apduBuff, 0, sendLen);
        sendBytes(apduBuff, 0, sendLen);
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
    //Only APDUs case 3 and 4 are expected to call this method.
    uint16_t length = t0RcvData(array, offset, len);
	return length;
}

/*setIncomingandreceive api framework function*/
uint16_t setIncomingAndReceive(uint8_t *array, uint16_t len){
    uint16_t length = t0RcvData(array, OFFSET_CData, len);
    return length;
}

/*GetBuffer get the apdu buffer*/
void getBuffer(uint8_t *apduArr, uint16_t len){
   arrayCopy(bufferRcv, 0, apduArr, 0, len);
}



