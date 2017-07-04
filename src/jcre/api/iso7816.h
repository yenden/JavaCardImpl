#ifndef _iso7816_h
#define _iso7816_h

// Mnemonics for the SW1,SW2 error codes

	
// Response status : No Error = (short)0x9000
#define SW_NO_ERROR  0x9000

	
// Response status : Wrong length = 0x6700	 
#define SW_WRONG_LENGTH 0x6700

// Response status : Security condition not satisfied = 0x6982
#define SW_SECURITY_STATUS_NOT_SATISFIED 0x6982

// Response status : Data invalid = 0x6984
#define SW_DATA_INVALID 0x6984

// Response status : Conditions of use not satisfied = 0x6985
#define SW_CONTIDTION_NOT_SATISFIED 0x6985

// Response status : Applet selection failed = 0x6999;
#define SW_APPLET_SELECT_FAILED 0x6999

// Response status : Wrong data = 0x6A80
#define SW_WRONG_DATA 0x6A80

// Response status : Correct Expected Length (Le) = 0x6C00
#define SW_CORRECT_LENGTH_00 0x6C00

// Response status : INS value not supported = 0x6D00
#define SW_INS_NOT_SUPPORTED 0x6D00

// Response status : CLA value not supported = 0x6E00
#define SW_CLA_NOT_SUPPORTED 0x6E00

// Response status : No precise diagnosis = 0x6F00
#define SW_UNKNOWN 0x6F00

// Offsets into APDU header information

// APDU header offset : CLA = 0
#define OFFSET_CLA 0

// APDU header offset : INS = 1
#define OFFSET_INS 1

// APDU header offset : P1 = 2
#define OFFSET_P1 2

// APDU header offset : P2 = 3
#define OFFSET_P2 3

// APDU header offset : LC = 4
#define OFFSET_LC 4

// APDU command data offset : CDATA = 5
#define OFFSET_CData 5

// APDU command CLA : ISO 7816 = 0x00
#define CLA_ISO7816 0x00

// APDU command INS : SELECT = 0xA4
#define INS_SELECT  0xA4
#define INS_INSTALL  0x01

// APDU command INS : EXTERNAL AUTHENTICATE = 0x82
#define INS_EXTERNAL_AUTHENTICATE 0x82

// PINException reason codes
// This reason code is used to indicate that one or more input parameters is
// out of allowed bounds.
#define ILLEGAL_VALUE 1

// This reason code is used to indicate a method has been invoked at an illegal
// or inappropriate time.
#define ILLEGAL_STATE 2

#endif