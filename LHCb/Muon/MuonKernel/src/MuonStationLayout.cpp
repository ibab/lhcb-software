// $Id: MuonStationLayout.cpp,v 1.4 2002-02-25 10:57:43 atsareg Exp $
// Include files
#include <iostream>
#include <algorithm>
#include "MuonKernel/MuonStationLayout.h"
#include "MuonKernel/MuonTileID.h"

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

std::vector<MuonTileID> 
MuonStationLayout::tiles(const MuonTileID& pad) const {
  return tilesInArea(pad,0,0);
}

std::vector<MuonTileID> MuonStationLayout::tilesInArea(const MuonTileID& pad, 
					int areaX, int areaY) const {
					
  unsigned int reg = pad.region();
  
  std::vector<MuonTileID> result;
  std::vector<MuonTileID> vtm;
  std::vector<MuonTileID>::iterator it;
  
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

std::vector<MuonTileID> MuonStationLayout::tiles() const {
					
  std::vector<MuonTileID> result;

  for (int ir = 0; ir<4; ir++) {
    for (int iq = 0; iq<4; iq++) {
      std::vector<MuonTileID> tmp=m_layouts[ir].tiles(iq,ir);
      result.insert(result.end(),tmp.begin(),tmp.end());
    }    
  }
  return result;
}

std::vector<MuonTileID> MuonStationLayout::tiles(int iq) const {
					
  std::vector<MuonTileID> result;

  for (int ir = 0; ir<4; ir++) {
    std::vector<MuonTileID> tmp=m_layouts[ir].tiles(iq,ir);
    result.insert(result.end(),tmp.begin(),tmp.end());
  }
  return result;
}

std::vector<MuonTileID> MuonStationLayout::tiles(int iq, int ir) const {
					
  return m_layouts[ir].tiles(iq,ir);
}

std::vector<MuonTileID> MuonStationLayout::tilesInRegion(const MuonTileID& pad, 
                                                int pregion) const{
    
  int reg = pad.region();
  return m_layouts[reg].tilesInRegion(pad,pregion);
  
}  

std::vector<MuonTileID> 
MuonStationLayout::neighbours(const MuonTileID& pad) const {
  unsigned int reg = pad.region();			      
  return m_layouts[reg].neighbours(pad);
}

std::vector<MuonTileID> 
MuonStationLayout::neighbours(const MuonTileID& pad,
                              int dirX,
			      int dirY,
			      int depth) const {
			      
  std::vector<MuonTileID> result;			      
  std::vector<MuonTileID> vreg;
  std::vector<MuonTileID> vtmit;
  std::vector<MuonTileID>::iterator it,itm,itf;
  			      
  unsigned int reg = pad.region();  
  
  vreg = m_layouts[reg].neighbours(pad,dirX,dirY,depth);
  for(it = vreg.begin(); it != vreg.end(); it++) {
    if(it->region() == reg) {
      result.push_back(*it);
    } else {
      int itreg = it->region();
      vtmit = m_layouts[itreg].tiles(*it);   
      for(itm = vtmit.begin(); itm != vtmit.end(); itm++) {   
        itf = std::find(result.begin(),result.end(),*itm);
	if ( itf == result.end() ) result.push_back(*itm);
      }
    }  
  }			      
  return result;
}

// std::vector<MuonTileID> 
// MuonStationLayout::neighbours(const MuonTileID& pad,
//                               int dirX,
// 			      int dirY,
// 			      int depth) const {
// 			      
//   std::vector<MuonTileID> result;			      
//   std::vector<MuonTileID> vtm;
//   std::vector<MuonTileID>::iterator it;
//   			      
//   unsigned int reg = pad.region();  
//   
//   for(unsigned int i = 0; i<4; i++) {
//   
//     int depthX = depth;
//     int depthY = depth;
//     if(i != reg) {
//       depthX = depth*m_layouts[i].xGrid()/m_layouts[reg].xGrid();
//       depthY = depth*m_layouts[i].yGrid()/m_layouts[reg].yGrid();
//     }
// 
//     vtm=m_layouts[i].neighboursInArea(pad,dirX,dirY,
//                                       depthX,
// 				      depthY);
//     
//     if(!vtm.empty()) {
//       for(it = vtm.begin(); it != vtm.end(); it++) {
//         if(it->region() == i) result.push_back(*it);
//       }
//     }
//   } 			      
//   return result;
// }

bool MuonStationLayout::isValidID(const MuonTileID& pad) const {

  int reg = pad.region();
  return m_layouts[reg].isValidID(pad);
}

MuonTileID MuonStationLayout::contains(const MuonTileID& pad) const {
  // It is responsibility of the user to assure that the pad
  // layout is finer than the containing layout
  int reg = pad.region();
  return m_layouts[reg].contains(pad);
} 
