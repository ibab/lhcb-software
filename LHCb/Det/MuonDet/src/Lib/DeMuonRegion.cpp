// $Id: DeMuonRegion.cpp,v 1.6 2003-11-24 14:54:39 cattanem Exp $
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
DeMuonRegion::DeMuonRegion()
  : m_chamberNum(0), m_GapsPerFE(0), 
    m_FEAnodeX(0), m_FEAnodeY(0), 
    m_FECathodeX(0), m_FECathodeY(0), m_logMap()
{
}

/// Standard Destructor
DeMuonRegion::~DeMuonRegion()
{
}
  
/// Add a logical mapping between FE readout channels and 
/// logical channels (or pads if not crossed)
void DeMuonRegion::addLogicalMap( const MuonParameters::ReadoutType &type, 
                                  const int &xMap, const int &yMap){
  m_logMap.push_back(logMap_(type,xMap,yMap));
}

/// Get the logical mappings beween FE readout channels and 
/// logical channels (or pads if not crossed)
StatusCode DeMuonRegion::logicalMap(const unsigned int& mapNum,
                                    MuonParameters::ReadoutType &rType,
                                    int &xMap, int &yMap) const {
  if(mapNum >= m_logMap.size()){
    return StatusCode::FAILURE;
  }
  xMap = m_logMap[mapNum].m_xMap;
  yMap = m_logMap[mapNum].m_yMap;
  rType = m_logMap[mapNum].m_type;
  return StatusCode::SUCCESS;
}
