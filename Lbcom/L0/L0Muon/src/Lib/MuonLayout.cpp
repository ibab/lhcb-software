// $Id: MuonLayout.cpp,v 1.6 2002-01-14 10:01:04 cattanem Exp $
// Include files
#include <iostream>
#include "L0Muon/MuonLayout.h"
#include "L0Muon/MuonTile.h"

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

MuonLayout::MuonLayout(int x, int y) {
	
    m_xgrid = x;
    m_ygrid = y;    
    
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

std::vector<MuonTile> MuonLayout::tiles(const MuonTile& pad, 
					int areaX, int areaY) {
					
//  This function returns all the MuonTile's touched by the "pad"
//  defined with the "playout" MuonLayout +- areaX and +- areaY	


  MuonLayout playout = pad.layout();
  std::vector<MuonTile> vmt;

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
  int maxX = (pad.nX()+areaX+1)*playout.rfactor(nreg)*xratio-1;
  int maxY = (pad.nY()+areaY+1)*playout.rfactor(nreg)*yratio-1;
  int minX = maxX - playout.rfactor(nreg)*xratio*(2*areaX+1) + 1;
  int minY = maxY - playout.rfactor(nreg)*yratio*(2*areaY+1) + 1;
  minX = std::_MAX(0,minX);
  minY = std::_MAX(0,minY);

  // Which tiles are hit ?

  xratio = mxgrid/m_xgrid;
  yratio = mygrid/m_ygrid;


  int nrmin = fineGrid.region(minX,minY);
  if(nrmin == -1 ) nrmin = 0;

  int ix = minX;
  int iy = minY;
  while ( ix <= maxX ) {
    int nrx = fineGrid.region(ix,iy);
    if( nrx == -1 ) nrx = 0;
    while ( iy <= maxY ) {
      // which region	
      int nr = fineGrid.region(ix,iy);
      if( nr == -1 ) nr = 0;
      int newx = ix/rfactor(nr)/xratio; 	  
      int newy = iy/rfactor(nr)/yratio;

      // cout << " New MuonTile " << rfactor(nr)*xratio << " " 
      //      << rfactor(nr)*yratio << " " << quarter << " " 
      //      << nr << " " << newx << " " << newy << endl;

      vmt.push_back(MuonTile(quarter,nr,newx,newy,*this));
      iy += rfactor(nr)*yratio;
    }    
    iy = minY;
    ix += rfactor(nrx)*xratio;
  }
  
  return vmt;
}

std::vector<MuonTile> MuonLayout::tilesInArea(const MuonTile& pad, 
					int areaX, int areaY) {
					
//  This function returns all the MuonTile's touched by the "pad"
//  defined with the "playout" MuonLayout +- areaX and +- areaY	


  MuonLayout playout = pad.layout();
  std::vector<MuonTile> vmt;

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
  int maxX = (pad.nX()+areaX+1)*playout.rfactor(nreg)*xratio-1;
  int maxY = (pad.nY()+areaY+1)*playout.rfactor(nreg)*yratio-1;
  int minX = maxX - playout.rfactor(nreg)*xratio*(2*areaX+1) + 1;
  int minY = maxY - playout.rfactor(nreg)*yratio*(2*areaY+1) + 1;
  minX = std::_MAX(0,minX);
  minY = std::_MAX(0,minY);

  // Which tiles are hit ?

  xratio = mxgrid/m_xgrid;
  yratio = mygrid/m_ygrid;


  int nrmin = fineGrid.region(minX,minY);
  if(nrmin == -1 ) nrmin = 0;

  int ix = minX;
  int iy = minY;
  while ( ix <= maxX ) {
    int nrx = fineGrid.region(ix,iy);
    if( nrx == -1 ) nrx = 0;
    while ( iy <= maxY ) {
      // which region	
      int nr = fineGrid.region(ix,iy);
      if( nr == -1 ) nr = 0;
      int newx = ix/rfactor(nr)/xratio; 	  
      int newy = iy/rfactor(nr)/yratio;

      // cout << " New MuonTile " << rfactor(nr)*xratio << " " 
      //      << rfactor(nr)*yratio << " " << quarter << " " 
      //      << nr << " " << newx << " " << newy << endl;

      vmt.push_back(MuonTile(quarter,nr,newx,newy,*this));
      iy += rfactor(nr)*yratio;
    }    
    iy = minY;
    ix += rfactor(nrx)*xratio;
  }
  for(int it=0; it<5; it++) {
    vmt.push_back(MuonTile(0,0,10,10,*this));
  }
  return vmt;
}


std::vector<MuonTile> MuonLayout::tiles() {
  
  std::vector<MuonTile> vmt;
  std::vector<MuonTile> vmtr;
  
  for(int iq = 0; iq <4; iq++) {
    for(int ir = 0; ir <4; ir++) {
      vmtr = tiles(iq,ir);
      vmt.insert(vmt.end(),vmtr.begin(),vmtr.end());
    }
  }
  
  return vmt;
  
}

std::vector<MuonTile> MuonLayout::tiles(int iq) {
  
  std::vector<MuonTile> vmt;
  std::vector<MuonTile> vmtr;
  
  for(int ir = 0; ir <4; ir++) {
    vmtr = tiles(iq,ir);
    vmt.insert(vmt.end(),vmtr.begin(),vmtr.end());
  }
  
  return vmt;
  
}

std::vector<MuonTile> MuonLayout::tiles(int iq, int ir) {
  
  std::vector<MuonTile> vmt;
  
  int ix; int iy;
  for(ix = 0; ix < 2*xGrid(); ix++) {
    for(iy = yGrid(); iy < 2*yGrid(); iy++) {
      vmt.push_back(MuonTile(iq,ir,ix,iy,*this));
    }
  }
  for(ix = xGrid(); ix < 2*xGrid(); ix++) {
    for(iy = 0; iy < yGrid(); iy++) {
      vmt.push_back(MuonTile(iq,ir,ix,iy,*this));
    }
  }
  
  return vmt;
  
}

std::vector<MuonTile> MuonLayout::tilesInRegion(const MuonTile& pad, 
                                                int pregion) {
    
  int nr = pad.region();
  int nq = pad.quarter();
  
  if(nr == pregion) {
    return tilesInArea(pad,0,0);    
  } else {
    std::vector<MuonTile> vmt = tilesInArea(pad,0,0); 
    for(int it=0; it<5; it++) {
      vmt.pop_back();
    }
    std::vector<MuonTile>::iterator ivmt;
    std::vector<MuonTile> nvmt;
    // Bring the pads in vmt to the pregion definition
    for (ivmt = vmt.begin(); ivmt != vmt.end(); ivmt++) {      
      if(nr<pregion) {
	int factor = rfactor(pregion)/rfactor(nr);
	int newX = ivmt->nX()/factor;
	int newY = ivmt->nY()/factor;
	MuonTile tile(nq,pregion,newX,newY,*this);
	nvmt.push_back(tile);
      } else {
        int factor = rfactor(nr)/rfactor(pregion);
	int minX = ivmt->nX()*factor;
	int minY = ivmt->nY()*factor;
        for(int ix=0; ix<factor; ix++) {
	  for(int iy=0; iy<factor; iy++) {
	    MuonTile tile(nq,pregion,minX+ix,minY+iy,*this);
	    nvmt.push_back(tile);
	  }
	}
      }
    }
    for(int it1=0; it1<5; it1++) {
      nvmt.push_back(MuonTile(0,0,10,10,*this));
    }
    return nvmt;
  }
}

bool MuonLayout::validTile(const MuonTile& mt) {

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

MuonTile MuonLayout::contains(const MuonTile& pad) {
  // It is responsibility of the user to assure that the pad
  // layout is finer than the containing layout
  std::vector<MuonTile> mtiles = tiles(pad);
  return mtiles[0];
} 

bool operator==(MuonLayout ml1, MuonLayout ml2) {
    return ml1.xGrid()==ml2.xGrid() &&
           ml1.yGrid()==ml2.yGrid();
}

MsgStream& operator<<(MsgStream& log, const MuonLayout& ml) {
   
    return log << " X grid: " << ml.xGrid()
               << " Y grid: " << ml.yGrid();
}
