// $Id: XmlITStationCnv.cpp,v 1.1 2005-12-19 15:18:25 mneedham Exp $

// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"

// ITDet
#include "STDet/DeITStation.h"

typedef XmlUserDetElemCnv<DeITStation>        XmlITStationCnv;
static CnvFactory<XmlITStationCnv>           s_XmlITStationCnvFactory ;
const ICnvFactory&  XmlITStationCnvFactory = s_XmlITStationCnvFactory ;
