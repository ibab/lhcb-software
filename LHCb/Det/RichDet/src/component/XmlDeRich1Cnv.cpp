#ifndef XMLDERICH1CNV_H 
#define XMLDERICH1CNV_H 1

// Include files
#include "RichDet/DeRich1.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRich1>  XmlDeRich1Cnv;
DECLARE_CONVERTER_FACTORY( XmlDeRich1Cnv )

#endif // XMLDERICH1CNV_H
