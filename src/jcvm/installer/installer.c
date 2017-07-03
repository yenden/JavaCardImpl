#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "installer.h"
#include "../util.h"
#include "../../nvm/nvm.h"
#include "../../addresses.h"


//Header component installation
bool installHeaderComp(uint8_t dataBuffer[],CardApplet *newApplet, uint16_t* iPos){
    uint8_t i=0;
    uint8_t tag = readU1(dataBuffer, iPos);
    
    if (tag != TAG_HEADER_COMP) {
        printf("Header tag is not correct. This is not an applet\n");
        return false;
    }
    (*iPos) += 6;
    HeaderComponent ipHeader;
    ipHeader.minorVersion = readU1(dataBuffer, iPos);
    ipHeader.majorVersion = readU1(dataBuffer, iPos);
    ipHeader.flags = readU1(dataBuffer, iPos);
    ipHeader.pThisPackage.minorVersion = readU1(dataBuffer, iPos);
    ipHeader.pThisPackage.majorVersion = readU1(dataBuffer, iPos);
    ipHeader.pThisPackage.AIDLength = readU1(dataBuffer, iPos);
    for(i=0;i<ipHeader.pThisPackage.AIDLength;i++){
        ipHeader.pThisPackage.AID[i] = readU1(dataBuffer, iPos);
    }
    ipHeader.pNameInfo.nameLength = readU1(dataBuffer, iPos);
    for(i=0;i<ipHeader.pNameInfo.nameLength;i++){
        ipHeader.pNameInfo.name[i] = readU1(dataBuffer, iPos);
    }
    newApplet->absApp.pHeader = ipHeader;
    return true;
}

//Directory component installation
bool installDirComp(uint8_t dataBuffer[],CardApplet *newApplet, uint16_t * iPos){
    DirectoryComponent ipDir;
    uint8_t tag = dataBuffer[*iPos];
    if( tag != TAG_DIR_COMP ){
        printf("Directory Tag is not correct \n");
        return false;
    }
    (*iPos) += 3;
    for (uint8_t i = 0; i<12; i++){
        ipDir.componentSizes[i] = readU2(dataBuffer, iPos);
    }
    ipDir.pStaticFieldSize.imageSize = readU2(dataBuffer, iPos);
    ipDir.pStaticFieldSize.arrayInitCount = readU2(dataBuffer, iPos);
    ipDir.pStaticFieldSize.arrayInitSize = readU2(dataBuffer, iPos);
    ipDir.importCount = readU1(dataBuffer, iPos);
    ipDir.appletCount = readU1(dataBuffer, iPos);

    //The subset of jcvm we consider doesn't have customize ClassComponent
	//customcomp Number = 0
    //We thus need to step over or ignore that data
    (*iPos)++;
    newApplet->absApp.pDir = ipDir;
    return true;
}

//Applet component Installation
void installAppletComp(uint8_t dataBuffer[],CardApplet *newApplet){
    uint16_t iPosa =0;
    uint16_t appletCompSize = newApplet->absApp.pDir.componentSizes[TAG_APPLET_COMP - 1];
    AppletComponenent ipAppletComponent;
    if (appletCompSize !=0) {
        ipAppletComponent.count = readU1(dataBuffer, &iPosa);
        for (uint8_t i=0;i<ipAppletComponent.count;i++){
            ipAppletComponent.applets[i].aidLength = readU1(dataBuffer, &iPosa);
            for(uint8_t j=0;j<ipAppletComponent.applets[i].aidLength;j++){
                ipAppletComponent.applets[i].pAID[j] = readU1(dataBuffer, &iPosa);
            }
            ipAppletComponent.applets[i].installMethodOffset = readU2(dataBuffer, &iPosa);
        }
    }
    newApplet->pApplet = ipAppletComponent;
}

