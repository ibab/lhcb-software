// $Id: MuonLayout.cpp,v 1.4 2002-02-25 10:57:43 atsareg Exp $
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

int MuonLayout::rfactor(int nr) const {

  int rfac = 1 << nr;
  return rfac;
}  

int MuonLayout::region(int ix, int iy) const {

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

  int quarter = pad.quarter();
  int nreg = pad.region();

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
  int nr, nrx;
  
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
	int newx = ix/rfactor(nr)/xratio; 	  
	int newy = iy/rfactor(nr)/yratio;
	if(newy < 2*yGrid() && newx < 2*xGrid()) {
          MuonTileID newtile(0,0,0,*this,nr,quarter,newx,newy); 
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
  
//   std::sort(vmt.begin(),vmt.end());
//   std::vector<MuonTileID>::iterator uvmt;
//   uvmt = std::unique(vmt.begin(),vmt.end());
//   int nc = vmt.end()-uvmt;
//   for(int iv = 0; iv < nc; iv++) vmt.pop_back();

  return vmt;
}

std::vector<MuonTileID> MuonLayout::tiles() const {
  
  std::vector<MuonTileID> vmt;
  std::vector<MuonTileID> vmtr;
  
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
  
  for(int ir = 0; ir <4; ir++) {
    vmtr = tiles(iq,ir);
    vmt.insert(vmt.end(),vmtr.begin(),vmtr.end());
  }
  
  return vmt;
  
}

std::vector<MuonTileID> MuonLayout::tiles(int iq, int ir) const {

  std::vector<MuonTileID> vmt;
  vmt.reserve(xGrid()*yGrid()*3);
  
  MuonTileID newpad;
  
  int ix; int iy;
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
    
  int nr = pad.region();
  int nq = pad.quarter();
  
  if(nr == pregion) {
    return tiles(pad);    
  } else {
    std::vector<MuonTileID> vmt = tiles(pad); 
    std::vector<MuonTileID>::iterator ivmt;
    std::vector<MuonTileID> nvmt;
    // Bring the pads in vmt to the pregion definition
    for (ivmt = vmt.begin(); ivmt != vmt.end(); ivmt++) {      
      if(nr<pregion) {
	int factor = rfactor(pregion)/rfactor(nr);
	int newX = ivmt->nX()/factor;
	int newY = ivmt->nY()/factor;
	MuonTileID tile(0,0,0,*this,nq,pregion,newX,newY);
	nvmt.push_back(tile);
      } else {
        int factor = rfactor(nr)/rfactor(pregion);
	int minX = ivmt->nX()*factor;
	int minY = ivmt->nY()*factor;
        for(int ix=0; ix<factor; ix++) {
	  for(int iy=0; iy<factor; iy++) {
	    MuonTileID tile(0,0,0,*this,nq,pregion,minX+ix,minY+iy);
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
  vmt = neighbours(pad,MuonBase::RIGHT,MuonBase::DOWN,1);
  result.insert(result.end(),vmt.begin(),vmt.end());
  vmt = neighbours(pad,MuonBase::RIGHT,MuonBase::CENTER,1);
  result.insert(result.end(),vmt.begin(),vmt.end());	
  vmt = neighbours(pad,MuonBase::RIGHT,MuonBase::UP,1);
  result.insert(result.end(),vmt.begin(),vmt.end());
  vmt = neighbours(pad,MuonBase::LEFT,MuonBase::DOWN,1);
  result.insert(result.end(),vmt.begin(),vmt.end());
  vmt = neighbours(pad,MuonBase::LEFT,MuonBase::CENTER,1);
  result.insert(result.end(),vmt.begin(),vmt.end());	
  vmt = neighbours(pad,MuonBase::LEFT,MuonBase::UP,1);
  result.insert(result.end(),vmt.begin(),vmt.end());	
  vmt = neighbours(pad,MuonBase::CENTER,MuonBase::UP,1);
  result.insert(result.end(),vmt.begin(),vmt.end());	
  vmt = neighbours(pad,MuonBase::CENTER,MuonBase::DOWN,1);
  result.insert(result.end(),vmt.begin(),vmt.end());
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

  // MuonLayout playout = pad.layout();
//   std::vector<MuonTileID> vmt;
// 
//   int quarter = pad.quarter();
//   int nreg = pad.region();
// 
//   // the finest grid of the two layouts
//   int mxgrid = std::_MAX(m_xgrid, playout.xGrid() );
//   int mygrid = std::_MAX(m_ygrid, playout.yGrid() );
// 
//   // cout << mxgrid << mygrid << endl;	    
// 
//   MuonLayout fineGrid(mxgrid,mygrid);
// 
//   int xratio = m_xgrid/playout.xGrid();
//   int yratio = m_ygrid/playout.yGrid();
// 
//   xratio = (xratio == 0) ? 1 : xratio ;
//   yratio = (yratio == 0) ? 1 : yratio ;
// 
//   // input pad area in terms of the finest grid and smallest region  
//   // depending on the direction of the search
//  
//   int maxX,maxY,minX,minY;
//   
//   if(dirX == MuonBase::RIGHT) {
//     maxX = (pad.nX()+1)*rfactor(nreg)*xratio+depth*rfactor(nreg)-1;
//     minX = (pad.nX()+1)*rfactor(nreg)*xratio;
//   } else if (dirX == MuonBase::LEFT) {
//     maxX = pad.nX()*rfactor(nreg)*xratio-1;
//     minX = pad.nX()*rfactor(nreg)*xratio - depth*rfactor(nreg) ;
//   } else {
//     maxX = (pad.nX()+1)*rfactor(nreg)*xratio-1;
//     minX = (pad.nX())*rfactor(nreg)*xratio;
//   } 
//   
//   if(dirY == MuonBase::UP) {
//     maxY = (pad.nY()+1)*rfactor(nreg)*yratio+depth*rfactor(nreg)-1;
//     minY = (pad.nY()+1)*rfactor(nreg)*yratio;
//   } else if (dirY == MuonBase::DOWN){
//     maxY = pad.nY()*rfactor(nreg)*yratio-1;
//     minY = pad.nY()*rfactor(nreg)*yratio - depth*rfactor(nreg) ;
//   } else {
//     maxY = (pad.nY()+1)*rfactor(nreg)*yratio-1;
//     minY = (pad.nY())*rfactor(nreg)*yratio;
//   }
//     
//   minX = std::_MAX(0,minX);
//   minY = std::_MAX(0,minY);
// 
//   //cout << minX << " " << minY << " " << maxX << " " << maxY << " " << endl;
// 
//   // Which tiles are hit ?
// 
//   xratio = mxgrid/m_xgrid;
//   yratio = mygrid/m_ygrid;
// 
//   int ix = minX;
//   int iy = minY;
//   bool regleap = false;
//   int nr, nrx;
//   
//   while ( ix <= maxX ) {
//     nrx = fineGrid.region(ix,iy);
//     if( nrx == -1 ) nrx = 0;
//     while ( iy <= maxY ) {
//       // which region	
//       nr = fineGrid.region(ix,iy);
//       if( nr == -1 ) nr = 0;
//       if(nr == nrx || !regleap) {
// 	int newx = ix/rfactor(nr)/xratio; 	  
// 	int newy = iy/rfactor(nr)/yratio;
// 	MuonTileID newtile(0,0,0,*this,nr,quarter,newx,newy);
// 	if (newtile.isValid()) {
// 	  vmt.push_back(newtile);
// 	}  
//       }		
//       iy += rfactor(nr)*yratio;
//     }   
//     if(regleap) {
//       regleap = false;
//     } else if (nr != nrx) {
//       regleap = true;	
//     } 
//     iy = minY;
//     ix += rfactor(nrx)*xratio;
//   }  
//   return vmt;					       
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
  int nr, nrx;
  
  while ( ix <= maxX ) {
    nrx = fineGrid.region(ix,iy);
    if( nrx == -1 ) nrx = 0;
    while ( iy <= maxY ) {
      // which region	
      nr = fineGrid.region(ix,iy);
      if( nr == -1 ) nr = 0;
      if(nr == nrx || !regleap) {
	int newx = ix/rfactor(nr)/xratio; 	  
	int newy = iy/rfactor(nr)/yratio;
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

  int nx = mt.nX();
  int ny = mt.nY();
  
  if(nx >= 0 && 
     nx < (2*xGrid()) && 
     ny >= (yGrid()) && 
     ny < 2*yGrid()) {
    return true;
  }
  if(nx >= (xGrid()) && 
     nx < 2*xGrid() && 
     ny >= 0 && 
     ny < yGrid()) {
    return true;
  }
  return false;
}

MuonTileID MuonLayout::contains(const MuonTileID& pad) const {
  // It is responsibility of the user to assure that the pad
  // layout is finer than the containing layout
  std::vector<MuonTileID> mtiles = tiles(pad);
  return mtiles[0];
} 
