// $Id: XmlDeVeloRTypeCnv.cpp,v 1.1 2004-01-29 01:02:57 mtobin Exp $

/// GaudiKernel
#include "GaudiKernel/CnvFactory.h"
/// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// VeloDet 
#include "VeloDet/DeVeloRType.h"


/// =======================================================================
/// Instantiation of a static factory class used by clients to create
/// instances of this service
/// =======================================================================
typedef XmlUserDetElemCnv<DeVeloRType>         XmlDeVeloRTypeCnv;
static  CnvFactory<XmlDeVeloRTypeCnv>          s_XmlDeVeloRTypeCnvFactory ;
const ICnvFactory&  XmlDeVeloRTypeCnvFactory = s_XmlDeVeloRTypeCnvFactory ;
