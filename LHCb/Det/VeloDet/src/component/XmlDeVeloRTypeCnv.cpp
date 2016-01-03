// $Id: XmlDeVeloRTypeCnv.cpp,v 1.2 2006-11-30 07:57:41 cattanem Exp $

/// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// VeloDet 
#include "VeloDet/DeVeloRType.h"


/// =======================================================================
/// Instantiation of a static factory class used by clients to create
/// instances of this service
/// =======================================================================
typedef XmlUserDetElemCnv<DeVeloRType>         XmlDeVeloRTypeCnv;
DECLARE_CONVERTER_FACTORY( XmlDeVeloRTypeCnv )
