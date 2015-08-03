#ifndef XMLDEFTMODULECNV_H
#define XMLDEFTMODULECNV_H 1

// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
// FTDet
#include "FTDet/DeFTModule.h"

typedef XmlUserDetElemCnv<DeFTModule>       XmlDeFTModuleCnv;
DECLARE_CONVERTER_FACTORY( XmlDeFTModuleCnv )

#endif //XMLDEFTMODULECNV_H
