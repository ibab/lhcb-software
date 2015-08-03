#ifndef XMLDEFTBILAYERCNV_H
#define XMLDEFTBILAYERCNV_H 1

// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
// FTDet
#include "FTDet/DeFTBiLayer.h"

typedef XmlUserDetElemCnv<DeFTBiLayer>       XmlDeFTBiLayerCnv;
DECLARE_CONVERTER_FACTORY( XmlDeFTBiLayerCnv )

#endif //XMLDEFTBILAYERCNV_H
