#ifndef XMLDERICHRADIATORCNV_CPP
#define XMLDERICHRADIATORCNV_CPP 1

// Include files
//#include "RichDet/DeRichRadiator.h"
#include "RichDet/DeRichRadiator.h"
#include "DetDesc/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRichRadiator>  XmlDeRichRadiatorCnv;
static CnvFactory<XmlDeRichRadiatorCnv> s_DeRichRadiatorFactory;
const ICnvFactory& XmlDeRichRadiatorCnvFactory = s_DeRichRadiatorFactory;

#endif // XMLDERICHRADIATORCNV_CPP
