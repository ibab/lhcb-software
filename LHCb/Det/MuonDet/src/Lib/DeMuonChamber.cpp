// $Id: DeMuonChamber.cpp,v 1.3 2002-02-21 16:38:43 dhcroft Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/01/31 10:00:09  dhcroft
// Moved CLIDs to seperate files for Visual C linker
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
    m_padx( 0.0 ), 
    m_pady( 0.0 ), 
    m_NumberGasGaps(0)
{
}

/// Constructor seting pad sizes and number of gas gaps and chamber number
DeMuonChamber::DeMuonChamber( int nStation,
                              int nRegion,
                              int nChamber, 
                              double padx, 
                              double pady,  
                              int nGasGaps)
  : m_StationNumber(nStation), 
    m_RegionNumber(nRegion),
    m_ChamberNumber(nChamber), 
    m_padx( padx ), 
    m_pady( pady ),   
    m_NumberGasGaps(nGasGaps)
{
}

/// Standard Destructor
DeMuonChamber::~DeMuonChamber()
{
}


