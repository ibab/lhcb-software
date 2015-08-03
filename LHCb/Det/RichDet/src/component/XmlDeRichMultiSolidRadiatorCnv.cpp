#ifndef XMLDERICHMULTISOLIDRADIATORCNV_CPP
#define XMLDERICHMULTISOLIDRADIATORCNV_CPP 1

// Include files
#include "RichDet/DeRichMultiSolidRadiator.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRichMultiSolidRadiator>  
XmlDeRichMultiSolidRadiatorCnv;
DECLARE_CONVERTER_FACTORY( XmlDeRichMultiSolidRadiatorCnv )

#endif // XMLDERICHMULTISOLIDRADIATORCNV_CPP