//Import Component Installation
bool installImportComp(uint8_t dataBuffer[],CardApplet *newApplet){
    uint16_t iPosa =0;
    uint16_t importCompSize = newApplet->absApp.pDir.componentSizes[TAG_IMPORT_COMP - 1];
    ImportComponent ipImportComponent;
    if (importCompSize ==0) {
        printf("import component which is a mandatory component is NULL");
        return false;
    } 
    ipImportComponent.count = readU1(dataBuffer, &iPosa);
    for(uint8_t i=0;i<ipImportComponent.count;i++){
        ipImportComponent.packages[i].minorVersion = readU1(dataBuffer, &iPosa);
        ipImportComponent.packages[i].majorVersion = readU1(dataBuffer, &iPosa);
        ipImportComponent.packages[i].AIDLength = readU1(dataBuffer, &iPosa);
        for(uint8_t j=0;j< ipImportComponent.packages[i].AIDLength;j++){
            ipImportComponent.packages[i].AID[j] = readU1(dataBuffer, &iPosa);
        }
    }
    newApplet->absApp.pImport = ipImportComponent;
    return true;
}

//CLass component installation
void installClassComp(uint8_t dataBuffer[],CardApplet *newApplet){
    uint16_t iPosa =0;
    uint16_t classCompSize = newApplet->absApp.pDir.componentSizes[TAG_CLASS_COMP - 1];
    //signature pool for remote methods.
    // In our subset we don't use remote methods
    // remote method parsing is in the JCVM go version
    uint16_t sPoolLenght = readU2(dataBuffer, &iPosa);
    ClassComponent ipClassComponent;
    // In this subset we didn't consider interfaces
    //intefaces parsing is in the go version
    uint16_t i = 0;
    while(iPosa < classCompSize-1){
        ipClassComponent.pClasses[i].bitfield = readU1(dataBuffer, &iPosa);
        ipClassComponent.pClasses[i].superClassRef = readU2(dataBuffer, &iPosa);
        ipClassComponent.pClasses[i].declaredInstanceSize = readU1(dataBuffer, &iPosa);
        ipClassComponent.pClasses[i].firstReferenceToken = readU1(dataBuffer, &iPosa);
        ipClassComponent.pClasses[i].referenceCount = readU1(dataBuffer, &iPosa);
        ipClassComponent.pClasses[i].publicMethodTableBase = readU1(dataBuffer, &iPosa);
        ipClassComponent.pClasses[i].publicMethodTableCount = readU1(dataBuffer, &iPosa);
        ipClassComponent.pClasses[i].packageMethodTableBase = readU1(dataBuffer, &iPosa);
        ipClassComponent.pClasses[i].packageMethodTableCount = readU1(dataBuffer, &iPosa);
        for(uint8_t j=0;j<ipClassComponent.pClasses[i].publicMethodTableCount;j++){
            ipClassComponent.pClasses[i].publicMethodTable[j] = readU2(dataBuffer, &iPosa);
        }
        for(uint8_t j=0;j<ipClassComponent.pClasses[i].packageMethodTableCount;j++){
            ipClassComponent.pClasses[i].packageMethodTable[j] = readU2(dataBuffer, &iPosa);
        }
        i++;
    }
    newApplet->absApp.pClass = ipClassComponent;
}

//ConstantPool installation
void installConstPoolComp(uint8_t dataBuffer[],CardApplet *newApplet){
    uint16_t iPosa =0;
    ConstantPoolComponent ipConstPoolComponent;
    ipConstPoolComponent.count = readU2(dataBuffer, &iPosa);
    for(uint16_t i=0;i<ipConstPoolComponent.count;i++){
        ipConstPoolComponent.constantPool[i].tag = readU1(dataBuffer, &iPosa);
        for(uint8_t j=0; j<3;j++){
            ipConstPoolComponent.constantPool[i].info[j] = readU1(dataBuffer, &iPosa);
        }
    }
    newApplet->absApp.pConstantPool = ipConstPoolComponent;
}

