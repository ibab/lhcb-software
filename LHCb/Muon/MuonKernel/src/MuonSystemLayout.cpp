// $Id: MuonSystemLayout.cpp,v 1.7 2003-12-18 15:33:23 cattanem Exp $
// Include files
#include <iostream>
#include "MuonKernel/MuonSystemLayout.h"
#include "MuonKernel/MuonTileID.h"

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

std::vector<MuonTileID> 
MuonSystemLayout::tiles(const MuonTileID& pad) const {
  return tilesInArea(pad,0,0);
}

std::vector<MuonTileID> 
MuonSystemLayout::tilesInArea(const MuonTileID& pad, 
			      int areaX, int areaY) const {
					
  int st = pad.station();
  return m_station_layouts[st].tilesInArea(pad,areaX,areaY);

}

std::vector<MuonTileID> MuonSystemLayout::tiles() const {

  std::vector<MuonTileID> result;
  std::vector<MuonTileID>::iterator it;

  for (int is = 0; is<5; is++) {
    std::vector<MuonTileID> tmp=m_station_layouts[is].tiles();
    for(it = tmp.begin(); it != tmp.end(); it++ ) {
      it->setStation(is);
    }
    result.insert(result.end(),tmp.begin(),tmp.end()); 
  }					
  return result;
}

std::vector<MuonTileID> MuonSystemLayout::tiles(int iq) const {

  std::vector<MuonTileID> result;
  std::vector<MuonTileID>::iterator it;

  for (int is = 0; is<5; is++) {
    std::vector<MuonTileID> tmp=m_station_layouts[is].tiles(iq);
    for(it = tmp.begin(); it != tmp.end(); it++ ) {
      it->setStation(is);
    }
    result.insert(result.end(),tmp.begin(),tmp.end()); 
  }					
  return result;
}

std::vector<MuonTileID> MuonSystemLayout::tiles(int iq, int ir) const {

  std::vector<MuonTileID> result;
  std::vector<MuonTileID>::iterator it;

  for (int is = 0; is<5; is++) {
    std::vector<MuonTileID> tmp=m_station_layouts[is].tiles(iq,ir);
    for(it = tmp.begin(); it != tmp.end(); it++ ) {
      it->setStation(is);
    }
    result.insert(result.end(),tmp.begin(),tmp.end()); 
  }					
  return result;
}

std::vector<MuonTileID> MuonSystemLayout::tilesInRegion(const MuonTileID& pad, 
                                                int pregion) const {
    
  int st = pad.station();
  std::vector<MuonTileID> result;
  std::vector<MuonTileID>::iterator it;
  
  result=m_station_layouts[st].tilesInRegion(pad,pregion);
  for(it = result.begin(); it != result.end(); it++ ) {
    it->setStation(st);
  }
  return result;
}  

std::vector<MuonTileID> 
MuonSystemLayout::neighbours(const MuonTileID& pad) const {

  int st = pad.station();
  std::vector<MuonTileID> result;
  std::vector<MuonTileID>::iterator it;
  
  result=m_station_layouts[st].neighbours(pad);
  for(it = result.begin(); it != result.end(); it++ ) {
    it->setStation(st);
  }
  return result;  
}

std::vector<MuonTileID> 
MuonSystemLayout::neighbours(const MuonTileID& pad,
                             int dirX, int dirY) const {

  int st = pad.station();
  std::vector<MuonTileID> result;
  std::vector<MuonTileID>::iterator it;
  
  result=m_station_layouts[st].neighbours(pad,dirX,dirY);
  for(it = result.begin(); it != result.end(); it++ ) {
    it->setStation(st);
  }
  return result;
}

std::vector<MuonTileID> 
MuonSystemLayout::neighbours(const MuonTileID& pad,
                             int dirX, int dirY, int depth) const {
  int st = pad.station();
  std::vector<MuonTileID> result;
  std::vector<MuonTileID>::iterator it;
  
  result=m_station_layouts[st].neighbours(pad,dirX,dirY,depth);
  for(it = result.begin(); it != result.end(); it++ ) {
    it->setStation(st);
  }
  return result;    
}

std::vector<MuonTileID> 
MuonSystemLayout::neighboursInArea(const MuonTileID& pad,
                                   int dirX, int dirY, 
				   int depthX, int depthY) const {
  int st = pad.station();
  std::vector<MuonTileID> result;
  std::vector<MuonTileID>::iterator it;
  
  result=m_station_layouts[st].neighboursInArea(pad,dirX,dirY,depthX,depthY);
  for(it = result.begin(); it != result.end(); it++ ) {
    it->setStation(st);
  }
  return result;    
}

bool MuonSystemLayout::isValidID(const MuonTileID& pad) const {

  int st = pad.station();
  return m_station_layouts[st].isValidID(pad);
}

MuonTileID MuonSystemLayout::contains(const MuonTileID& pad) const {
  // It is responsibility of the user to assure that the pad
  // layout is finer than the containing layout
  int st = pad.station();
  MuonTileID result = m_station_layouts[st].contains(pad);
  result.setStation(st);
  return result;
} 
