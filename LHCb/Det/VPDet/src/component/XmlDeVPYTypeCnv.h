/// GaudiKernel
#include "GaudiKernel/CnvFactory.h"
/// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// VeloDet 
#include "VPDet/DeVPYType.h"


/// =======================================================================
/// Instantiation of a static factory class used by clients to create
/// instances of this service
/// =======================================================================
typedef XmlUserDetElemCnv<DeVPYType>         XmlDeVPYTypeCnv;
DECLARE_CONVERTER_FACTORY( XmlDeVPYTypeCnv );
