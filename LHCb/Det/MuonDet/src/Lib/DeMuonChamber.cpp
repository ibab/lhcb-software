// $Id: DeMuonChamber.cpp,v 1.4 2002-09-27 13:59:41 dhcroft Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
//
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


/// Standard Constructors
DeMuonChamber::DeMuonChamber()
  : m_StationNumber(0), 
    m_RegionNumber(0),
    m_ChamberNumber(0),
    m_NumberGasGaps(0)
{
}

/// Constructor seting pad sizes and number of gas gaps and chamber number
DeMuonChamber::DeMuonChamber( int nStation,
                              int nRegion,
                              int nChamber,  
                              int nGasGaps)
  : m_StationNumber(nStation), 
    m_RegionNumber(nRegion),
    m_ChamberNumber(nChamber), 
    m_NumberGasGaps(nGasGaps)
{
}

/// Standard Destructor
DeMuonChamber::~DeMuonChamber()
{
}


