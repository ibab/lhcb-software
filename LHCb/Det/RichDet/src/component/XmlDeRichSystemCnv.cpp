#ifndef XMLDERICHSYSTEMCNV_H 
#define XMLDERICHSYSTEMCNV_H 1

// Include files

/// GaudiKernel
#include "GaudiKernel/CnvFactory.h"

#include "RichDet/DeRichSystem.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRichSystem>  XmlDeRichSystemCnv;
static CnvFactory<XmlDeRichSystemCnv> s_DeRichSystemFactory;
const ICnvFactory& XmlDeRichSystemCnvFactory = s_DeRichSystemFactory;

#endif // XMLDERICHSYSTEMCNV_H