//Ref Location Installation
void installRefLocComp(uint8_t dataBuffer[],CardApplet *newApplet){
   //nothing 
   //because our implementation dont use this component
   // An implementation has been done in go version
}
void createArrayInMemory(uint16_t address, uint8_t type, uint16_t count, uint8_t *arrayInit){
    //write the header of the array (type and length) in the first byte
    //and the arrayInit body in the second byte of the sector
    uint8_t array[8 + count];
    array[0] = type;
    array[1] = makeU1High(count);
    array[2] = makeU1Low(count);
    for(uint16_t i = 8; i < count + 8; i++){
       array[i] = arrayInit[i];
    }
    nvmWrite(address, array, 8 + count);
}
//create a static fiel image in non volatile memory
void createStaticFieldImage(StaticFieldComponent ipStaticFieldComponent, uint8_t *arrayRef){
    uint8_t image[ipStaticFieldComponent.imageSize];
    uint16_t i = 0;
    uint16_t j = 0;
    //Build segment 1 and segment 2 data.
	//Segment 1 - arrays of primitive types initialized by <clinit> methods.
     for (i=0; i < ipStaticFieldComponent.arrayInitCount*2; i++){
        image[i] = arrayRef[i];
     }
	//Segment 2 - reference types initialized to null, including arrays.
    for (j=i; j < ipStaticFieldComponent.referenceCount*2; j++){
            image[j] = 0;
    }
    //Update segment 3
	//Segment 3 - primitive types initialized to default values.
    for (uint16_t k = j; k < ipStaticFieldComponent.defaultValueCount; k++){
        image[j]=0;
        j++;
    }
    //Update segment 4
	//Segment 4 - primitive types initialized to non-default values.
    for(uint16_t k = 0; k < ipStaticFieldComponent.nondefaultValueCount; k++){
        image[j] = ipStaticFieldComponent.pnondefaultValues[j];
        j++;
    }

    //Fill buffer in flash memory
    //todo case if the size of image is > flash sector size 
    nvmWrite(FLASH_STATIC_IMAGE_ADDRESS,image,ipStaticFieldComponent.imageSize);
    
}

//StaticField installation
void installStaticFieldComp(uint8_t dataBuffer[],CardApplet *newApplet){
    uint16_t iPosa =0;
    StaticFieldComponent ipStaticFieldComponent;
    ipStaticFieldComponent.imageSize = readU2(dataBuffer, &iPosa);
    ipStaticFieldComponent.referenceCount = readU2(dataBuffer, &iPosa);
    ipStaticFieldComponent.arrayInitCount = readU2(dataBuffer, &iPosa);

    //the purpose of this is to keep the nvm addresses of initialized arrays.
    uint8_t arrayRef[ipStaticFieldComponent.arrayInitCount * 2];
    uint16_t iPosArray = 0;
    uint16_t arryAddress = FLASH_STATIC_ARRAY_ADDRESS;

    for(uint16_t i=0; i < ipStaticFieldComponent.arrayInitCount;i++){
        ipStaticFieldComponent.pArrayInit[i].typ = readU1(dataBuffer, &iPosa);
        ipStaticFieldComponent.pArrayInit[i].count = readU2(dataBuffer, &iPosa);
        for(uint8_t j=0;j < ipStaticFieldComponent.pArrayInit[i].count;j++){
            ipStaticFieldComponent.pArrayInit[i].pValues[j] = readU1(dataBuffer, &iPosa);
        }
        //create the initialized array in nvm memory
        createArrayInMemory(arryAddress, 
                            ipStaticFieldComponent.pArrayInit[i].typ, 
                            ipStaticFieldComponent.pArrayInit[i].count,
                            ipStaticFieldComponent.pArrayInit[i].pValues
                            );
        //put the memory address (reference) in the arrayRef
        arrayRef[iPosArray] = makeU1High(arryAddress);
        iPosArray++;
        arrayRef[iPosArray] = makeU1Low(arryAddress);
        iPosArray++;
        //move to next sector for the next initialized array
        arryAddress += NvmSectorSize;
    }
    ipStaticFieldComponent.defaultValueCount = readU2(dataBuffer, &iPosa);
    ipStaticFieldComponent.nondefaultValueCount = readU2(dataBuffer, &iPosa);
    for(uint16_t i=0;i<ipStaticFieldComponent.nondefaultValueCount;i++){
        ipStaticFieldComponent.pnondefaultValues[i] = readU1(dataBuffer, &iPosa);
    }
    createStaticFieldImage(ipStaticFieldComponent, arrayRef);
    newApplet->absApp.pStaticField = ipStaticFieldComponent;
}


