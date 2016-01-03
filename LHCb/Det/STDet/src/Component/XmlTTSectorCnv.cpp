// $Id: XmlTTSectorCnv.cpp,v 1.2 2006-12-14 13:41:07 ranjard Exp $

// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"

// TTDet
#include "STDet/DeTTSector.h"

typedef XmlUserDetElemCnv<DeTTSector >        XmlTTSectorCnv;
DECLARE_CONVERTER_FACTORY(XmlTTSectorCnv)
