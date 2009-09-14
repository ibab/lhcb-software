// $Id: DeMuonRegion.cpp,v 1.11 2009-09-14 08:58:36 jonrob Exp $
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

/// Standard Constructor
DeMuonRegion::DeMuonRegion() : m_msgStream(NULL)
{
}

/// Standard Destructor
DeMuonRegion::~DeMuonRegion()
{
  delete m_msgStream; m_msgStream = NULL;
}
  
StatusCode DeMuonRegion::initialize()  
{
  StatusCode sc = DetectorElement::initialize();
  if( sc.isFailure() ) { 
    msgStream() << MSG::ERROR << "Failure to initialize DetectorElement" << endreq;
    return sc ; 
  }

  return sc;
}


  
