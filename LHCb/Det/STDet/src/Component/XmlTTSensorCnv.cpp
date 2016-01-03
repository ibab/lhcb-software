// $Id: XmlTTSensorCnv.cpp,v 1.1 2008-03-14 18:26:41 mneedham Exp $

// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"

// TTDet
#include "STDet/DeTTSensor.h"

typedef XmlUserDetElemCnv<DeTTSensor >        XmlTTSensorCnv;
DECLARE_CONVERTER_FACTORY(XmlTTSensorCnv)
