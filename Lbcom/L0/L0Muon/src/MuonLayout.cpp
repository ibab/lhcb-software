// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/src/MuonLayout.cpp,v 1.4 2001-05-04 10:22:28 cattaneb Exp $
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
#endif

//-----------------------------------------------------------------------------
//
/// Implementation of class :  MuonLayout
//
/// Author: A.Tsaregorodtsev
//
//-----------------------------------------------------------------------------

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

  int rfac = 1 << (nr-1);
  return rfac;
}  

int MuonLayout::region(int ix, int iy) const {

  int nrx;
  if ( (ix-1)/m_xgrid < 1 ) {
    nrx = 0;
  } else if ( (ix-1)/m_xgrid == 1 ) {
    nrx = 1;
  } else if ( (ix-1)/m_xgrid <= 3 ) {
    nrx = 2;  
  } else if ( (ix-1)/m_xgrid <= 7 ) {
    nrx = 3; 	
  } else {
    nrx = 4; 	
  }
  int nry;
  if ( (iy-1)/m_ygrid < 1 ) {
    nry = 0;
  } else if ( (iy-1)/m_ygrid == 1 ) {
    nry = 1;
  } else if ( (iy-1)/m_ygrid <= 3 ) {
    nry = 2;  
  } else if ( (iy-1)/m_ygrid <= 7 ) {
    nry = 3; 	
  } else {
    nry = 4; 	
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
  int maxX = (pad.nX()+areaX)*playout.rfactor(nreg)*xratio;
  int maxY = (pad.nY()+areaY)*playout.rfactor(nreg)*yratio;
  int minX = maxX - playout.rfactor(nreg)*xratio*(2*areaX+1) + 1;
  int minY = maxY - playout.rfactor(nreg)*yratio*(2*areaY+1) + 1;
  minX = std::_MAX(1,minX);
  minY = std::_MAX(1,minY);

  // Which tiles are hit ?

  xratio = mxgrid/m_xgrid;
  yratio = mygrid/m_ygrid;


  int nrmin = fineGrid.region(minX,minY);
  if(nrmin == 0 ) nrmin = 1;

  int ix = minX;
  int iy = minY;
  while ( ix <= maxX ) {
    int nrx = fineGrid.region(ix,iy);
    if( nrx == 0 ) nrx = 1;
    while ( iy <= maxY ) {
      // which region	
      int nr = fineGrid.region(ix,iy);
      if( nr == 0 ) nr = 1;
      int newx = (ix-1)/rfactor(nr)/xratio+1;	    
      int newy = (iy-1)/rfactor(nr)/yratio+1;

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

MuonTile MuonLayout::contains(const MuonTile& pad) {
  // It is responsibility of the user to assure that the pad
  // layout is finer than the containing layout
  std::vector<MuonTile> mtiles = tiles(pad);
  return mtiles[0];
} 

MsgStream& operator<<(MsgStream& log, const MuonLayout& ml) {
   
    return log << " X grid: " << ml.xGrid()
               << " Y grid: " << ml.yGrid();
}
