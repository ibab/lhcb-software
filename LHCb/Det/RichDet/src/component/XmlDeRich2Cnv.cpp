#ifndef XMLDERICH2CNV_H 
#define XMLDERICH2CNV_H 1

// Include files
//#include "RichDet/DeRich2.h"
/// GaudiKernel
#include "GaudiKernel/CnvFactory.h"

#include "RichDet/DeRich2.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRich2>  XmlDeRich2Cnv;
static CnvFactory<XmlDeRich2Cnv> s_DeRich2Factory;
const ICnvFactory& XmlDeRich2CnvFactory = s_DeRich2Factory;

#endif // XMLDERICH2CNV_H
