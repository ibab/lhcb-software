#ifndef XMLDEFTSTATIONCNV_H
#define XMLDEFTSTATIONCNV_H 1

// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
// FTDet
#include "FTDet/DeFTStation.h"

typedef XmlUserDetElemCnv<DeFTStation>       XmlDeFTStationCnv;
DECLARE_CONVERTER_FACTORY( XmlDeFTStationCnv )

#endif //XMLDEFTSTATIONCNV_H
