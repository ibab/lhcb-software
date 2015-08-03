#ifndef XMLDEFTFIBREMATCNV_H
#define XMLDEFTFIBREMATCNV_H 1

// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
// FTDet
#include "FTDet/DeFTFibreMat.h"

typedef XmlUserDetElemCnv<DeFTFibreMat>       XmlDeFTFibreMatCnv;
DECLARE_CONVERTER_FACTORY( XmlDeFTFibreMatCnv )

#endif //XMLDEFTFIBREMATCNV_H
