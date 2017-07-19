#include "installer.h"
#include "../../applet.h"

#define max_arrayInit 8
#define max_imageSize 13
#define max_arrayInitCount 4

//Header component installation
bool installHeaderComp(CardApplet *newApplet){
    uint16_t iPos = 0;
    uint8_t tag = readU1(headerComp, &iPos);
    
    if (tag != TAG_HEADER_COMP) {
        uint8_t ptr[] = "Header tag is not correct. This is not an applet";
        myprintf(ptr, sizeof(ptr),CHAR);
        return false;
    }
    iPos += 6;
    HeaderComponent ipHeader;
    ipHeader.minorVersion = readU1(headerComp, &iPos);
    ipHeader.majorVersion = readU1(headerComp, &iPos);
    ipHeader.flags = readU1(headerComp, &iPos);
    ipHeader.pThisPackage.minorVersion = readU1(headerComp, &iPos);
    ipHeader.pThisPackage.majorVersion = readU1(headerComp, &iPos);
    ipHeader.pThisPackage.AIDLength = readU1(headerComp, &iPos);

    ipHeader.pThisPackage.AID = &headerComp[iPos];
    iPos += ipHeader.pThisPackage.AIDLength;

    ipHeader.pNameInfo.nameLength = readU1(headerComp, &iPos);
    ipHeader.pNameInfo.name = &headerComp[iPos];
    iPos += ipHeader.pNameInfo.nameLength;
   
    newApplet->absApp.pHeader = ipHeader;
    return true;
}

//Directory component installation
bool installDirComp(CardApplet *newApplet){
    uint16_t iPos = 0;
    DirectoryComponent ipDir;
    uint8_t tag = readU1(dirComp, &iPos);
    if( tag != TAG_DIR_COMP ){
        uint8_t ptr[] = "Directory Tag is not correct";
        myprintf(ptr, sizeof(ptr),CHAR);
        return false;
    }
    iPos += 2;
    for (uint8_t i = 0; i<12; i++){
        ipDir.componentSizes[i] = readU2(dirComp, &iPos);
    }
    ipDir.pStaticFieldSize.imageSize = readU2(dirComp, &iPos);
    ipDir.pStaticFieldSize.arrayInitCount = readU2(dirComp, &iPos);
    ipDir.pStaticFieldSize.arrayInitSize = readU2(dirComp, &iPos);
    ipDir.importCount = readU1(dirComp, &iPos);
   
    ipDir.appletCount = readU1(dirComp, &iPos);

    //The subset of jcvm we consider doesn't have customize ClassComponent
	//customcomp Number = 0
    //We thus need to step over or ignore that data
    iPos++;
    newApplet->absApp.pDir = ipDir;
    return true;
}

//Applet component Installation
void installAppletComp(CardApplet *newApplet){
    uint16_t iPos = 0;
    uint16_t appletCompSize = newApplet->absApp.pDir.componentSizes[TAG_APPLET_COMP - 1];
    AppletComponenent ipAppletComponent;
    iPos += 3;
    if (appletCompSize !=0) {
        ipAppletComponent.count = readU1(appletComp, &iPos);
        for (uint8_t i=0;i<ipAppletComponent.count;i++){
            ipAppletComponent.applets[i].aidLength = readU1(appletComp, &iPos);
            ipAppletComponent.applets[i].pAID = &appletComp[iPos];
            iPos += ipAppletComponent.applets[i].aidLength;
            ipAppletComponent.applets[i].installMethodOffset = readU2(appletComp, &iPos);
        }
    }
    newApplet->pApplet = ipAppletComponent;
}

//Import Component Installation
bool installImportComp(CardApplet *newApplet){
    uint16_t iPos = 0;
    uint16_t importCompSize = newApplet->absApp.pDir.componentSizes[TAG_IMPORT_COMP - 1];
    ImportComponent ipImportComponent;
    if (importCompSize ==0) {
        uint8_t ptr[] = "import component which is a mandatory component is NULL";
        myprintf(ptr, sizeof(ptr),CHAR);
        return false;
    } 
    iPos += 3;
    ipImportComponent.count = readU1(importComp, &iPos);
    for(uint8_t i = 0; i < ipImportComponent.count; i++){
        ipImportComponent.packages[i].minorVersion = readU1(importComp, &iPos);
        ipImportComponent.packages[i].majorVersion = readU1(importComp, &iPos);
        ipImportComponent.packages[i].AIDLength = readU1(importComp, &iPos);
        ipImportComponent.packages[i].AID = &importComp[iPos];
        iPos += ipImportComponent.packages[i].AIDLength;
    }
    newApplet->absApp.pImport = ipImportComponent;
    return true;
}

