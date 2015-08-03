#include "DetDescCnv/XmlUserDetElemCnv.h"
#include "VPDet/DeVPSensor.h"

/// =======================================================================
/// Instantiation of a static factory class used by clients to create
/// instances of this service
/// =======================================================================
typedef XmlUserDetElemCnv<DeVPSensor> XmlDeVPSensorCnv;
DECLARE_CONVERTER_FACTORY(XmlDeVPSensorCnv)
