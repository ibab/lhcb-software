// $Id: XmlDeVeloPileUpTypeCnv.cpp,v 1.1 2004-01-29 01:02:57 mtobin Exp $

/// GaudiKernel
#include "GaudiKernel/CnvFactory.h"
/// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// VeloDet 
#include "VeloDet/DeVeloPileUpType.h"


/// =======================================================================
/// Instantiation of a static factory class used by clients to create
/// instances of this service
/// =======================================================================
typedef XmlUserDetElemCnv<DeVeloPileUpType>         XmlDeVeloPileUpTypeCnv;
static  CnvFactory<XmlDeVeloPileUpTypeCnv> s_XmlDeVeloPileUpTypeCnvFactory ;
const ICnvFactory& XmlDeVeloPileUpTypeCnvFactory=s_XmlDeVeloPileUpTypeCnvFactory;
