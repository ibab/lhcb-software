// $Id: XmlOTDetectorCnv.cpp,v 1.3 2006-12-14 13:29:51 ranjard Exp $

/// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// OTDet 
#include "OTDet/DeOTDetector.h"

typedef XmlUserDetElemCnv<DeOTDetector>       XmlOTDetectorCnv;
DECLARE_CONVERTER_FACTORY(XmlOTDetectorCnv)
