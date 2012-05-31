#ifndef XMLDEFTLAYERCNV_H
#define XMLDEFTLAYERCNV_H 1

// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
// Gaudi
#include "GaudiKernel/CnvFactory.h"
// FTDet
#include "FTDet/DeFTLayer.h"

typedef XmlUserDetElemCnv<DeFTLayer>       XmlDeFTLayerCnv;
DECLARE_CONVERTER_FACTORY( XmlDeFTLayerCnv )

#endif //XMLDEFTLAYERCNV_H
