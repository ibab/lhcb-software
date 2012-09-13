/// GaudiKernel
#include "GaudiKernel/CnvFactory.h"
/// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// VeloDet 
#include "VPDet/DeVPSquareType.h"


/// =======================================================================
/// Instantiation of a static factory class used by clients to create
/// instances of this service
/// =======================================================================
typedef XmlUserDetElemCnv<DeVPSquareType>         XmlDeVPSquareTypeCnv;
DECLARE_CONVERTER_FACTORY( XmlDeVPSquareTypeCnv )