//Method Comp installation
void installMethodComp(uint8_t dataBuffer[],CardApplet *newApplet){
    uint16_t iPosa =0;
    MethodComponent ipMethodComp;
    ipMethodComp.handlerCount = readU1(dataBuffer, &iPosa);
    for(uint16_t i=0;i<ipMethodComp.handlerCount;i++){
         ipMethodComp.pExceptionHandlers[i].startOffset = readU2(dataBuffer, &iPosa);
         ipMethodComp.pExceptionHandlers[i].activeLength = readU2(dataBuffer, &iPosa);
         ipMethodComp.pExceptionHandlers[i].handlerOffset = readU2(dataBuffer, &iPosa);
         ipMethodComp.pExceptionHandlers[i].catchTypeIndex = readU2(dataBuffer, &iPosa);
    }
    for(uint16_t i=0;i<max_methoInfo;i++){
        ipMethodComp.pMethodInfo[i] = readU1(dataBuffer, &iPosa);
    }
    newApplet->absApp.pMethod = ipMethodComp;
}

//Export comp installation
void installExportComponent(uint8_t dataBuffer[],CardApplet *newApplet){
    uint16_t iPosa =0;
    ExportComponent ipExportComp;
    ipExportComp.classCount = readU1(dataBuffer, &iPosa);
    for(uint16_t i=0;i<ipExportComp.classCount;i++){
        ipExportComp.pClassExport[i].classOffset = readU2(dataBuffer, &iPosa);
        ipExportComp.pClassExport[i].staticFieldCount = readU1(dataBuffer, &iPosa);
        ipExportComp.pClassExport[i].staticMethodCount = readU1(dataBuffer, &iPosa);
        for(uint16_t j=0;j<ipExportComp.pClassExport[i].staticFieldCount;j++){
            ipExportComp.pClassExport[i].pStaticFieldOffsets[j]= readU2(dataBuffer, &iPosa);
        }
        for(uint16_t j=0;j<ipExportComp.pClassExport[i].staticMethodCount;j++){
            ipExportComp.pClassExport[i].pStaticMethodsOffsets[j]= readU2(dataBuffer, &iPosa);
        }
    }
    newApplet->absApp.pExport = ipExportComp;
}

