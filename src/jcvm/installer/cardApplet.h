#ifndef _cardApplet_h
#define _cardApplet_h


#include "staticField.h"
#include "export.h"
#include "appletComp.h"
#include "method.h"
#include "constantPool.h"
#include "descriptor.h"
#include "header.h"
#include "directory.h"
#include "import.h"
#include "classComp.h"

typedef struct AbstractApplet {
    HeaderComponent pHeader;
    DirectoryComponent pDir;
    ImportComponent pImport;
    ClassComponent pClass;
    StaticFieldComponent pStaticField;
    MethodComponent pMethod;
    ConstantPoolComponent pConstantPool;
    DescriptorComponent pDescriptor;
    ExportComponent pExport;
}AbstractApplet;

typedef struct CardApplet {
    AbstractApplet absApp;
    AppletComponenent pApplet;
}CardApplet;

typedef struct MapAID_CardAPP{
    uint8_t aid[16];
    CardApplet *cardApp;
}MapAID_CardAPP;

extern MapAID_CardAPP appletTable[max_Applet];

extern CardApplet *constantApplet; 
#endif
