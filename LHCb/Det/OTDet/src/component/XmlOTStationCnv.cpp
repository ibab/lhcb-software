// $Id: XmlOTStationCnv.cpp,v 1.1 2003-06-11 11:49:36 cattanem Exp $

/// DetDesc
#include "DetDesc/XmlUserDetElemCnv.h"
/// OTDet 
#include "OTDet/DeOTStation.h"

typedef XmlUserDetElemCnv<DeOTStation>       XmlOTStationCnv;
static CnvFactory<XmlOTStationCnv>           s_XmlOTStationCnvFactory ;
const ICnvFactory&  XmlOTStationCnvFactory = s_XmlOTStationCnvFactory ;
