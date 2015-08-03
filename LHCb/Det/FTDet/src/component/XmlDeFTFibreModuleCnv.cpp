#ifndef XMLDEFTFIBREMODULECNV_H
#define XMLDEFTFIBREMODULECNV_H 1

// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
// FTDet
#include "FTDet/DeFTFibreModule.h"

typedef XmlUserDetElemCnv<DeFTFibreModule>       XmlDeFTFibreModuleCnv;
DECLARE_CONVERTER_FACTORY( XmlDeFTFibreModuleCnv )

#endif //XMLDEFTFIBREMODULECNV_H
