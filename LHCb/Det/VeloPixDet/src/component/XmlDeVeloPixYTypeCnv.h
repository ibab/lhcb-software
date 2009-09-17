/// GaudiKernel
#include "GaudiKernel/CnvFactory.h"
/// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// VeloDet 
#include "VeloPixDet/DeVeloPixYType.h"


/// =======================================================================
/// Instantiation of a static factory class used by clients to create
/// instances of this service
/// =======================================================================
typedef XmlUserDetElemCnv<DeVeloPixYType>         XmlDeVeloPixYTypeCnv;
DECLARE_CONVERTER_FACTORY( XmlDeVeloPixYTypeCnv );
