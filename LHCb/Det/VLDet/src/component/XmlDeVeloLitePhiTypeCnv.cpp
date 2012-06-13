/// GaudiKernel
#include "GaudiKernel/CnvFactory.h"
/// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// VeloLiteDet 
#include "VeloLiteDet/DeVeloLitePhiType.h"

/// =======================================================================
/// Instantiation of a static factory class used by clients to create
/// instances of this service
/// =======================================================================
typedef XmlUserDetElemCnv<DeVeloLitePhiType> XmlDeVeloLitePhiTypeCnv;
DECLARE_CONVERTER_FACTORY( XmlDeVeloLitePhiTypeCnv )
