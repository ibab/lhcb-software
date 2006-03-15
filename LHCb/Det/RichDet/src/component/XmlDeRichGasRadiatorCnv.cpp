#ifndef XMLDERICHGASRADIATORCNV_H 
#define XMLDERICHGASRADIATORCNV_H 1

// Include files
//#include "RichDet/DeRichGasRadiator.h"
#include "RichDet/DeRichGasRadiator.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRichGasRadiator>  XmlDeRichGasRadiatorCnv;
static CnvFactory<XmlDeRichGasRadiatorCnv> s_DeRichGasRadiatorFactory;
const ICnvFactory& XmlDeRichGasRadiatorCnvFactory = s_DeRichGasRadiatorFactory;

#endif // XMLDERICHGASRADIATORCNV_H
