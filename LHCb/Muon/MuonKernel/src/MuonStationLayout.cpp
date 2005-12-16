// $Id: MuonStationLayout.cpp,v 1.8 2005-12-16 15:57:59 asatta Exp $
// Include files
#include <iostream>
#include <algorithm>
#include "MuonKernel/MuonStationLayout.h"
#include "Kernel/MuonTileID.h"

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

std::vector<LHCb::MuonTileID> 
MuonStationLayout::tiles(const LHCb::MuonTileID& pad) const {
  return tilesInArea(pad,0,0);
}

std::vector<LHCb::MuonTileID> MuonStationLayout::tilesInArea(const LHCb::MuonTileID& pad, 
					int areaX, int areaY) const {
					
  unsigned int reg = pad.region();
  
  std::vector<LHCb::MuonTileID> result;
  std::vector<LHCb::MuonTileID> vtm;
  std::vector<LHCb::MuonTileID>::iterator it;
  
  for(unsigned int i = 0; i<4; i++) {
  
    int factorX = 1;
    int factorY = 1;
    if(i != reg) {
      factorX = m_layouts[i].xGrid()/m_layouts[reg].xGrid();
      factorY = m_layouts[i].yGrid()/m_layouts[reg].yGrid();
    }
  
    vtm=m_layouts[i].tilesInArea(pad,areaX*factorX,areaY*factorY);
    if(!vtm.empty()) {
      for(it = vtm.begin(); it != vtm.end(); it++) {
        if(it->region() == i) result.push_back(*it);
      }
    }
  }  
  return result;
}

std::vector<LHCb::MuonTileID> MuonStationLayout::tiles() const {
					
  std::vector<LHCb::MuonTileID> result;

  for (int ir = 0; ir<4; ir++) {
    for (int iq = 0; iq<4; iq++) {
      std::vector<LHCb::MuonTileID> tmp=m_layouts[ir].tiles(iq,ir);
      result.insert(result.end(),tmp.begin(),tmp.end());
    }    
  }
  return result;
}

std::vector<LHCb::MuonTileID> MuonStationLayout::tiles(int iq) const {
					
  std::vector<LHCb::MuonTileID> result;

  for (int ir = 0; ir<4; ir++) {
    std::vector<LHCb::MuonTileID> tmp=m_layouts[ir].tiles(iq,ir);
    result.insert(result.end(),tmp.begin(),tmp.end());
  }
  return result;
}

std::vector<LHCb::MuonTileID> MuonStationLayout::tiles(int iq, int ir) const {
					
  return m_layouts[ir].tiles(iq,ir);
}

std::vector<LHCb::MuonTileID> MuonStationLayout::tilesInRegion(const LHCb::MuonTileID& pad, 
                                                int pregion) const{
    
  int reg = pad.region();
  return m_layouts[reg].tilesInRegion(pad,pregion);
  
}  

std::vector<LHCb::MuonTileID> 
MuonStationLayout::neighbours(const LHCb::MuonTileID& pad) const {

  std::vector<LHCb::MuonTileID> result;			      
  std::vector<LHCb::MuonTileID> vreg;
  std::vector<LHCb::MuonTileID>::iterator it;
  			        
  for ( unsigned int ireg = 0; ireg < 4; ireg++ ) {
    vreg = m_layouts[ireg].neighbours(pad);
    for(it = vreg.begin(); it != vreg.end(); it++) {
      if(it->region() == ireg) {
	result.push_back(*it);
      }  
    }
  }  		
  return result;
}

std::vector<LHCb::MuonTileID> 
MuonStationLayout::neighbours(const LHCb::MuonTileID& pad,
                              int dirX,
			      int dirY) const {
  
//  This function returns all the LHCb::MuonTileID's which are neighbours
//  of the given pad in the direction indicated by dirX and dirY and 
//  defined in terms of this layout. 

  unsigned int nreg = pad.region();
  std::vector<LHCb::MuonTileID> vtm = neighbours(pad,dirX,dirY,1);
  // if no neigbours at all
  if(vtm.empty()) return vtm;
  // if the neigbours are all in the same region or larger region
  if(vtm[0].region() >= nreg) return vtm;
  // if there is only one neighbour
  if(vtm.size() == 1) return vtm;
  
  // We have got to the smaller region - make use of the corresponding
  // function in this region's MuonLayout
  nreg = vtm[0].region();
  return m_layouts[nreg].neighbours(pad,dirX,dirY);			      
}			      

std::vector<LHCb::MuonTileID> 
MuonStationLayout::neighbours(const LHCb::MuonTileID& pad,
                              int dirX,
			      int dirY,
			      int depth) const {
			      
  std::vector<LHCb::MuonTileID> result;			      
  std::vector<LHCb::MuonTileID> vreg;
  std::vector<LHCb::MuonTileID>::iterator it;
  			        
  for ( unsigned int ireg = 0; ireg < 4; ireg++ ) {
    vreg = m_layouts[ireg].neighbours(pad,dirX,dirY,depth);
    for(it = vreg.begin(); it != vreg.end(); it++) {
      if(it->region() == ireg) {
	result.push_back(*it);
      }  
    }
  }  		      
  return result;
}

std::vector<LHCb::MuonTileID> 
MuonStationLayout::neighboursInArea(const LHCb::MuonTileID& pad,
                        	    int dirX,
				    int dirY,
				    int depthX,
				    int depthY) const {
				    
  std::vector<LHCb::MuonTileID> result;			      
  std::vector<LHCb::MuonTileID> vreg;
  std::vector<LHCb::MuonTileID>::iterator it;
  			        
  for ( unsigned int ireg = 0; ireg < 4; ireg++ ) {
    vreg = m_layouts[ireg].neighboursInArea(pad,dirX,dirY,depthX,depthY);
    for(it = vreg.begin(); it != vreg.end(); it++) {
      if(it->region() == ireg) {
	result.push_back(*it);
      }  
    }
  }  		      
  return result;
}				    

bool MuonStationLayout::isValidID(const LHCb::MuonTileID& pad) const {

  int reg = pad.region();
  return m_layouts[reg].isValidID(pad);
}

LHCb::MuonTileID MuonStationLayout::contains(const LHCb::MuonTileID& pad) const {
  // It is responsibility of the user to assure that the pad
  // layout is finer than the containing layout
  int reg = pad.region();
  LHCb::MuonTileID tile = m_layouts[reg].contains(pad);
  return tile;
} 
