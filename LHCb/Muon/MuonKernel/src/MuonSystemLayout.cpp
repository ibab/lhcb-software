// $Id: MuonSystemLayout.cpp,v 1.2 2002-01-31 10:13:45 atsareg Exp $
// Include files
#include <iostream>
#include "MuonKernel/MuonSystemLayout.h"
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
/// Implementation of class :  MuonSystemLayout
//
/// Author: A.Tsaregorodtsev
//
//------------------------------------------------------------------------------

MuonSystemLayout::MuonSystemLayout() {    
}

MuonSystemLayout::MuonSystemLayout(const MuonStationLayout& lq1,
                		   const MuonStationLayout& lq2,
				   const MuonStationLayout& lq3,
                		   const MuonStationLayout& lq4,
				   const MuonStationLayout& lq5) {
	
    m_station_layouts[0] = lq1;
    m_station_layouts[1] = lq2;
    m_station_layouts[2] = lq3;
    m_station_layouts[3] = lq4;
    m_station_layouts[4] = lq5;           
}

MuonSystemLayout::MuonSystemLayout(const MuonStationLayout& lq) {
	
    for (int i = 0; i<5; i++ ) {
      m_station_layouts[i] = lq;
    }        
}

MuonSystemLayout::~MuonSystemLayout() {}

std::vector<MuonSystemID> 
MuonSystemLayout::tiles(const MuonSystemID& pad) const {
  return tilesInArea(pad,0,0);
}

std::vector<MuonSystemID> 
MuonSystemLayout::tilesInArea(const MuonSystemID& pad, 
			      int areaX, int areaY) const {
					
  int st = pad.station();
  return m_station_layouts[st].tilesInArea(pad,areaX,areaY);

}

std::vector<MuonSystemID> MuonSystemLayout::tiles() const {

  std::vector<MuonSystemID> result;
  std::vector<MuonSystemID>::iterator it;

  for (int is = 0; is<5; is++) {
    std::vector<MuonSystemID> tmp=m_station_layouts[is].tiles();
    for(it = tmp.begin(); it != tmp.end(); it++ ) {
      it->setStation(is);
    }
    result.insert(result.end(),tmp.begin(),tmp.end()); 
  }					
  return result;
}

std::vector<MuonSystemID> MuonSystemLayout::tilesInRegion(const MuonSystemID& pad, 
                                                int pregion) const {
    
  int st = pad.station();
  return m_station_layouts[st].tilesInRegion(pad,pregion);
  
}  

bool MuonSystemLayout::validID(const MuonSystemID& pad) const {

  int st = pad.station();
  return m_station_layouts[st].validID(pad);
}

MuonSystemID MuonSystemLayout::contains(const MuonSystemID& pad) const {
  // It is responsibility of the user to assure that the pad
  // layout is finer than the containing layout
  int st = pad.station();
  return m_station_layouts[st].contains(pad);
} 
