// $Id: DeMuonGasGap.cpp,v 1.1.1.1 2002-01-21 17:13:46 dhcroft Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#define MUONDET_DEMUONCHAMBER_CPP 1
// ============================================================================

// Include files
#include "MuonDet/DeMuonGasGap.h"

/** @file DeMuonChamber.cpp
 * 
 * Implementation of class : DeMuonChamber
 *
 * @author David Hutchcroft, David.Hutchcroft@cern.ch
 *
 */

const CLID& CLID_DEMuonGasGap = 11007;  

/// Standard Constructor
DeMuonGasGap::DeMuonGasGap()
{
}

/// Standard Destructor
DeMuonGasGap::~DeMuonGasGap()
{
}

/// Retrieve Pointer to class defininition structure
const CLID& DeMuonGasGap::classID()
{
  return CLID_DEMuonGasGap;
}

