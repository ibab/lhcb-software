// $Id: XmlDeVeloCnv.cpp,v 1.1 2001-10-25 06:13:27 ocallot Exp $

/// GaudiKernel
#include "GaudiKernel/CnvFactory.h"
/// DetDesc
#include "DetDesc/XmlUserDetElemCnv.h"
/// VeloDet 
#include "VeloDet/DeVelo.h"


/// =======================================================================
/// Instantiation of a static factory class used by clients to create
/// instances of this service
/// =======================================================================
typedef XmlUserDetElemCnv<DeVelo>         XmlDeVeloCnv;
static  CnvFactory<XmlDeVeloCnv>          s_XmlDeVeloCnvFactory ;
const ICnvFactory&  XmlDeVeloCnvFactory = s_XmlDeVeloCnvFactory ;
