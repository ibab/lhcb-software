// $Id: DeMuonRegion.cpp,v 1.9 2006-12-14 13:27:07 ranjard Exp $
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
DECLARE_CONVERTER_FACTORY(XmlDeMuonRegion)

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


  