//CLass component installation
void installClassComp(CardApplet *newApplet){
    uint16_t iPos = 3;
    uint16_t classCompSize = newApplet->absApp.pDir.componentSizes[TAG_CLASS_COMP - 1];
    //signature pool for remote methods.
    // In our subset we don't use remote methods
    // remote method parsing is in the JCVM go version
    uint16_t sPoolLenght = readU2(classComp, &iPos);
    ClassComponent ipClassComponent;
    // In this subset we didn't consider interfaces
    //intefaces parsing is in the go version
    uint16_t i = 0;
    while(iPos < (classCompSize-1)+3){
        ipClassComponent.pClasses[i].bitfield = readU1(classComp, &iPos);
        ipClassComponent.pClasses[i].superClassRef = readU2(classComp, &iPos);
        ipClassComponent.pClasses[i].declaredInstanceSize = readU1(classComp, &iPos);
        ipClassComponent.pClasses[i].firstReferenceToken = readU1(classComp, &iPos);
        ipClassComponent.pClasses[i].referenceCount = readU1(classComp, &iPos);
        ipClassComponent.pClasses[i].publicMethodTableBase = readU1(classComp, &iPos);
        ipClassComponent.pClasses[i].publicMethodTableCount = readU1(classComp, &iPos);
        ipClassComponent.pClasses[i].packageMethodTableBase = readU1(classComp, &iPos);
        ipClassComponent.pClasses[i].packageMethodTableCount = readU1(classComp, &iPos);
        for(uint8_t j=0;j<ipClassComponent.pClasses[i].publicMethodTableCount;j++){
            ipClassComponent.pClasses[i].publicMethodTable[j] = readU2(classComp, &iPos);
        }
        for(uint8_t j=0;j<ipClassComponent.pClasses[i].packageMethodTableCount;j++){
            ipClassComponent.pClasses[i].packageMethodTable[j] = readU2(classComp, &iPos);
        }
        i++;
    }
    newApplet->absApp.pClass = ipClassComponent;
}

//ConstantPool installation
void installConstPoolComp(CardApplet *newApplet){
    uint16_t iPos = 3;
    ConstantPoolComponent ipConstPoolComponent;
    ipConstPoolComponent.count = readU2(constantPoolComp, &iPos);
    for(uint16_t i = 0; i < ipConstPoolComponent.count; i++){
        ipConstPoolComponent.constantPool[i].tag = readU1(constantPoolComp, &iPos);
        for(uint8_t j=0; j<3; j++){
            ipConstPoolComponent.constantPool[i].info[j] = readU1(constantPoolComp, &iPos);
        }
    }
    newApplet->absApp.pConstantPool = ipConstPoolComponent;
}

//Ref Location Installation
void installRefLocComp(CardApplet *newApplet){
   //nothing 
   //because our implementation dont use this component
   // An implementation has been done in go version
}

void createArrayInMemory(uint16_t address, ArrayInitInfo *arrayInit){
    //write the header of the array (type and length) in the first byte
    //and the arrayInit body in the second byte of the sector
    uint8_t array[8 + max_arrayInit];
    array[0] = arrayInit->typ;
    array[1] = makeU1High(arrayInit->count);
    array[2] = makeU1Low(arrayInit->count);
    uint16_t j = 0;
    for(uint16_t i = 8; i < (arrayInit->count + 8); i++){
       array[i] = *(arrayInit->pValues + j);
       j++;
    }
    nvmWrite(address, array, 8 + arrayInit->count);
}
//create a static fiel image in non volatile memory
void createStaticFieldImage(StaticFieldComponent ipStaticFieldComponent, uint8_t *arrayRef){
    uint8_t image[max_imageSize];
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
        image[j] = *(ipStaticFieldComponent.pnondefaultValues + k);
        j++;
    }

    //Fill buffer in flash memory
    //todo if the size of image is > flash sector size 
    nvmWrite(FLASH_STATIC_IMAGE_ADDRESS,image,max_imageSize);
    
}