//Descriptor component installation
void installDescriptorComp(uint8_t dataBuffer[],CardApplet *newApplet){
    uint16_t iPosa = 0;
    DescriptorComponent ipDescComp;
    uint16_t descCompSize = newApplet->absApp.pDir.componentSizes[TAG_DESCRIPTOR_COMP - 1];
    ipDescComp.classCount = readU1(dataBuffer, &iPosa);
     for(uint16_t i=0;i<ipDescComp.classCount;i++){
        ipDescComp.classes[i].token = readU1(dataBuffer, &iPosa);
        ipDescComp.classes[i].accessFlag = readU1(dataBuffer, &iPosa);
        ipDescComp.classes[i].thisClassRef = readU2(dataBuffer, &iPosa);
        iPosa++;
        ipDescComp.classes[i].fieldCount = readU2(dataBuffer, &iPosa);
        ipDescComp.classes[i].methodCount = readU2(dataBuffer, &iPosa);
        for(uint16_t j=0;j<ipDescComp.classes[i].fieldCount;j++){
            ipDescComp.classes[i].fields[j].token =  readU1(dataBuffer, &iPosa);
            ipDescComp.classes[i].fields[j].pAF = readU1(dataBuffer, &iPosa);
            ipDescComp.classes[i].fields[j].pFieldRef.fieldRef[0] = readU1(dataBuffer, &iPosa);
            ipDescComp.classes[i].fields[j].pFieldRef.fieldRef[1] = readU1(dataBuffer, &iPosa);
            ipDescComp.classes[i].fields[j].pFieldRef.fieldRef[2] = readU1(dataBuffer, &iPosa);
            ipDescComp.classes[i].fields[j].pFieldType = readU2(dataBuffer, &iPosa);
        }
        for(uint16_t j=0;j<ipDescComp.classes[i].methodCount;j++){
            ipDescComp.classes[i].methods[j].token = readU1(dataBuffer, &iPosa);
            ipDescComp.classes[i].methods[j].pAF = readU1(dataBuffer, &iPosa);
            ipDescComp.classes[i].methods[j].methodOffset = readU2(dataBuffer, &iPosa);
            ipDescComp.classes[i].methods[j].typeOffset = readU2(dataBuffer, &iPosa);
            ipDescComp.classes[i].methods[j].bytecodeCount = readU2(dataBuffer, &iPosa);
            ipDescComp.classes[i].methods[j].exceptionHandlerCount = readU2(dataBuffer, &iPosa);
            ipDescComp.classes[i].methods[j].exceptionHandlerIndex = readU2(dataBuffer, &iPosa);
        }
    }
    ipDescComp.types.constPoolCount = readU2(dataBuffer, &iPosa);
    for(uint16_t i=0; i<ipDescComp.types.constPoolCount; i++){
        ipDescComp.types.pConstantPoolTypes[i] = readU2(dataBuffer, &iPosa);

    }
     for(uint16_t i=0; iPosa < descCompSize ; i++){
        ipDescComp.types.pTypeDesc[i].nibbleCount = readU1(dataBuffer, &iPosa);
        uint16_t k = 0;
        for(uint16_t j = 0; j < (ipDescComp.types.pTypeDesc[i].nibbleCount+1)/2; j++){
            uint8_t val = readU1(dataBuffer, &iPosa);
            ipDescComp.types.pTypeDesc[i].typeArray[k] = readHighShift(val);
            k++;
            if(k==ipDescComp.types.pTypeDesc[i].nibbleCount){
                continue;
            }
            ipDescComp.types.pTypeDesc[i].typeArray[k] = readLow(val);
            k++;           
        }
     }
     newApplet->absApp.pDescriptor = ipDescComp;
}

