// $Id: XmlTTStationCnv.cpp,v 1.2 2006-12-14 13:41:07 ranjard Exp $

// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"

// TTDet
#include "STDet/DeTTStation.h"

typedef XmlUserDetElemCnv<DeTTStation>        XmlTTStationCnv;
DECLARE_CONVERTER_FACTORY(XmlTTStationCnv)
