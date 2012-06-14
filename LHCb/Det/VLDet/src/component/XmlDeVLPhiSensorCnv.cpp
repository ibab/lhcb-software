/// GaudiKernel
#include "GaudiKernel/CnvFactory.h"
/// Det/DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// Det/VLDet 
#include "VLDet/DeVLPhiSensor.h"

/// =======================================================================
/// Instantiation of a static factory class used by clients to create
/// instances of this service
/// =======================================================================
typedef XmlUserDetElemCnv<DeVLPhiSensor> XmlDeVLPhiSensorCnv;
DECLARE_CONVERTER_FACTORY(XmlDeVLPhiSensorCnv)
