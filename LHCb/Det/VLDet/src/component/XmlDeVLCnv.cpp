/// GaudiKernel
#include "GaudiKernel/CnvFactory.h"
/// Det/DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// Det/VLDet 
#include "VLDet/DeVL.h"

/// =======================================================================
/// Instantiation of a static factory class used by clients to create
/// instances of this service
/// =======================================================================
typedef XmlUserDetElemCnv<DeVL> XmlDeVLCnv;
DECLARE_CONVERTER_FACTORY(XmlDeVLCnv)
