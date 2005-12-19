// $Id: XmlTTHalfModuleCnv.cpp,v 1.1 2005-12-19 15:18:25 mneedham Exp $

// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"

// TTDet
#include "STDet/DeTTHalfModule.h"

typedef XmlUserDetElemCnv<DeTTHalfModule>      XmlTTHalfModuleCnv;
static CnvFactory<XmlTTHalfModuleCnv>          s_XmlTTHalfModuleCnvFactory ;
const ICnvFactory& XmlTTHalfModuleCnvFactory = s_XmlTTHalfModuleCnvFactory ;
