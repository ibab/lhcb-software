// $Id: DeMuonRegion.cpp,v 1.12 2010-03-17 16:19:07 cattanem Exp $
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
    msgStream() << MSG::ERROR << "Failure to initialize DetectorElement" << endmsg;
    return sc ; 
  }

  return sc;
}


  
