#ifndef XMLDERICH1CNV_H 
#define XMLDERICH1CNV_H 1

// Include files
//#include "RichDet/DeRich1.h"
/// GaudiKernel
#include "GaudiKernel/CnvFactory.h"

#include "RichDet/DeRich1.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRich1>  XmlDeRich1Cnv;
static CnvFactory<XmlDeRich1Cnv> s_DeRich1Factory;
const ICnvFactory& XmlDeRich1CnvFactory = s_DeRich1Factory;

#endif // XMLDERICH1CNV_H
