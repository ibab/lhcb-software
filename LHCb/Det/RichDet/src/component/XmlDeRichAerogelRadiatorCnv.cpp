#ifndef XMLDERICHAEROGELRADIATORCNV_H 
#define XMLDERICHAEROGELRADIATORCNV_H 1

// Include files
#include "GaudiKernel/CnvFactory.h"

#include "RichDet/DeRichAerogelRadiator.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRichAerogelRadiator>  XmlDeRichAerogelRadiatorCnv;
DECLARE_CONVERTER_FACTORY( XmlDeRichAerogelRadiatorCnv )

#endif // XMLDERICHAEROGELRADIATORCNV_H
