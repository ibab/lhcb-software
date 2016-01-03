// $Id: XmlOTQuarterCnv.cpp,v 1.2 2006-12-14 13:29:51 ranjard Exp $

/// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// OTDet 
#include "OTDet/DeOTQuarter.h"

typedef XmlUserDetElemCnv<DeOTQuarter>       XmlOTQuarterCnv;
DECLARE_CONVERTER_FACTORY(XmlOTQuarterCnv)
