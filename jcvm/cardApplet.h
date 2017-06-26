#ifndef _cardApplet_h
#define _cardApplet_h

#include "header.h"
#include "directory.h"
#include "import.h"
#include "classComp.h"
#include "staticField.h"
#include "method.h"
#include "constantPool.h"
#include "descriptor.h"
#include "export.h"
#include "appletComp.h"

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

#endif