// $Id: XmlDeVeloCnv.cpp,v 1.2 2003-06-16 13:41:23 sponce Exp $

/// GaudiKernel
#include "GaudiKernel/CnvFactory.h"
/// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// VeloDet 
#include "VeloDet/DeVelo.h"


/// =======================================================================
/// Instantiation of a static factory class used by clients to create
/// instances of this service
/// =======================================================================
typedef XmlUserDetElemCnv<DeVelo>         XmlDeVeloCnv;
static  CnvFactory<XmlDeVeloCnv>          s_XmlDeVeloCnvFactory ;
const ICnvFactory&  XmlDeVeloCnvFactory = s_XmlDeVeloCnvFactory ;
