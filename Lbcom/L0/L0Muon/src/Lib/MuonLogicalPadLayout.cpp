// $Id: MuonLogicalPadLayout.cpp,v 1.1 2001-07-09 19:02:16 atsareg Exp $

// Include files
#include <strstream>
#include <iostream>

#include "GaudiKernel/MsgStream.h"

#include "L0Muon/MuonLogicalPadLayout.h"
#include "L0Muon/MuonLayout.h"
#include "L0Muon/L0mPad.h"

//------------------------------------------------------------------------------
//
/// Implementation of class :  MuonLogicalPadLayout
//
/// Author: A.Tsaregorodtsev
//
//------------------------------------------------------------------------------


MuonLogicalPadLayout::MuonLogicalPadLayout(IDataProviderSvc* detSvc, MsgStream& log) {
}

MuonLogicalPadLayout::MuonLogicalPadLayout(std::vector<double> zstation,
                                           std::vector<int>* regions) {

// Crude constructor when the XML database is not available

  Parameter* st;   
  int i;

  for (i=0; i<5; i++) {
      st = new Parameter();
      st->addParameter(zstation[i],"Zposition");
      st->addParameterVector(regions[i],"Regions");
      m_layout.push_back(st);
  }	
  
  for (int st=0; st<5; st++) {
    for (int nr=0; nr<4; nr++) {
      switch (st) {
	//:::::::::::::::::::::
	//  Station 1
	//:::::::::::::::::::::
	case 0: {
	  switch (nr) {
            case 0: {
	      m_lunitX[st][nr] = 1;
	      m_npadX[st][nr] = 24;
	      m_lunitY[st][nr] = 1;
	      m_npadY[st][nr] = 8;
	      break;
	    }
	    case 1: {
	      m_lunitX[st][nr] = 1;
	      m_npadX[st][nr] = 24;
	      m_lunitY[st][nr] = 2;
	      m_npadY[st][nr] = 4;
	      break;
	    }
	    case 2: {
	      m_lunitX[st][nr] = 1;
	      m_npadX[st][nr] = 24;
	      m_lunitY[st][nr] = 4;
	      m_npadY[st][nr] = 2;
	      break;
	    }
	    case 3: {
	      m_lunitX[st][nr] = 2;
	      m_npadX[st][nr] = 12;
	      m_lunitY[st][nr] = 8;
	      m_npadY[st][nr] = 1;
	      break;
	    }
	  }
	}  
	break;
	//:::::::::::::::::::::
	//  Station 2 and 3
	//:::::::::::::::::::::
	case 1: case 2: {  
	  switch (nr) {
            case 0: {
	      m_lunitX[st][nr] = 8;
	      m_npadX[st][nr] = 6;
	      m_lunitY[st][nr] = 1;
	      m_npadY[st][nr] = 8;
	      break;
	    }
	    case 1: {
	      m_lunitX[st][nr] = 4;
	      m_npadX[st][nr] = 12;
	      m_lunitY[st][nr] = 2;
	      m_npadY[st][nr] = 4;
	      break;
	    }
	    case 2: {
	      m_lunitX[st][nr] = 2;
	      m_npadX[st][nr] = 24;
	      m_lunitY[st][nr] = 2;
	      m_npadY[st][nr] = 4;
	      break;
	    }
	    case 3: {
	      m_lunitX[st][nr] = 2;
	      m_npadX[st][nr] = 24;
	      m_lunitY[st][nr] = 2;
	      m_npadY[st][nr] = 4;
	      break;
	    }
	  }
	}
	break;
	//:::::::::::::::::::::
	//  Station 4 and 5
	//:::::::::::::::::::::
	case 3: case 4: {  
	  switch (nr) {
            case 0: {
	      m_lunitX[st][nr] = 1;
	      m_npadX[st][nr] = 12;
	      m_lunitY[st][nr] = 1;
	      m_npadY[st][nr] = 8;
	      break;
	    }
	    case 1: {
	      m_lunitX[st][nr] = 4;
	      m_npadX[st][nr] = 3;
	      m_lunitY[st][nr] = 2;
	      m_npadY[st][nr] = 4;
	      break;
	    }
	    case 2: {
	      m_lunitX[st][nr] = 2;
	      m_npadX[st][nr] = 6;
	      m_lunitY[st][nr] = 2;
	      m_npadY[st][nr] = 4;
	      break;
	    }
	    case 3: {
	      m_lunitX[st][nr] = 2;
	      m_npadX[st][nr] = 6;
	      m_lunitY[st][nr] = 2;
	      m_npadY[st][nr] = 4;
	      break;
	    }
	  }
	}  
      }
    }   
  }  
}

