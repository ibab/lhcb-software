#ifndef XMLDERICHSINGLESOLIDRADIATORCNV_CPP
#define XMLDERICHSINGLESOLIDRADIATORCNV_CPP 1

// Include files
#include "RichDet/DeRichSingleSolidRadiator.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRichSingleSolidRadiator>  
XmlDeRichSingleSolidRadiatorCnv;
DECLARE_CONVERTER_FACTORY( XmlDeRichSingleSolidRadiatorCnv )

#endif // XMLDERICHSINGLESOLIDRADIATORCNV_CPP
