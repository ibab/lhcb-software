// $Id: XmlDeVeloCnv.cpp,v 1.3 2006-11-30 07:57:41 cattanem Exp $
/// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// VeloDet 
#include "VeloDet/DeVelo.h"


/// =======================================================================
/// Instantiation of a static factory class used by clients to create
/// instances of this service
/// =======================================================================
typedef XmlUserDetElemCnv<DeVelo>         XmlDeVeloCnv;
DECLARE_CONVERTER_FACTORY( XmlDeVeloCnv )
