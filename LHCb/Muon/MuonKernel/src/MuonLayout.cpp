// $Id: MuonLayout.cpp,v 1.8 2003-06-16 13:48:01 sponce Exp $
// Include files
#include <iostream>
#include <algorithm>
#include "MuonKernel/MuonLayout.h"
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
/// Implementation of class :  MuonLayout
//
/// Author: A.Tsaregorodtsev
//
//------------------------------------------------------------------------------

MuonLayout::MuonLayout() {	
    m_xgrid = 0;
    m_ygrid = 0;        
}

MuonLayout::MuonLayout(unsigned int x, unsigned int y) {
    m_xgrid = x;
    m_ygrid = y;       
}

MuonLayout::MuonLayout(std::pair<unsigned int, unsigned int> grid) {	
    m_xgrid = grid.first;
    m_ygrid = grid.second;       
}

MuonLayout::~MuonLayout() {} 

int MuonLayout::region(unsigned int ix, unsigned int iy) const {

  int nrx;
  if ( ix/m_xgrid < 1 ) {
    nrx = -1;
  } else if ( ix/m_xgrid == 1 ) {
    nrx = 0;
  } else if ( ix/m_xgrid <= 3 ) {
    nrx = 1;  
  } else if ( ix/m_xgrid <= 7 ) {
    nrx = 2; 	
  } else {
    nrx = 3; 	
  }
  int nry;
  if ( iy/m_ygrid < 1 ) {
    nry = -1;
  } else if ( iy/m_ygrid == 1 ) {
    nry = 0;
  } else if ( iy/m_ygrid <= 3 ) {
    nry = 1;  
  } else if ( iy/m_ygrid <= 7 ) {
    nry = 2; 	
  } else {
    nry = 3; 	
  }	
  return std::_MAX(nrx,nry);
    
}

std::vector<MuonTileID> MuonLayout::tiles(const MuonTileID& pad) const {
  return tilesInArea(pad,0,0);
}

std::vector<MuonTileID> MuonLayout::tilesInArea(const MuonTileID& pad, 
					          int areaX, 
						  int areaY) const {
					
//  This function returns all the MuonTileID's touched by the "pad"
//  defined with the "playout" MuonLayout +- areaX and +- areaY.
//  The areaX and areaY are defined in terms of this layout.


  MuonLayout playout = pad.layout();
  std::vector<MuonTileID> vmt;
  
  if( ! isDefined() ) return vmt;

  int quarter = pad.quarter();
  int nreg = pad.region();
  int sta = pad.station();

  // cout << quarter << " " << nreg << " " << rfactor(nreg) << endl;	
  // cout << m_xgrid << m_ygrid << endl;		
  // cout << playout.xGrid() << playout.yGrid() << endl;	

  // the finest grid of the two layouts
  int mxgrid = std::_MAX(m_xgrid, playout.xGrid() );
  int mygrid = std::_MAX(m_ygrid, playout.yGrid() );

  // cout << mxgrid << mygrid << endl;	    

  MuonLayout fineGrid(mxgrid,mygrid);

  int xratio = m_xgrid/playout.xGrid();
  int yratio = m_ygrid/playout.yGrid();

  xratio = (xratio == 0) ? 1 : xratio ;
  yratio = (yratio == 0) ? 1 : yratio ;

  // input pad area in terms of the finest grid
//   int maxX = (pad.nX()+areaX+1)*playout.rfactor(nreg)*xratio-1;
//   int maxY = (pad.nY()+areaY+1)*playout.rfactor(nreg)*yratio-1;
//   int minX = maxX - playout.rfactor(nreg)*xratio*(2*areaX+1) + 1;
//   int minY = maxY - playout.rfactor(nreg)*yratio*(2*areaY+1) + 1;

  // input pad area in terms of the finest grid and smallest region  
  int maxX = (pad.nX()+1)*rfactor(nreg)*xratio+areaX*rfactor(nreg)-1;
  int maxY = (pad.nY()+1)*rfactor(nreg)*yratio+areaY*rfactor(nreg)-1;
  int minX = maxX - rfactor(nreg)*xratio - 2*areaX*rfactor(nreg) + 1;
  int minY = maxY - rfactor(nreg)*yratio - 2*areaY*rfactor(nreg) + 1;
  
  
  minX = std::_MAX(0,minX);
  minY = std::_MAX(0,minY);

  // Which tiles are hit ?

  xratio = mxgrid/m_xgrid;
  yratio = mygrid/m_ygrid;
  
  int ix = minX;
  int iy = minY;
  bool regleap = false;
  int nr = 0;
  int nrx = 0;
  
  while ( ix <= maxX ) {
    nrx = fineGrid.region(ix,iy);
    if( nrx == -1 ) nrx = 0;
    if( nrx > 3 ) break;
    while ( iy <= maxY ) {
      // which region	
      nr = fineGrid.region(ix,iy);
      if( nr == -1 ) nr = 0;
      if( nr > 3 ) break;                    
      if(nr == nrx || !regleap) {
	unsigned int newx = ix/rfactor(nr)/xratio; 	  
	unsigned int newy = iy/rfactor(nr)/yratio;
	if(newy < 2*yGrid() && newx < 2*xGrid()) {
          MuonTileID newtile(sta,0,0,*this,nr,quarter,newx,newy); 
	  if(newtile.isValid()) {
	    vmt.push_back(newtile);
	  } 
	}   
      }		
      iy += rfactor(nr)*yratio;
    }   
    if(regleap) {
      regleap = false;
    } else if (nr != nrx) {
      regleap = true;	
    } 
    iy = minY;
    ix += rfactor(nrx)*xratio;
  }    
  return vmt;
}

