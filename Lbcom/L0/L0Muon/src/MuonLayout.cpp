#define MUONLAYOUT_CPP

// Include files
#include <iostream>
#include "MuonLayout.h"
#include "L0Muon/MuonTile.h"

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

    int rfac = 1;
    int i = 1;
    while ( i<nr ) {
        rfac *= 2;
        i++;
    }
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
    return max(nrx,nry);
    
}

std::vector<MuonTile> MuonLayout::tiles(const MuonTile& pad, 
                                        const MuonLayout& playout,
					int areaX = 0,
					int areaY = 0) {

    std::vector<MuonTile> vmt;
    
    int quarter = pad.quarter();
    int nreg = pad.region();
    
  //  cout << "Pad quarter/region/nx/ny " << quarter << "/"
  //       << nreg << "/" << pad.nX()<< "/"  << pad.nY() << endl
  //	 << "Target grid : " << m_xgrid << "-" << m_ygrid << endl;
    
    // the finest grid of the two layouts
    int mxgrid = max(m_xgrid, playout.xGrid() );
    int mygrid = max(m_ygrid, playout.yGrid() );
    
//    cout << "Fine grid: " << mxgrid << " " << mygrid << endl;
    
    MuonLayout fineGrid(mxgrid,mygrid);
    
    int xratio = m_xgrid/playout.xGrid();
    int yratio = m_ygrid/playout.yGrid();
    
    xratio = (xratio == 0) ? 1 : xratio ;
    yratio = (yratio == 0) ? 1 : yratio ;
    
//    cout << "Ratios " << xratio << " " << yratio << endl;
    
    // input pad area in terms of the finest grid
    int maxX = (pad.nX()+areaX)*playout.rfactor(nreg)*xratio;
    int maxY = (pad.nY()+areaY)*playout.rfactor(nreg)*yratio;
    int minX = maxX - playout.rfactor(nreg)*xratio*(2*areaX+1) + 1;
    int minY = maxY - playout.rfactor(nreg)*yratio*(2*areaY+1) + 1;
    minX = max(1,minX);
    minY = max(1,minY);
    
  //  cout << "Pad area " << minX << "-" << minY << " "
  //                      << maxX << "-" << maxY << endl; 
    
    // Which tiles are hit ?
    
    xratio = mxgrid/m_xgrid;
    yratio = mygrid/m_ygrid;
    
  //  cout << "Ratios bis " << xratio << " " << yratio << endl;
    
    int nrmin = fineGrid.region(minX,minY);
    
  /*  cout << "Nrmin " << nrmin << " " <<  rfactor(nrmin) << endl;
    
    if(nrmin != nreg ) {
       cout << "!!!!!!!!!! Change of the region !!!!!!!!!!!!!!" << endl;
    }
  */  
    
    for (int ix = minX; ix <= maxX; ix+=rfactor(nrmin) ) {
        for (int iy = minY; iy <= maxY; iy+=rfactor(nrmin) ) {
            // which region	
	    int nr = fineGrid.region(ix,iy);
	    int newx = (ix-1)/rfactor(nr)/xratio+1;
	    int newy = (iy-1)/rfactor(nr)/yratio+1;
	    vmt.push_back(MuonTile(quarter,nr,newx,newy));	
	   			   
	//     cout << "vmt size " << vmt.size() << endl;	
	//    cout << "Adding tile :" << endl;
	//    MuonTile(quarter,nr,newx,newy).print();	   
	}
    }
    return vmt;
}
