#include "GaudiKernel/CnvFactory.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"
#include "VPDet/DeVPNewSensor.h"

/// =======================================================================
/// Instantiation of a static factory class used by clients to create
/// instances of this service
/// =======================================================================
typedef XmlUserDetElemCnv<DeVPNewSensor> XmlDeVPNewSensorCnv;
DECLARE_CONVERTER_FACTORY(XmlDeVPNewSensorCnv)