std::vector<MuonTileID> MuonLayout::tiles() const {
  
  std::vector<MuonTileID> vmt;
  std::vector<MuonTileID> vmtr;
  
  if( ! isDefined() ) return vmt;
  
  for(int iq = 0; iq <4; iq++) {
    for(int ir = 0; ir <4; ir++) {
      vmtr = tiles(iq,ir);
      vmt.insert(vmt.end(),vmtr.begin(),vmtr.end());
    }
  }
  
  return vmt;
  
}

std::vector<MuonTileID> MuonLayout::tiles(int iq) const {
  
  std::vector<MuonTileID> vmt;
  std::vector<MuonTileID> vmtr;
  
  if( ! isDefined() ) return vmt;
    
  for(int ir = 0; ir <4; ir++) {
    vmtr = tiles(iq,ir);
    vmt.insert(vmt.end(),vmtr.begin(),vmtr.end());
  }
  
  return vmt;
  
}

std::vector<MuonTileID> MuonLayout::tiles(int iq, int ir) const {

  std::vector<MuonTileID> vmt;
  if( ! isDefined() ) return vmt;
  
  vmt.reserve(xGrid()*yGrid()*3);
  
  MuonTileID newpad;
  
  unsigned int ix; unsigned int iy;
  for(ix = 0; ix < 2*xGrid(); ix++) {
    for(iy = yGrid(); iy < 2*yGrid(); iy++) {
      vmt.push_back(MuonTileID(0,0,0,*this,ir,iq,ix,iy));
    }
  }
  for(ix = xGrid(); ix < 2*xGrid(); ix++) {
    for(iy = 0; iy < yGrid(); iy++) {
      vmt.push_back(MuonTileID(0,0,0,*this,ir,iq,ix,iy));
    }
  }
  return vmt; 
}

std::vector<MuonTileID> MuonLayout::tilesInRegion(const MuonTileID& pad, 
                                                int pregion) const{
						
  std::vector<MuonTileID> vmt;
  if( ! isDefined() ) return vmt;						
    
  int nr = pad.region();
  int nq = pad.quarter();
  int sta = pad.station();
  
  if(nr == pregion) {
    return tiles(pad);    
  } else {
    vmt = tiles(pad); 
    std::vector<MuonTileID>::iterator ivmt;
    std::vector<MuonTileID> nvmt;
    // Bring the pads in vmt to the pregion definition
    for (ivmt = vmt.begin(); ivmt != vmt.end(); ivmt++) {      
      if(nr<pregion) {
	int factor = rfactor(pregion)/rfactor(nr);
	int newX = ivmt->nX()/factor;
	int newY = ivmt->nY()/factor;
	MuonTileID tile(sta,0,0,*this,nq,pregion,newX,newY);
	nvmt.push_back(tile);
      } else {
        int factor = rfactor(nr)/rfactor(pregion);
	int minX = ivmt->nX()*factor;
	int minY = ivmt->nY()*factor;
        for(int ix=0; ix<factor; ix++) {
	  for(int iy=0; iy<factor; iy++) {
	    MuonTileID tile(sta,0,0,*this,nq,pregion,minX+ix,minY+iy);
	    nvmt.push_back(tile);
	  }
	}
      }
    }
    return nvmt;
  }
}

