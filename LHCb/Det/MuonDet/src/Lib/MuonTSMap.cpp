// $Id: MuonTSMap.cpp,v 1.1 2004-02-03 11:40:27 asatta Exp $
// Include files

// local
#include "MuonDet/MuonTSMap.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonTSMap
//
// 2004-01-13 : Alessia Satta
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonTSMap::MuonTSMap(  ) {

}
//=============================================================================
// Destructor
//=============================================================================
MuonTSMap::~MuonTSMap() {}; 

//=============================================================================
StatusCode MuonTSMap::initialize(long num, long gridx[2],long gridy[2]) {
  m_NumberLogLayout=num;
  m_GridXLayout[0]=gridx[0];
  m_GridXLayout[1]=gridx[1];
  m_GridYLayout[0]=gridy[0];
  m_GridYLayout[1]=gridy[1];
  return StatusCode::SUCCESS;  
};


StatusCode MuonTSMap::update(long output,std::vector<long> lay,
                             std::vector<long> gridx,std::vector<long> gridy)
{
  m_OutputSignal=output;
  m_OutputLayoutSequence=lay;
  m_OutputGridXSequence=gridx;
  m_OutputGridYSequence=gridy;  
  return StatusCode::SUCCESS;  
};



