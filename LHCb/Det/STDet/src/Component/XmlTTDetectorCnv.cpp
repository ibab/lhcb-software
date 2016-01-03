// $Id: XmlTTDetectorCnv.cpp,v 1.2 2006-12-14 13:41:07 ranjard Exp $

// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"

// TTDet
#include "STDet/DeTTDetector.h"

typedef XmlUserDetElemCnv<DeTTDetector>       XmlTTDetectorCnv;
DECLARE_CONVERTER_FACTORY(XmlTTDetectorCnv)