//StaticField installation
void installStaticFieldComp(CardApplet *newApplet){
    uint16_t iPos = 3;
    StaticFieldComponent ipStaticFieldComponent;
    ipStaticFieldComponent.imageSize = readU2(staticFieldComp, &iPos);
    ipStaticFieldComponent.referenceCount = readU2(staticFieldComp, &iPos);
    ipStaticFieldComponent.arrayInitCount = readU2(staticFieldComp, &iPos);
    //the purpose of this is to keep the nvm addresses of initialized arrays.
    uint8_t arrayRef[max_arrayInitCount * 2];
    uint16_t iPosArray = 0;
    uint16_t arryAddress = FLASH_STATIC_ARRAY_ADDRESS;

    for(uint16_t i=0; i < ipStaticFieldComponent.arrayInitCount; i++){
        ipStaticFieldComponent.pArrayInit[i].typ = readU1(staticFieldComp, &iPos);
        ipStaticFieldComponent.pArrayInit[i].count = readU2(staticFieldComp,&iPos);
        ipStaticFieldComponent.pArrayInit[i].pValues = &staticFieldComp[iPos];
        iPos += ipStaticFieldComponent.pArrayInit[i].count;
    
        //create the initialized array in nvm memory
        createArrayInMemory(arryAddress, 
                            &ipStaticFieldComponent.pArrayInit[i]
                            );
        //put the memory address (reference) in the arrayRef
        arrayRef[iPosArray] = makeU1High(arryAddress);
        iPosArray++;
        arrayRef[iPosArray] = makeU1Low(arryAddress);
        iPosArray++;
        //move to next sector for the next initialized array
        arryAddress += NvmSectorSize;
    }
    ipStaticFieldComponent.defaultValueCount = readU2(staticFieldComp, &iPos);
    ipStaticFieldComponent.nondefaultValueCount = readU2(staticFieldComp, &iPos);
    ipStaticFieldComponent.pnondefaultValues = &staticFieldComp[iPos];
    iPos += ipStaticFieldComponent.nondefaultValueCount;
    
    createStaticFieldImage(ipStaticFieldComponent, arrayRef);
    newApplet->absApp.pStaticField = ipStaticFieldComponent;
}


//Method Comp installation
void installMethodComp(CardApplet *newApplet){
    uint16_t iPos = 3;
    MethodComponent ipMethodComp;
    uint16_t methCompSize = newApplet->absApp.pDir.componentSizes[TAG_METHOD_COMP - 1];
    ipMethodComp.handlerCount = readU1(methodComp, &iPos);
    for(uint16_t i=0;i<ipMethodComp.handlerCount;i++){
         ipMethodComp.pExceptionHandlers[i].startOffset = readU2(methodComp, &iPos);
         ipMethodComp.pExceptionHandlers[i].activeLength = readU2(methodComp, &iPos);
         ipMethodComp.pExceptionHandlers[i].handlerOffset = readU2(methodComp, &iPos);
         ipMethodComp.pExceptionHandlers[i].catchTypeIndex = readU2(methodComp, &iPos);
    }
    ipMethodComp.methInfoSize = methCompSize - (8*ipMethodComp.handlerCount+1);
    ipMethodComp.pMethodInfo = &methodComp[iPos];
    iPos += ipMethodComp.methInfoSize;
   
    newApplet->absApp.pMethod = ipMethodComp;
}

//Export comp installation
void installExportComponent(CardApplet *newApplet){
    uint16_t iPos = 3;
    ExportComponent ipExportComp;
    uint16_t expCompSize = newApplet->absApp.pDir.componentSizes[TAG_EXPORT_COMP - 1];
    if(expCompSize != 0){
        ipExportComp.classCount = readU1(exportComp, &iPos);
        for(uint16_t i=0; i<ipExportComp.classCount; i++){
            ipExportComp.pClassExport[i].classOffset = readU2(exportComp, &iPos);
            ipExportComp.pClassExport[i].staticFieldCount = readU1(exportComp, &iPos);
            ipExportComp.pClassExport[i].staticMethodCount = readU1(exportComp, &iPos);
            for(uint16_t j=0;j<ipExportComp.pClassExport[i].staticFieldCount;j++){
                    ipExportComp.pClassExport[i].pStaticFieldOffsets[j]= readU2(exportComp, &iPos);
            }
            for(uint16_t j=0;j<ipExportComp.pClassExport[i].staticMethodCount;j++){
                    ipExportComp.pClassExport[i].pStaticMethodsOffsets[j]= readU2(exportComp, &iPos);
            }
        }
    }
    newApplet->absApp.pExport = ipExportComp;
}