//main installing method
void installer(uint8_t dataBuffer[], uint16_t length, CardApplet *newApplet){
    uint16_t iPos = 0; //Position in applet buffer

    bool headerInstall = installHeaderComp(dataBuffer,newApplet,&iPos);
    if (!headerInstall){
        return ;
    }
    bool dirInstall = installDirComp(dataBuffer,newApplet,&iPos);
    if (!dirInstall){
        return ;
    }
   
    /*Split the IJC file into component blocks
	*to facilitate component data manipulation.
	 */
    uint16_t appletCompSize = newApplet->absApp.pDir.componentSizes[TAG_APPLET_COMP - 1];
    uint8_t pAppletComponent[appletCompSize];

    uint16_t importCompSize = newApplet->absApp.pDir.componentSizes[TAG_IMPORT_COMP - 1];
    uint8_t pImportComponent[importCompSize];

    uint16_t constantPoolCompSize = newApplet->absApp.pDir.componentSizes[TAG_CONSTANTPOOL_COMP - 1];
    uint8_t pConstPoolComponent[constantPoolCompSize];

    uint16_t classCompSize = newApplet->absApp.pDir.componentSizes[TAG_CLASS_COMP - 1];
    uint8_t pClassComponent[classCompSize];

    uint16_t methodCompSize = newApplet->absApp.pDir.componentSizes[TAG_METHOD_COMP - 1];
    uint8_t pMethodComponent[methodCompSize];

    uint16_t staticFieldCompSize = newApplet->absApp.pDir.componentSizes[TAG_STATICFIELD_COMP - 1];
    uint8_t pStaticFieldComponent[staticFieldCompSize];

    uint16_t refLocCompSize = newApplet->absApp.pDir.componentSizes[TAG_REFERENCELOCATION_COMP - 1];
    uint8_t pRefLocComponent[refLocCompSize];

    uint16_t exportCompSize = newApplet->absApp.pDir.componentSizes[TAG_EXPORT_COMP - 1];
    uint8_t pExportComponent[exportCompSize];

    uint16_t descriptorCompSize = newApplet->absApp.pDir.componentSizes[TAG_DESCRIPTOR_COMP - 1];
    uint8_t pDescriptorComponent[descriptorCompSize];

    uint16_t debugCompSize = newApplet->absApp.pDir.componentSizes[TAG_DEBUG_COMP - 1];
    uint8_t pDebugComponent[debugCompSize];

    while(iPos < (length-1)){
        uint8_t tag = readU1(dataBuffer, &iPos);
        uint16_t compSize = readU2(dataBuffer, &iPos);
        switch (tag){
            case TAG_IMPORT_COMP:
                for (uint16_t j=0;j<compSize;j++){
                    pImportComponent[j] = readU1(dataBuffer, &iPos);
                }
                break;
            case TAG_APPLET_COMP: 
                for (uint16_t j=0;j<compSize;j++){
                    pAppletComponent[j] = readU1(dataBuffer, &iPos);
                }
                break;
            case TAG_CONSTANTPOOL_COMP: 
                for (uint16_t j=0;j<compSize;j++){
                    pConstPoolComponent[j] = readU1(dataBuffer, &iPos);
                }
                break;
            case TAG_CLASS_COMP:
                for (uint16_t j=0;j<compSize;j++){
                    pClassComponent[j] = readU1(dataBuffer, &iPos);
                }
                break;
            case TAG_METHOD_COMP:
                for (uint16_t j=0;j<compSize;j++){
                    pMethodComponent[j] = readU1(dataBuffer, &iPos);
                }
                break;
            case TAG_STATICFIELD_COMP:
                for (uint16_t j=0;j<compSize;j++){
                    pStaticFieldComponent[j] = readU1(dataBuffer, &iPos);
                }
                break;
            case TAG_REFERENCELOCATION_COMP:
                for (uint16_t j=0;j<compSize;j++){
                    pRefLocComponent[j] = readU1(dataBuffer, &iPos);
                }
                break;
            case TAG_EXPORT_COMP:
                for (uint16_t j=0;j<compSize;j++){
                    pExportComponent[j] = readU1(dataBuffer, &iPos);
                }
                break;
            case TAG_DESCRIPTOR_COMP:
                for (uint16_t j=0;j<compSize;j++){
                    pDescriptorComponent[j] = readU1(dataBuffer, &iPos);
                }
                break;
            case TAG_DEBUG_COMP:
                for (uint16_t j=0;j<compSize;j++){
                    pDebugComponent[j] = readU1(dataBuffer, &iPos);
                }
                break;
            default:
                //nothing
                break;
        }
    }
    installAppletComp(pAppletComponent,newApplet);
    bool importInstall = installImportComp(pImportComponent,newApplet);
    if (!importInstall){
        return;
    }
    installClassComp(pClassComponent,newApplet);
    installConstPoolComp(pConstPoolComponent,newApplet);
    installRefLocComp(pRefLocComponent,newApplet);
    installStaticFieldComp(pStaticFieldComponent,newApplet);
    installMethodComp(pMethodComponent,newApplet);
    installExportComponent(pExportComponent,newApplet);
    installDescriptorComp(pDescriptorComponent,newApplet);
}
