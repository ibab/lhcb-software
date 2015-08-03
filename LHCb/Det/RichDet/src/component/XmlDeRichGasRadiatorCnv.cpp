#ifndef XMLDERICHGASRADIATORCNV_H 
#define XMLDERICHGASRADIATORCNV_H 1

// Include files
#include "RichDet/DeRichGasRadiator.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRichGasRadiator>  XmlDeRichGasRadiatorCnv;
DECLARE_CONVERTER_FACTORY( XmlDeRichGasRadiatorCnv )

#endif // XMLDERICHGASRADIATORCNV_H
