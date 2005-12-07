// $Id: DeMuonRegion.cpp,v 1.8 2005-12-07 08:46:46 asarti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================

// Include files
#include "MuonDet/DeMuonRegion.h"

/** @file DeMuonRegion.cpp
 * 
 * Implementation of class : DeMuonRegion
 *
 * @author David Hutchcroft, David.Hutchcroft@cern.ch
 *
 */
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef XmlUserDetElemCnv<DeMuonRegion>       XmlDeMuonRegion;
static CnvFactory<XmlDeMuonRegion>           s_XmlDeMuonRegionFactory ;
const ICnvFactory&  XmlDeMuonRegionFactory = s_XmlDeMuonRegionFactory ;

/// Standard Constructor
DeMuonRegion::DeMuonRegion()
{
}

/// Standard Destructor
DeMuonRegion::~DeMuonRegion()
{
}
  
StatusCode DeMuonRegion::initialize()  
{
  MsgStream msg( msgSvc(), name() );

  StatusCode sc = DetectorElement::initialize();
  if( sc.isFailure() ) { 
    msg << MSG::ERROR << "Failure to initialize DetectorElement" << endreq;
    return sc ; 
  }

  return sc;
}


  