std::vector<MuonTileID> MuonLayout::neighbours(const MuonTileID& pad) const {

  // This function returns all the neighbours within unity depth
  std::vector<MuonTileID> result;
  std::vector<MuonTileID> vmt;
  vmt = neighbours(pad,MuonBase::RIGHT,MuonBase::DOWN);
  result.insert(result.end(),vmt.begin(),vmt.end());
  vmt = neighbours(pad,MuonBase::RIGHT,MuonBase::CENTER);
  result.insert(result.end(),vmt.begin(),vmt.end());	
  vmt = neighbours(pad,MuonBase::RIGHT,MuonBase::UP);
  result.insert(result.end(),vmt.begin(),vmt.end());
  vmt = neighbours(pad,MuonBase::LEFT,MuonBase::DOWN);
  result.insert(result.end(),vmt.begin(),vmt.end());
  vmt = neighbours(pad,MuonBase::LEFT,MuonBase::CENTER);
  result.insert(result.end(),vmt.begin(),vmt.end());	
  vmt = neighbours(pad,MuonBase::LEFT,MuonBase::UP);
  result.insert(result.end(),vmt.begin(),vmt.end());	
  vmt = neighbours(pad,MuonBase::CENTER,MuonBase::UP);
  result.insert(result.end(),vmt.begin(),vmt.end());	
  vmt = neighbours(pad,MuonBase::CENTER,MuonBase::DOWN);
  result.insert(result.end(),vmt.begin(),vmt.end());
  return result;			     
}

std::vector<MuonTileID> MuonLayout::neighbours(const MuonTileID& pad,
                                               int dirX,
					       int dirY) const {
					       
//  This function returns all the MuonTileID's which are neighbours
//  of the given pad in the direction indicated by dirX and dirY and 
//  defined in terms of this layout. 

  unsigned int nreg = pad.region();
  std::vector<MuonTileID> vtm = neighbours(pad,dirX,dirY,1);
  // if no neigbours at all
  if(vtm.empty()) return vtm;
  // if the neigbours are all in the same region or the neighboring 
  // region is larger than the pad's one
  if(vtm[0].region() >= nreg) return vtm;
  // if there is only one neighbour
  if(vtm.size() == 1) return vtm;
  
  // We have got to the smaller region
  nreg = vtm[0].region();
  std::vector<MuonTileID> xvtm,result;
  std::vector<MuonTileID>::iterator ivtm;
  // Find the limits of the returned pad's area
  unsigned int minx=9999;
  unsigned int maxx=0;
  unsigned int miny=9999;
  unsigned int maxy=0;
  for ( ivtm = vtm.begin(); ivtm != vtm.end(); ivtm++) {
    if( (*ivtm).nY() > maxy ) maxy = (*ivtm).nY();
    if( (*ivtm).nX() > maxx ) maxx = (*ivtm).nX();
    if( (*ivtm).nY() < miny ) miny = (*ivtm).nY();
    if( (*ivtm).nX() < minx ) minx = (*ivtm).nX();
  }
  
  // perform the check in the X direction first
  if ( dirX == MuonBase::LEFT ) {
    for ( ivtm = vtm.begin(); ivtm != vtm.end(); ivtm++) {
      if( (*ivtm).nX() == maxx ) {
        xvtm.push_back(*ivtm);
      }
    }
  } else if ( dirX == MuonBase::RIGHT ) {
    for ( ivtm = vtm.begin(); ivtm != vtm.end(); ivtm++) {
      if( (*ivtm).nX() == minx ) {
        xvtm.push_back(*ivtm);
      }
    }  
  } else {
    for ( ivtm = vtm.begin(); ivtm != vtm.end(); ivtm++) {
      xvtm.push_back(*ivtm);
    } 
  }
    
  // check in the Y direction with the pads that are left 
  if ( dirY == MuonBase::DOWN ) {
    for ( ivtm = xvtm.begin(); ivtm != xvtm.end(); ivtm++) {
      if( (*ivtm).nY() == maxy ) {
        result.push_back(*ivtm);
      }
    }
  } else if ( dirY == MuonBase::UP ) {
    for ( ivtm = xvtm.begin(); ivtm != xvtm.end(); ivtm++) {
      if( (*ivtm).nY() == miny ) {
        result.push_back(*ivtm);
      }
    }  
  } else {
    for ( ivtm = xvtm.begin(); ivtm != xvtm.end(); ivtm++) {
      result.push_back(*ivtm);
    } 
  }
  return result;  					       				       
}

std::vector<MuonTileID> MuonLayout::neighbours(const MuonTileID& pad,
                                               int dirX,
					       int dirY,
					       int depth) const {
					       
//  This function returns all the MuonTileID's touched by the area
//  situated in the direction indicated by dirX and dirY and defined
//  in terms of this layout. The depth of the area is defined by
//  the depth parameter

  return neighboursInArea(pad,dirX,dirY,depth,depth);
					       
}

