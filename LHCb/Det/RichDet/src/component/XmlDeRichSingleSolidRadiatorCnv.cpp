#ifndef XMLDERICHSINGLESOLIDRADIATORCNV_CPP
#define XMLDERICHSINGLESOLIDRADIATORCNV_CPP 1

// Include files
#include "RichDet/DeRichSingleSolidRadiator.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRichSingleSolidRadiator>  
XmlDeRichSingleSolidRadiatorCnv;
static CnvFactory<XmlDeRichSingleSolidRadiatorCnv> 
s_DeRichSingleSolidRadiatorFactory;
const ICnvFactory& XmlDeRichSingleSolidRadiatorCnvFactory = 
s_DeRichSingleSolidRadiatorFactory;

#endif // XMLDERICHSINGLESOLIDRADIATORCNV_CPP
