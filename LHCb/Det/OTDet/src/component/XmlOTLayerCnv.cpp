// $Id: XmlOTLayerCnv.cpp,v 1.1 2003-06-11 11:49:36 cattanem Exp $

/// DetDesc
#include "DetDesc/XmlUserDetElemCnv.h"
/// OTDet 
#include "OTDet/DeOTLayer.h"

typedef XmlUserDetElemCnv<DeOTLayer>       XmlOTLayerCnv;
static CnvFactory<XmlOTLayerCnv>           s_XmlOTLayerCnvFactory ;
const ICnvFactory&  XmlOTLayerCnvFactory = s_XmlOTLayerCnvFactory ;
