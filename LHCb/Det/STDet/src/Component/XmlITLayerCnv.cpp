// $Id: XmlITLayerCnv.cpp,v 1.1 2005-12-19 15:18:25 mneedham Exp $

// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"

// TTDet
#include "STDet/DeITLayer.h"

typedef XmlUserDetElemCnv<DeITLayer >        XmlITLayerCnv;
static CnvFactory<XmlITLayerCnv>           s_XmlITLayerCnvFactory ;
const ICnvFactory&  XmlITLayerCnvFactory = s_XmlITLayerCnvFactory ;
