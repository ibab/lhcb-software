// $Id: DeMuonChamber.cpp,v 1.1.1.1 2002-01-21 17:13:46 dhcroft Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#define MUONDET_DEMUONCHAMBER_CPP 1
// ============================================================================

// Include files
#include "MuonDet/DeMuonChamber.h"

/** @file DeMuonChamber.cpp
 * 
 * Implementation of class : DeMuonChamber
 *
 * @author David Hutchcroft, David.Hutchcroft@cern.ch
 *
 */


/// Class ID of chambers 
const CLID& CLID_DEMuonChamber = 11006;  

/// Standard Constructors
DeMuonChamber::DeMuonChamber()
  : m_padx( 0.0 ), m_pady( 0.0 ), m_NumberGasGaps(0)
{
}

/// Constructor seting pad sizes and number of gas gaps
DeMuonChamber::DeMuonChamber( double padx, double pady,  int nGasGaps)
  : m_padx( padx ), m_pady( pady ),   m_NumberGasGaps(nGasGaps)
{
}

/// Standard Destructor
DeMuonChamber::~DeMuonChamber()
{
}

/// Retrieve Pointer to class defininition structure
const CLID& DeMuonChamber::classID()
{
  return CLID_DEMuonChamber;
}

