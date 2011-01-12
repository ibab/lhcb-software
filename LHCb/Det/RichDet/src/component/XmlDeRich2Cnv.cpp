#ifndef XMLDERICH2CNV_H 
#define XMLDERICH2CNV_H 1

// Include files
/// GaudiKernel
#include "GaudiKernel/CnvFactory.h"

#include "RichDet/DeRich2.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRich2>  XmlDeRich2Cnv;
DECLARE_CONVERTER_FACTORY( XmlDeRich2Cnv )

#endif // XMLDERICH2CNV_H
