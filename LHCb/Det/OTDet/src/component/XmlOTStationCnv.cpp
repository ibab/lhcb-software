// $Id: XmlOTStationCnv.cpp,v 1.2 2003-06-16 13:40:41 sponce Exp $

/// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// OTDet 
#include "OTDet/DeOTStation.h"

typedef XmlUserDetElemCnv<DeOTStation>       XmlOTStationCnv;
static CnvFactory<XmlOTStationCnv>           s_XmlOTStationCnvFactory ;
const ICnvFactory&  XmlOTStationCnvFactory = s_XmlOTStationCnvFactory ;