MuonLogicalPadLayout::~MuonLogicalPadLayout() {
  Parameter* st;   
  int i;
  for (i=0; i<5; i++) {
      st = m_layout[i];
      delete st;
  }
}


L0mPad* MuonLogicalPadLayout::createPad(MuonDigit* md) {
      
  double x = md->getX();
  double y = md->getY();
  double z = md->getZ();
  unsigned int id = md->getPadID();

  ID tt = getId(id);
  
  Parameter* lst = m_layout[tt.station];
  int regX = lst->getInt("Regions",0);
  int regY = lst->getInt("Regions",1);
  
  double zStation = lst->get("Zposition");
  x = x*zStation/z;
  y = y*zStation/z;        
  
  MuonLayout ml(regX,regY);
  L0mPad* lp = new L0mPad(tt.station,tt.quarter,tt.region,tt.nx,tt.ny,ml);
  lp->setX(x);
  lp->setY(y);
  lp->setZ(z);

  return lp; 
}

MuonLogicalPadLayout::ID MuonLogicalPadLayout::getId(unsigned int id) {

  int st =       id                    >> (32-3);
  int region =   (id << (33 - 28 - 2)) >> (32-2);
  int segment =  (id << (33 - 24 - 4)) >> (32-4);
  int luc =      (id << (33 - 21 - 3)) >> (32-3);
  int lur =      (id << (33 - 18 - 3)) >> (32-3);
  int lup =      (id << (33 - 16 - 2)) >> (32-2);
  int map =      (id << (33 - 15 - 1)) >> (32-1);
  int nx  =      (id << (33 - 8 - 7)) >> (32-7);
  int ny  =      (id << (33 - 3 - 5)) >> (32-5);
  int type  =    (id << (33 - 1 - 2)) >> (32-2);
  
  ID tt;
  // station
  tt.station = st;
  // nq - quarter number
  tt.quarter = segment/3;
  int nq = tt.quarter;
  // nr - region number
  tt.region = region;
  int nr = tt.region;
  // ns - symmetrical segment number for given nq and nr
  int ns = segment%3;
  if(nq==1 || nq==3) ns = 2-ns; 
  
  int nLUX = m_lunitX[st][nr];
  int nLUY = m_lunitY[st][nr];
  int nPX  = m_npadX[st][nr];
  int nPY  = m_npadY[st][nr];
   
   
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  // find the "MuonLayout" style LU number and pad number within LU
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  
  if(tt.quarter==1 || tt.quarter==2) {
    ny = nPY-1-ny;
    lur = nLUY-1-lur;
  }  
  if(tt.quarter==3 || tt.quarter==2) {
    nx = nPX-1-nx;
    luc = nLUX-1-luc;
  }
  
  int nluy = lur;
  if(ns==0 || ns==1) nluy += nLUY;
  
  int nlux = luc;
  if(ns==2 || ns==1) nlux += nLUX;  
  
  tt.ny = nluy*nPY + ny;
  tt.nx = nlux*nPX + nx;
    
//   cout << " ns=" << ns << " nluy=" << nluy << " nlux=" << nlux 
//        << " nPX=" << nPX << " nPY=" << nPY 
//        << " nx=" << nx << " ny=" << ny << endl;
  
  return tt;
  
}
