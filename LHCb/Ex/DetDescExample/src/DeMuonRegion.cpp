// $ID: $
#define DDEXAMPLE_DEMUONREGION_CPP

// Include files
#include "DeMuonRegion.h"
const CLID& CLID_DEMuonRegion = 11005;  // User defined

// standard constructor
DeMuonRegion::DeMuonRegion() :
  m_chamberNum(0), m_GapsPerFE(0), 
  m_FEAnodeX(0), m_FEAnodeY(0), 
  m_FECathodeX(0), m_FECathodeY(0)
{};

// Retrieve Pointer to class defininition structure
const CLID& DeMuonRegion::classID() {
  return CLID_DEMuonRegion;
}


