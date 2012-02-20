#ifndef XMLDETUTORIALPIXELCONTAINERCNV_H
#define XMLDETUTORIALPIXELCONTAINERCNV_H 1


// local

#include "GaudiKernel/CnvFactory.h"

#include "DeTutorialPixelContainer.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeTutorialPixelContainer>  XmlDeTutorialPixelContainerCnv;
DECLARE_CONVERTER_FACTORY( XmlDeTutorialPixelContainerCnv )

#endif //XMLDETUTORIALPIXELCONTAINERCNV_H
