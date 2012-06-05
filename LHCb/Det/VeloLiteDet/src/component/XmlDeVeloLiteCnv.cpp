/// GaudiKernel
#include "GaudiKernel/CnvFactory.h"
/// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// VeloLiteDet 
#include "VeloLiteDet/DeVeloLite.h"

/// =======================================================================
/// Instantiation of a static factory class used by clients to create
/// instances of this service
/// =======================================================================
typedef XmlUserDetElemCnv<DeVeloLite>         XmlDeVeloLiteCnv;
DECLARE_CONVERTER_FACTORY( XmlDeVeloLiteCnv )
