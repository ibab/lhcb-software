// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/src/Lib/MuonPadLayout.cpp,v 1.1 2001-06-07 16:35:57 atsareg Exp $

#define MUONPADLAYOUT_CPP

// Include files
#include <strstream>
#include <iostream>

#include "GaudiKernel/MsgStream.h"
#include "DetDesc/DetectorElement.h"

#include "L0Muon/MuonPadLayout.h"
#include "L0Muon/MuonLayout.h"
#include "L0Muon/L0mPad.h"

//------------------------------------------------------------------------------
//
/// Implementation of class :  MuonPadLayout
//
/// Author: A.Tsaregorodtsev
//
//------------------------------------------------------------------------------


MuonPadLayout::MuonPadLayout(IDataProviderSvc* detSvc, MsgStream& log) {

   // Get the layout data

/*   
    int station; 

    for(int i = 0; i<5; i++) {
	
	std::string path = "/dd/Structure/LHCb/MuonTrigger/Layout";
        station = i+1;
	
        SmartDataPtr<DetectorElement> st(detSvc,path+station);
	if(!st) {
	    log << MSG::ERROR << " MuonPadLayout: Could not load path " 
	                      << path+station << endreq;
	} 
	else {
            m_layout.push_back(st);
	}    
    }
*/    
}

MuonPadLayout::MuonPadLayout(std::vector<double> zstation,
        		     std::vector<double> bcellX,
			     std::vector<double> bcellY, 
			     std::vector<int>* regions) {

// Crude constructor when the XML database is not available

    Parameter* st;   
    int i;
    
    for (i=0; i<5; i++) {
	st = new Parameter();
	st->addParameter(zstation[i],"Zposition");
	st->addParameter(bcellX[i],"BasicCellX");
	st->addParameter(bcellY[i],"BasicCellY");
	st->addParameter(4,"nRegion");
	st->addParameterVector(regions[i],"Regions");
	m_layout.push_back(st);
    }	
}

MuonPadLayout::~MuonPadLayout() {
    Parameter* st;   
    int i;
    for (i=0; i<5; i++) {
	st = m_layout[i];
	delete st;
    }
}

L0mPad* MuonPadLayout::createPad(int st, double x, double y) {

  Parameter* lst = m_layout[st];

  double bcellX = lst->get("BasicCellX");
  double bcellY = lst->get("BasicCellY"); 

  int regX = lst->getInt("Regions",0);
  int regY = lst->getInt("Regions",1);
  double reglimX = regX*bcellX;
  double reglimY = regY*bcellY;	

  // Find the region

  double ax = fabs(x);
  double ay = fabs(y);
  int nr = 0;
  for (int i=1; i<=4; i++) {
    if ( ax>reglimX*rfactor(i) || ay>reglimY*rfactor(i) ) {
      nr++;
    }
  }
  int nx =  (int) (ax/(bcellX*rfactor(nr)) + 1);
  int ny =  (int) (ay/(bcellY*rfactor(nr)) + 1);

  int iquarter = 1;
  if(y<0 && x>0) iquarter=2;
  if(y<0 && x<0) iquarter=3;
  if(y>0 && x<0) iquarter=4;

  MuonLayout ml(regX,regY);	
  L0mPad* lp = new L0mPad(st,iquarter,nr,nx,ny,ml);
  lp->setX(x);
  lp->setDx(bcellX*rfactor(nr));
  lp->setY(y);
  lp->setDy(bcellY*rfactor(nr));
  lp->setZ(lst->get("Zposition"));

  return lp;
}

L0mPad* MuonPadLayout::createPad(MuonDigit* md) {
   
    double x = md->getX();
    double y = md->getY();
    double z = md->getZ();
//
//  Crude estimate of the station number
//
    double zbound[] = { 1400., 1600., 1700., 1800. };
    int st = 0;
    for (int i=0; i<4; i++) { 
        if ( z > zbound[i] ) st++;
    }    
         
    return createPad(st,x,y);
}

//int MuonPadLayout::region(int nx, int ny, int nr) {
//
//    Parameter* lst = m_layout[2];
//    int nybase = (ny-1)*rfactor(nr)+1; 
//    int nxbase = (nx-1)*rfactor(nr)+1; 
//    
//    int reglimX = lst->getInt("Regions",0);
//    int reglimY = lst->getInt("Regions",1);
//		      
//    int newregion = 0;      
//    for (int i=1; i<=4; i++) {
//        if ( nxbase>reglimX*rfactor(i) || nybase>reglimY*rfactor(i) ) {
//	    newregion++;
//	}
//    }
//    return newregion;
//}

int MuonPadLayout::rfactor(int nr) {

    int rfac = 1 << (nr-1);
    return rfac ;
}

std::string operator+(const std::string& st, int ii) {
        ostrstream ost;
	ost << st  << ii << '\0';
        return ost.str();
}

MsgStream& operator<<(MsgStream& log, const MuonPadLayout& mpl) {
    for(unsigned int i=1; i <= 5; i++) {
	log << "Station " << i 
            << " Z: "  << mpl.layout(i)->get("Zposition") 
	    << " cellX: "  << mpl.layout(i)->get("BasicCellX") 
	    << " cellY: "  << mpl.layout(i)->get("BasicCellY") 
	    << endreq;
    }
    return log;
}
