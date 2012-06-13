/// GaudiKernel
#include "GaudiKernel/CnvFactory.h"
/// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// VeloLiteDet 
#include "VeloLiteDet/DeVeloLiteRType.h"

/// =======================================================================
/// Instantiation of a static factory class used by clients to create
/// instances of this service
/// =======================================================================
typedef XmlUserDetElemCnv<DeVeloLiteRType> XmlDeVeloLiteRTypeCnv;
DECLARE_CONVERTER_FACTORY( XmlDeVeloLiteRTypeCnv )
