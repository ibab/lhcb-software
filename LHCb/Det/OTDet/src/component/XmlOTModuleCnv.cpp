// $Id: XmlOTModuleCnv.cpp,v 1.3 2006-12-14 13:29:51 ranjard Exp $

/// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// OTDet 
#include "OTDet/DeOTModule.h"

typedef XmlUserDetElemCnv<DeOTModule>       XmlOTModuleCnv;
DECLARE_CONVERTER_FACTORY(XmlOTModuleCnv)
