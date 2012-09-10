/// GaudiKernel
#include "GaudiKernel/CnvFactory.h"
/// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// VeloDet 
#include "VeloPixDet/DeVeloPixXType.h"


/// =======================================================================
/// Instantiation of a static factory class used by clients to create
/// instances of this service
/// =======================================================================
typedef XmlUserDetElemCnv<DeVeloPixXType>         XmlDeVeloPixXTypeCnv;
DECLARE_CONVERTER_FACTORY( XmlDeVeloPixXTypeCnv );
