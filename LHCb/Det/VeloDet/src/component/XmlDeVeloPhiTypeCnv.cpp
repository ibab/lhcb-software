// $Id: XmlDeVeloPhiTypeCnv.cpp,v 1.1 2004-01-29 01:02:57 mtobin Exp $

/// GaudiKernel
#include "GaudiKernel/CnvFactory.h"
/// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"
/// VeloDet 
#include "VeloDet/DeVeloPhiType.h"


/// =======================================================================
/// Instantiation of a static factory class used by clients to create
/// instances of this service
/// =======================================================================
typedef XmlUserDetElemCnv<DeVeloPhiType>         XmlDeVeloPhiTypeCnv;
static  CnvFactory<XmlDeVeloPhiTypeCnv>          s_XmlDeVeloPhiTypeCnvFactory ;
const ICnvFactory&  XmlDeVeloPhiTypeCnvFactory = s_XmlDeVeloPhiTypeCnvFactory ;
