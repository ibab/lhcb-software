// $Id: XmlOTModuleCnv.cpp,v 1.2 2003-06-16 13:40:41 sponce Exp $

/// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// OTDet 
#include "OTDet/DeOTModule.h"

typedef XmlUserDetElemCnv<DeOTModule>       XmlOTModuleCnv;
static CnvFactory<XmlOTModuleCnv>           s_XmlOTModuleCnvFactory ;
const ICnvFactory&  XmlOTModuleCnvFactory = s_XmlOTModuleCnvFactory ;
