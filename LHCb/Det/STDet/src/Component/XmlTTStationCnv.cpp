// $Id: XmlTTStationCnv.cpp,v 1.1 2005-12-19 15:18:25 mneedham Exp $

// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"

// TTDet
#include "STDet/DeTTStation.h"

typedef XmlUserDetElemCnv<DeTTStation>        XmlTTStationCnv;
static CnvFactory<XmlTTStationCnv>           s_XmlTTStationCnvFactory ;
const ICnvFactory&  XmlTTStationCnvFactory = s_XmlTTStationCnvFactory ;
