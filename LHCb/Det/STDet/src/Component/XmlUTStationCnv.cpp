// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"

// UTDet
#include "STDet/DeUTStation.h"

typedef XmlUserDetElemCnv<DeUTStation>        XmlUTStationCnv;
DECLARE_CONVERTER_FACTORY(XmlUTStationCnv)