//Descriptor component installation
void installDescriptorComp(CardApplet *newApplet){
    uint16_t iPos = 3;
    DescriptorComponent ipDescComp;
    uint16_t descCompSize = newApplet->absApp.pDir.componentSizes[TAG_DESCRIPTOR_COMP - 1];
    ipDescComp.classCount = readU1(descriptorComp, &iPos);
     for(uint16_t i=0;i<ipDescComp.classCount;i++){
        ipDescComp.classes[i].token = readU1(descriptorComp, &iPos);
        ipDescComp.classes[i].accessFlag = readU1(descriptorComp, &iPos);
        ipDescComp.classes[i].thisClassRef = readU2(descriptorComp, &iPos);
        iPos++;
        ipDescComp.classes[i].fieldCount = readU2(descriptorComp, &iPos);
        ipDescComp.classes[i].methodCount = readU2(descriptorComp, &iPos);
        for(uint16_t j=0; j < ipDescComp.classes[i].fieldCount; j++){
            ipDescComp.classes[i].fields[j].token =  readU1(descriptorComp, &iPos);
            ipDescComp.classes[i].fields[j].pAF = readU1(descriptorComp, &iPos);
            ipDescComp.classes[i].fields[j].pFieldRef.fieldRef[0] = readU1(descriptorComp, &iPos);
            ipDescComp.classes[i].fields[j].pFieldRef.fieldRef[1] = readU1(descriptorComp, &iPos);
            ipDescComp.classes[i].fields[j].pFieldRef.fieldRef[2] = readU1(descriptorComp, &iPos);
            ipDescComp.classes[i].fields[j].pFieldType = readU2(descriptorComp, &iPos);
        }
        for(uint16_t j=0; j < ipDescComp.classes[i].methodCount; j++){
            ipDescComp.classes[i].methods[j].token = readU1(descriptorComp, &iPos);
            ipDescComp.classes[i].methods[j].pAF = readU1(descriptorComp, &iPos);
            ipDescComp.classes[i].methods[j].methodOffset = readU2(descriptorComp, &iPos);
            ipDescComp.classes[i].methods[j].typeOffset = readU2(descriptorComp, &iPos);
            ipDescComp.classes[i].methods[j].bytecodeCount = readU2(descriptorComp, &iPos);
            ipDescComp.classes[i].methods[j].exceptionHandlerCount = readU2(descriptorComp, &iPos);
            ipDescComp.classes[i].methods[j].exceptionHandlerIndex = readU2(descriptorComp, &iPos);
        }
    }
    ipDescComp.types.constPoolCount = readU2(descriptorComp, &iPos);
    ipDescComp.types.pConstantPoolTypes = &descriptorComp[iPos];
    iPos += (2 * ipDescComp.types.constPoolCount);
    for(uint16_t i=0; iPos < 3 + descCompSize ; i++){
        ipDescComp.types.pTypeDesc[i].nibbleCount = readU1(descriptorComp, &iPos);
        uint16_t k = 0;
        for(uint16_t j = 0; j < (ipDescComp.types.pTypeDesc[i].nibbleCount+1)/2; j++){
            uint8_t val = readU1(descriptorComp, &iPos);
            ipDescComp.types.pTypeDesc[i].typeArray[k] = readHighShift(val);
            k++;
            if(k == ipDescComp.types.pTypeDesc[i].nibbleCount){
                continue;
            }
            ipDescComp.types.pTypeDesc[i].typeArray[k] = readLow(val);
            k++;       
        }
     }
    newApplet->absApp.pDescriptor = ipDescComp;
}

//main installing method
void installer(CardApplet *newApplet){
    uint16_t iPos = 0; //Position in applet buffer

    bool headerInstall = installHeaderComp(newApplet);
    if (!headerInstall){
        return ;
    }
   bool dirInstall = installDirComp(newApplet);
    if (!dirInstall){
        return ;
    }
    bool importInstall = installImportComp(newApplet);
   if (!importInstall){
        return;
    }
    installAppletComp(newApplet);
    installClassComp(newApplet);
    installMethodComp(newApplet);
    installStaticFieldComp(newApplet);
    installExportComponent(newApplet);
    installConstPoolComp(newApplet);
    installRefLocComp(newApplet);
    installDescriptorComp(newApplet);

}
