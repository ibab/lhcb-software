// $Id: XmlOTModuleCnv.cpp,v 1.1 2003-06-11 11:49:36 cattanem Exp $

/// DetDesc
#include "DetDesc/XmlUserDetElemCnv.h"
/// OTDet 
#include "OTDet/DeOTModule.h"

typedef XmlUserDetElemCnv<DeOTModule>       XmlOTModuleCnv;
static CnvFactory<XmlOTModuleCnv>           s_XmlOTModuleCnvFactory ;
const ICnvFactory&  XmlOTModuleCnvFactory = s_XmlOTModuleCnvFactory ;
