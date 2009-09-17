/// GaudiKernel
#include "GaudiKernel/CnvFactory.h"
/// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// VeloDet 
#include "VeloPixDet/DeVeloPixSquareType.h"


/// =======================================================================
/// Instantiation of a static factory class used by clients to create
/// instances of this service
/// =======================================================================
typedef XmlUserDetElemCnv<DeVeloPixSquareType>         XmlDeVeloPixSquareTypeCnv;
DECLARE_CONVERTER_FACTORY( XmlDeVeloPixSquareTypeCnv );
