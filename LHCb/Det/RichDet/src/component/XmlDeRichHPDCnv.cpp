#ifndef XMLDERICHHPDCNV_H 
#define XMLDERICHHPDCNV_H 1

// Include files

/// GaudiKernel
#include "GaudiKernel/CnvFactory.h"

#include "RichDet/DeRichHPD.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRichHPD>  XmlDeRichHPDCnv;
static CnvFactory<XmlDeRichHPDCnv> s_DeRichHPDFactory;
const ICnvFactory& XmlDeRichHPDCnvFactory = s_DeRichHPDFactory;

#endif // XMLDERICHHPDCNV_H
