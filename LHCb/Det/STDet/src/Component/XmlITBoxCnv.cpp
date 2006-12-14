// $Id: XmlITBoxCnv.cpp,v 1.2 2006-12-14 13:41:07 ranjard Exp $

// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"

// TTDet
#include "STDet/DeITBox.h"

typedef XmlUserDetElemCnv<DeITBox >        XmlITBoxCnv;
DECLARE_CONVERTER_FACTORY(XmlITBoxCnv)
