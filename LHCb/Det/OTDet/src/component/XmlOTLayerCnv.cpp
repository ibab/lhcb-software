// $Id: XmlOTLayerCnv.cpp,v 1.2 2003-06-16 13:40:41 sponce Exp $

/// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// OTDet 
#include "OTDet/DeOTLayer.h"

typedef XmlUserDetElemCnv<DeOTLayer>       XmlOTLayerCnv;
static CnvFactory<XmlOTLayerCnv>           s_XmlOTLayerCnvFactory ;
const ICnvFactory&  XmlOTLayerCnvFactory = s_XmlOTLayerCnvFactory ;
