// $Id: DeMuonGasGap.cpp,v 1.4 2003-11-24 14:54:39 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================

// Include files
#include "MuonDet/DeMuonGasGap.h"

/** @file DeMuonGasGap.cpp
 * 
 * Implementation of class : DeMuonChamber
 *
 * @author David Hutchcroft, David.Hutchcroft@cern.ch
 *
 */


DeMuonGasGap::DeMuonGasGap() :
  m_StationNumber(0),
  m_RegionNumber(0),
  m_ChamberNumber(0),
  m_GasGapNumber(0)
{
}

DeMuonGasGap::DeMuonGasGap(int nStation, int nRegion, 
                           int nChamber, int nGasGap) :
  m_StationNumber(nStation),
  m_RegionNumber(nRegion),
  m_ChamberNumber(nChamber),
  m_GasGapNumber(nGasGap)
{
}

DeMuonGasGap::~DeMuonGasGap()
{
}
