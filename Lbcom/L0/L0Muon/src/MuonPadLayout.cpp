// $Header: 

#define MUONPADLAYOUT_CPP

// Include files
#include <strstream>
#include <iostream>

#include "GaudiKernel/MsgStream.h"
#include "DetDesc/DetectorElement.h"

#include "MuonPadLayout.h"
#include "L0Muon/L0mPad.h"

//------------------------------------------------------------------------------
//
/// Implementation of class :  MuonPadLayout
//
/// Author: A.Tsaregorodtsev
//
//------------------------------------------------------------------------------


MuonPadLayout::MuonPadLayout(IDataProviderSvc* detSvc) {

   // Get the layout data

/*   
    int station; 

    for(int i = 0; i<5; i++) {
	
	std::string path = "/dd/Structure/LHCb/MuonTrigger/Layout";
        station = i+1;
//	cout << " MuonPadLayout path: \n"  << path+station 
//	     << (path+station).size() << endl;
	
        SmartDataPtr<DetectorElement> st(detSvc,path+station);
	if(!st) {
	    cout << " Could not load path " << path+station << endl;
	} 
	else {
            m_layout.push_back(st);
	}    
    }
*/    
}

MuonPadLayout::MuonPadLayout(DoubleArrayProperty zstation,
        		     DoubleArrayProperty bcellX,
			     DoubleArrayProperty bcellY, 
			     IntegerArrayProperty* regions) {

// Crude constructor when the XML database is not available

    Parameter* st;   
    int i;
    
    std::vector<double> zst=zstation;
    std::vector<double> bcx=bcellX;
    std::vector<double> bcy=bcellY;
    
    for (i=0; i<5; i++) {
	st = new Parameter();
	st->addParameter(zst[i],"Zposition");
	st->addParameter(bcx[i],"BasicCellX");
	st->addParameter(bcy[i],"BasicCellY");
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

    Parameter* lst = m_layout[st-1];
//
//  Crude estimate of the station number

    
    
    double bcellX = lst->get("BasicCellX");
    double bcellY = lst->get("BasicCellY"); 
    
    double reglimX[] = { lst->get("Regions",0)*bcellX,
                         lst->get("Regions",2)*bcellX,
		         lst->get("Regions",4)*bcellX,
                         lst->get("Regions",6)*bcellX  };
    double reglimY[] = { lst->get("Regions",1)*bcellY,
                         lst->get("Regions",3)*bcellY,
		         lst->get("Regions",5)*bcellY,
                         lst->get("Regions",7)*bcellY  };	
		    	    
    // Find the region
    
    double ax = fabs(x);
    double ay = fabs(y);
    int nr = 0;
    double rfac = 0.;
    for (int i=0; i<4; i++) {
        if ( ax>reglimX[i] || ay>reglimY[i] ) {
	    nr++;
	    rfac = (rfac==0.) ? rfac=1. : rfac*2.;
	}
    }
    int nx =  (int) (ax/(bcellX*rfac) + 1);
    int ny =  (int) (ay/(bcellY*rfac) + 1);
    
/*    if (abs(nx)>10000) {
        cout << "Pad nx " << nx << endl;
	cout << "Pad x " << x << endl;
	cout << "Pad y " << y << endl;
	cout << "Pad bcellX " << bcellX << endl;
	cout << "Pad bcellY " << bcellY << endl;
	cout << "Pad nr " << nr << endl;
	cout << "Pad rfac " << rfac << endl;
     }
*/
    
    int iquarter = 1;
    if(y<0 && x>0) iquarter=2;
    if(y<0 && x<0) iquarter=3;
    if(y>0 && x<0) iquarter=4;
        
    L0mPad* lp = new L0mPad(st,iquarter,nr,nx,ny);
    lp->setX(x);
    lp->setY(y);
    lp->setZ(lst->get("Zposition"));
    	
    return lp;
}

L0mPad* MuonPadLayout::createPad(MuonDigit* md) {
    int st = md->getPadID()>>30;
   
    double x = md->getX();
    double y = md->getY();
    double z = md->getZ();
//
//  Crude estimate of the station number
//
    double zbound[] = { 1400., 1600., 1700., 1800. };
    st = 1;
    for (int i=0; i<4; i++) { 
        if ( z > zbound[i] ) st++;
    }    
       
//    cout << " Muon pad station " << st 
//         << " x: " << x 
//         << " y: " << y << endl;
  
    return createPad(st,x,y);
}

void MuonPadLayout::print(MsgStream log) {
   for(unsigned int i=0; i < m_layout.size(); i++) {
      log << MSG::DEBUG << "Station " << i 
           << " Z: "  << m_layout[i]->get("Zposition") 
	   << " cellX: "  << m_layout[i]->get("BasicCellX") 
	   << " cellY: "  << m_layout[i]->get("BasicCellY") 
	   << endreq;
   }
}

int MuonPadLayout::region(int nx, int ny, int nr) {

    Parameter* lst = m_layout[2];
    int nybase = (ny-1)*rfactor(nr)+1; 
    int nxbase = (nx-1)*rfactor(nr)+1; 
    int reglimX[] = { lst->getInt("Regions",0),
                      lst->getInt("Regions",2),
		      lst->getInt("Regions",4),
                      lst->getInt("Regions",6)  };
    int reglimY[] = { lst->getInt("Regions",1),
                      lst->getInt("Regions",3),
		      lst->getInt("Regions",5),
                      lst->getInt("Regions",7)  };
		      
    int newregion = 0;      
    for (int i=0; i<4; i++) {
        if ( nxbase>reglimX[i] || nybase>reglimY[i] ) {
	    nr++;
	}
    }
    return newregion;
}

int MuonPadLayout::rfactor(int nr) {

    int rfac = 1;    
    int i = 1;
    while ( i<nr ) {
        rfac *= 2;
        i++;
    }
    return rfac;
}

std::string operator+(const std::string& st, int ii) {
        ostrstream ost;
	ost << st  << ii << '\0';
        return ost.str();
}
