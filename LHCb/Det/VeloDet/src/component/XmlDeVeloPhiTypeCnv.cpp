// $Id: XmlDeVeloPhiTypeCnv.cpp,v 1.2 2006-11-30 07:57:41 cattanem Exp $
/// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// VeloDet 
#include "VeloDet/DeVeloPhiType.h"


/// =======================================================================
/// Instantiation of a static factory class used by clients to create
/// instances of this service
/// =======================================================================
typedef XmlUserDetElemCnv<DeVeloPhiType>         XmlDeVeloPhiTypeCnv;
DECLARE_CONVERTER_FACTORY( XmlDeVeloPhiTypeCnv )
