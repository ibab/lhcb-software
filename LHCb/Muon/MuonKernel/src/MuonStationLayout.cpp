// $Id: MuonStationLayout.cpp,v 1.2 2002-01-31 10:13:45 atsareg Exp $
// Include files
#include <iostream>
#include "MuonKernel/MuonStationLayout.h"
#include "MuonKernel/MuonSystemID.h"

// Hack to get round usage of max
#ifdef WIN32
#include <xutility>
#else
#define _MIN min
#define _MAX max
#endif // WIN32

//------------------------------------------------------------------------------
//
/// Implementation of class :  MuonStationLayout
//
/// Author: A.Tsaregorodtsev
//
//------------------------------------------------------------------------------

MuonStationLayout::MuonStationLayout() {    
}

MuonStationLayout::MuonStationLayout(const MuonLayout& lq1,
                		     const MuonLayout& lq2,
				     const MuonLayout& lq3,
                		     const MuonLayout& lq4) {
	
    m_layouts[0] = lq1;
    m_layouts[1] = lq2;
    m_layouts[2] = lq3;
    m_layouts[3] = lq4;
            
}
MuonStationLayout::MuonStationLayout(const MuonLayout& lq) {
	    
    for (int i = 0; i<4; i++) {
      m_layouts[i] = lq;
    }
}

MuonStationLayout::~MuonStationLayout() {}

std::vector<MuonSystemID> 
MuonStationLayout::tiles(const MuonSystemID& pad) const {
  return tilesInArea(pad,0,0);
}

std::vector<MuonSystemID> MuonStationLayout::tilesInArea(const MuonSystemID& pad, 
					int areaX, int areaY) const {
					
  int reg = pad.region();
  return m_layouts[reg].tilesInArea(pad,areaX,areaY);

}

std::vector<MuonSystemID> MuonStationLayout::tiles() const {
					
  std::vector<MuonSystemID> result;

  for (int ir = 0; ir<4; ir++) {
    for (int iq = 0; iq<4; iq++) {
      std::vector<MuonSystemID> tmp=m_layouts[ir].tiles(iq,ir);
      result.insert(result.end(),tmp.begin(),tmp.end());
    }    
  }
  return result;
}

std::vector<MuonSystemID> MuonStationLayout::tilesInRegion(const MuonSystemID& pad, 
                                                int pregion) const{
    
  int reg = pad.region();
  return m_layouts[reg].tilesInRegion(pad,pregion);
  
}  

bool MuonStationLayout::validID(const MuonSystemID& pad) const {

  int reg = pad.region();
  return m_layouts[reg].validID(pad);
}

MuonSystemID MuonStationLayout::contains(const MuonSystemID& pad) const {
  // It is responsibility of the user to assure that the pad
  // layout is finer than the containing layout
  int reg = pad.region();
  return m_layouts[reg].contains(pad);
} 
