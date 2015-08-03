#ifndef XMLDEFTDETECTORCNV_H
#define XMLDEFTDETECTORCNV_H 1

// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
// FTDet
#include "FTDet/DeFTDetector.h"

typedef XmlUserDetElemCnv<DeFTDetector>       XmlDeFTDetectorCnv;
DECLARE_CONVERTER_FACTORY( XmlDeFTDetectorCnv )

#endif //XMLDEFTDETECTORCNV_H
