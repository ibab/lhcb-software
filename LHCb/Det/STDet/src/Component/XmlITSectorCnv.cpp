// $Id: XmlITSectorCnv.cpp,v 1.2 2006-12-14 13:41:07 ranjard Exp $

// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"

// TTDet
#include "STDet/DeITSector.h"

typedef XmlUserDetElemCnv<DeITSector >        XmlITSectorCnv;
DECLARE_CONVERTER_FACTORY(XmlITSectorCnv)
