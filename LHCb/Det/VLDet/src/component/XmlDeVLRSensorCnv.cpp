/// GaudiKernel
#include "GaudiKernel/CnvFactory.h"
/// Det/DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// Det/VLDet 
#include "VLDet/DeVLRSensor.h"

/// =======================================================================
/// Instantiation of a static factory class used by clients to create
/// instances of this service
/// =======================================================================
typedef XmlUserDetElemCnv<DeVLRSensor> XmlDeVLRSensorCnv;
DECLARE_CONVERTER_FACTORY(XmlDeVLRSensorCnv)
