#ifndef XMLDERICHMULTISOLIDRADIATORCNV_CPP
#define XMLDERICHMULTISOLIDRADIATORCNV_CPP 1

// Include files
#include "RichDet/DeRichMultiSolidRadiator.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRichMultiSolidRadiator>  
XmlDeRichMultiSolidRadiatorCnv;
static CnvFactory<XmlDeRichMultiSolidRadiatorCnv> 
s_DeRichMultiSolidRadiatorFactory;
const ICnvFactory& XmlDeRichMultiSolidRadiatorCnvFactory = 
s_DeRichMultiSolidRadiatorFactory;

#endif // XMLDERICHMULTISOLIDRADIATORCNV_CPP
