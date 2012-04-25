// $Id: $
// Include files 

// STL
#include <fstream>
#include <sstream>

// local
#include "L0MuonKernel/L0MPtLUT.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MPtLUT
//
// 2012-04-24 : Julien Cogan
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MPtLUT::L0MPtLUT( ) {
}

//=============================================================================
// Destructor
//=============================================================================
L0MPtLUT::~L0MPtLUT() {} 

//=============================================================================
bool L0MPtLUT::read( std::string filename ) {

  std::ifstream lutfile(filename.c_str());
  if (lutfile.is_open()){
    std::string line;
    while (lutfile.good()){
      getline(lutfile,line);
      std::istringstream isline(line);
      std::string sid;
      isline >> sid;
      LHCb::MuonTileID m2pad = LHCb::MuonTileID(sid);
      m1ptMap m1pt;
      do {
        std::string s_m1pad;
        int ipt;
        isline >> s_m1pad;
        isline >> ipt;
        m1pt[LHCb::MuonTileID(s_m1pad)]=ipt;
      } while (isline);
      m_m2m1lut[m2pad]=m1pt;
    }
    lutfile.close();
  }
  else {
    return false;
  }
  return true;
}

//=============================================================================
int L0MPtLUT::getEncodedPt( LHCb::MuonTileID m2pad, LHCb::MuonTileID m1pad)
{
  int ipt = -1;
  m2m1Lut::iterator itm1m2lut = m_m2m1lut.find(m2pad);
  if (itm1m2lut !=  m_m2m1lut.end()){
    m1ptMap m1pt = (*itm1m2lut).second;
    m1ptMap::iterator itm1pt = m1pt.find(m1pad);
    if (itm1pt != m1pt.end()) {
      ipt = (*itm1pt).second;
    }
  }
  return ipt;

}

int L0MPtLUT::getEncodedPt( LHCb::MuonTileID m3pad, LHCb::MuonTileID m2pad, LHCb::MuonTileID m1pad)
{

  // Get the list of MuonTileIDs the L0MuonCandidates would return
  std::vector<LHCb::MuonTileID> mids_M3;
  mids_M3 = m3pad.layout().tiles(m3pad);
  std::vector<LHCb::MuonTileID> mids_M2;
  mids_M2 = m2pad.layout().tiles(m2pad);
  std::vector<LHCb::MuonTileID> mids_M1;
  mids_M1 = m1pad.layout().tiles(m1pad);

  // Translate as the L0MuonPtTool does
  LHCb::MuonTileID seed  = mids_M3[0];
  // get M2 pad used in the trigger
  std::vector<LHCb::MuonTileID> padsM2 = mids_M2;
  LHCb::MuonTileID padM2 = padsM2[0];
  if (padM2.region()<seed.region()) {
    LHCb::MuonTileID padM2_Reg = padsM2[0];
    padM2 = padM2_Reg.layout().tilesInRegion(padM2_Reg,seed.region())[0];
  }
  if (!padM2.isValid()){
    if (padM2.nX()>=2*padM2.layout().xGrid()) {
      std::vector< LHCb::MuonTileID > padsM2 = padM2.layout().tiles(padM2);
      padM2 = padsM2[0];
    }
  }
  // get M1 pad used in the trigger
  std::vector<LHCb::MuonTileID> padsM1 = mids_M1;
  LHCb::MuonTileID padM1 = padsM1[0];
  if (padM1.region()<seed.region()) {
    LHCb::MuonTileID padM1_Reg = padsM1[0];
    padM1 = padM1_Reg.layout().tilesInRegion(padM1_Reg,seed.region())[0];
  }
  if (!padM1.isValid()){
    if (padM1.nX()>=2*padM1.layout().xGrid()) {
      std::vector< LHCb::MuonTileID > padsM1 = padM1.layout().tiles(padM1);
      padM1 = padsM1[0];
    }
  }

  return getEncodedPt(padM2,padM1);

}