//========================================================================
std::vector<MuonTileID> MuonLayout::neighboursInArea(const MuonTileID& pad,
                                        	     int dirX,
						     int dirY,
						     int depthX,
						     int depthY) const {
					       
//  This function returns all the MuonTileID's touched by the area
//  situated in the direction indicated by dirX and dirY and defined
//  in terms of this layout. The depth of the area is defined by
//  the depthX and depthY parameters. It is identical to the neighbours()
//  apart from the corner neighbours.


  MuonLayout playout = pad.layout();
  std::vector<MuonTileID> vmt;
  
  if ( ! isDefined() ) return vmt;

  int quarter = pad.quarter();
  int nreg = pad.region();

  // the finest grid of the two layouts
  int mxgrid = std::_MAX(m_xgrid, playout.xGrid() );
  int mygrid = std::_MAX(m_ygrid, playout.yGrid() );

  // cout << mxgrid << mygrid << endl;	    

  MuonLayout fineGrid(mxgrid,mygrid);

  int xratio = m_xgrid/playout.xGrid();
  int yratio = m_ygrid/playout.yGrid();

  xratio = (xratio == 0) ? 1 : xratio ;
  yratio = (yratio == 0) ? 1 : yratio ;

  // input pad area in terms of the finest grid and smallest region  
  // depending on the direction of the search
 
  int maxX,maxY,minX,minY;
  
  if(dirX == MuonBase::RIGHT) {
    maxX = (pad.nX()+1)*rfactor(nreg)*xratio+depthX*rfactor(nreg)-1;
    minX = (pad.nX()+1)*rfactor(nreg)*xratio;
  } else if (dirX == MuonBase::LEFT) {
    maxX = pad.nX()*rfactor(nreg)*xratio-1;
    minX = pad.nX()*rfactor(nreg)*xratio - depthX*rfactor(nreg) ;
  } else {
    maxX = (pad.nX()+1)*rfactor(nreg)*xratio-1;
    minX = (pad.nX())*rfactor(nreg)*xratio;
  } 
  
  if(dirY == MuonBase::UP) {
    maxY = (pad.nY()+1)*rfactor(nreg)*yratio+depthY*rfactor(nreg)-1;
    minY = (pad.nY()+1)*rfactor(nreg)*yratio;
  } else if (dirY == MuonBase::DOWN){
    maxY = pad.nY()*rfactor(nreg)*yratio-1;
    minY = pad.nY()*rfactor(nreg)*yratio - depthY*rfactor(nreg) ;
  } else {
    maxY = (pad.nY()+1)*rfactor(nreg)*yratio-1;
    minY = (pad.nY())*rfactor(nreg)*yratio;
  }
    
  minX = std::_MAX(0,minX);
  minY = std::_MAX(0,minY);

  //cout << minX << " " << minY << " " << maxX << " " << maxY << " " << endl;

  // Which tiles are hit ?

  xratio = mxgrid/m_xgrid;
  yratio = mygrid/m_ygrid;

  int ix = minX;
  int iy = minY;
  bool regleap = false;
  int nr = 0;
  int nrx = 0;
  
  while ( ix <= maxX ) {
    nrx = fineGrid.region(ix,iy);
    if( nrx == -1 ) nrx = 0;
    while ( iy <= maxY ) {
      // which region	
      nr = fineGrid.region(ix,iy);
      if( nr == -1 ) nr = 0;
      if(nr == nrx || !regleap) {
	unsigned int newx = ix/rfactor(nr)/xratio; 	  
	unsigned int newy = iy/rfactor(nr)/yratio;
	// New tile should not come out of its region
	if(newy < 2*yGrid() && newx < 2*xGrid()) {
	  MuonTileID newtile(0,0,0,*this,nr,quarter,newx,newy);
	  if (newtile.isValid()) {
	    vmt.push_back(newtile);
	  }
	}  
      }		
      iy += rfactor(nr)*yratio;
    }   
    if(regleap) {
      regleap = false;
    } else if (nr != nrx) {
      regleap = true;	
    } 
    iy = minY;
    ix += rfactor(nrx)*xratio;
  }  
  return vmt;					       
}

//==========================================================
bool MuonLayout::isValidID(const MuonTileID& mt) const {

  if(mt.layout() != *this) return false;

  int nx = mt.nX();
  int ny = mt.nY();
  int xg = xGrid();
  int yg = yGrid();
  
  if(nx >= 0 && 
     nx < (2*xg) && 
     ny >= (yg) && 
     ny < 2*yg) {
    return true;
  }
  if(nx >= xg && 
     nx < 2*xg && 
     ny >= 0 && 
     ny < yg) {
    return true;
  }
  return false;
}

MuonTileID MuonLayout::contains(const MuonTileID& pad) const {
  // It is responsibility of the user to assure that the pad
  // layout is finer than the containing layout
  std::vector<MuonTileID> mtiles = tiles(pad);
  if( mtiles.empty() ) return MuonTileID();
  MuonTileID tile = mtiles[0];
  return tile;
} 

bool MuonLayout::isDefined() const {
  return xGrid() > 0 && yGrid() > 0;
}
