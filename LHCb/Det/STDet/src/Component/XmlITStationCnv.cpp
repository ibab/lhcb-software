// $Id: XmlITStationCnv.cpp,v 1.2 2006-12-14 13:41:07 ranjard Exp $

// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"

// ITDet
#include "STDet/DeITStation.h"

typedef XmlUserDetElemCnv<DeITStation>        XmlITStationCnv;
DECLARE_CONVERTER_FACTORY(XmlITStationCnv)
