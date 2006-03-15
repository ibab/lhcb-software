#ifndef XMLDERICHAEROGELRADIATORCNV_H 
#define XMLDERICHAEROGELRADIATORCNV_H 1

// Include files
//#include "RichDet/DeRichAerogelRadiator.h"
#include "RichDet/DeRichAerogelRadiator.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRichAerogelRadiator>  XmlDeRichAerogelRadiatorCnv;
static CnvFactory<XmlDeRichAerogelRadiatorCnv> s_DeRichAerogelRadiatorFactory;
const ICnvFactory& XmlDeRichAerogelRadiatorCnvFactory = s_DeRichAerogelRadiatorFactory;

#endif // XMLDERICHAEROGELRADIATORCNV_H
