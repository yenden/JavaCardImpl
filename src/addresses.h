#ifndef _addesses_h
#define _addesses_h

/*different offsets in memory*/

#define FLASH_START_ADDRESS 0 //sector 0
#define FLASH_STATIC_IMAGE_ADDRESS 256 //sector 1
#define FLASH_STATIC_ARRAY_ADDRESS 512 //sector 2
#define FLASH_JAVA_INSTANCES_BASE 1536//2560 //sector 10
#define FLASH_ARRAY_BASE 2304//(1280+256)//5120 //sector 20
#define FLASH_APDU_ARRAY_BASE 2560//(1792+256)//7680 //sector 30
#define FLASH_END_ADDRESS 0x8000


#endif