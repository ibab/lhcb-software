// $Id: DeMuonRegion.cpp,v 1.1.1.1 2002-01-21 17:13:46 dhcroft Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#define MUONDET_DEMUONREGION_CPP 1
// ============================================================================

// Include files
#include "MuonDet/DeMuonRegion.h"

/** @file DeMuonChamber.cpp
 * 
 * Implementation of class : DeMuonRegion
 *
 * @author David Hutchcroft, David.Hutchcroft@cern.ch
 *
 */

/// Class ID of DeMuonRegion
const CLID& CLID_DEMuonRegion = 11005;  

/// Standard Constructor
DeMuonRegion::DeMuonRegion()
  : m_chamberNum(0)
{
}

/// Standard Constructor, sets number of chambers
DeMuonRegion::DeMuonRegion( int i )
  : m_chamberNum(i)
{
}
  

/// Standard Destructor
DeMuonRegion::~DeMuonRegion()
{
}
  
/// Retrieve Pointer to class defininition structure
const CLID& DeMuonRegion::classID()
{
  return CLID_DEMuonRegion;
}

