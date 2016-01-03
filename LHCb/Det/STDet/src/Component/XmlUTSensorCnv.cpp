// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"

// UTDet
#include "STDet/DeUTSensor.h"

typedef XmlUserDetElemCnv<DeUTSensor >        XmlUTSensorCnv;
DECLARE_CONVERTER_FACTORY(XmlUTSensorCnv)
