// $Id: XmlITSensorCnv.cpp,v 1.1 2008-03-14 18:26:40 mneedham Exp $

// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"

// ITDet
#include "STDet/DeITSensor.h"

typedef XmlUserDetElemCnv<DeITSensor >        XmlITSensorCnv;
DECLARE_CONVERTER_FACTORY(XmlITSensorCnv)
