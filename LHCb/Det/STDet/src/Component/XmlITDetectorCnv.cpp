// $Id: XmlITDetectorCnv.cpp,v 1.2 2006-12-14 13:41:07 ranjard Exp $

// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"

// TTDet
#include "STDet/DeITDetector.h"

typedef XmlUserDetElemCnv<DeITDetector>       XmlITDetectorCnv;
DECLARE_CONVERTER_FACTORY(XmlITDetectorCnv)
