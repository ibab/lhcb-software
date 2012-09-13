/// GaudiKernel
#include "GaudiKernel/CnvFactory.h"
/// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// VeloDet 
#include "VPDet/DeVPXType.h"


/// =======================================================================
/// Instantiation of a static factory class used by clients to create
/// instances of this service
/// =======================================================================
typedef XmlUserDetElemCnv<DeVPXType>         XmlDeVPXTypeCnv;
DECLARE_CONVERTER_FACTORY( XmlDeVPXTypeCnv );
