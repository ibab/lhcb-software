// $Id: L0mTower.cpp,v 1.8 2001-07-26 13:05:38 cattanem Exp $
#include "GaudiKernel/MsgStream.h"

#include <set>
#include <vector>
#include <algorithm>

#include "L0Muon/L0mTower.h"
#include "L0Muon/L0mPad.h"
#include "L0Muon/L0MuonCandidate.h"
#include "L0Muon/L0mProcUnit.h"
  

L0mTower::L0mTower()  {
  m_pad3 = 0;
  std::pair<int,int> hi(999,999);
  for (int i = 0; i<5; i++ ) {
    m_indices[i] = hi;  
  }  
  m_indices[2] = HitIndex(0,0);
  // prepare the map of indices
  // m_bitmap = std::vector<StationMap>(5);
  m_pt = 0.;
  m_theta = 0.;
  m_phi = 0.;
  m_found = false;
  m_limited = false;
  m_extra1 = 0;
}

L0mTower::L0mTower(L0mPad* pad)  {
  m_pad3 = pad;
  std::pair<int,int> hi(999,999);
  for (int i = 0; i<5; i++ ) {
    m_indices[i] = hi;
  }
  m_indices[2] = HitIndex(0,0);
  // prepare the map of indices
  //m_bitmap = std::vector<StationMap>(5);
  // in the 3-d station there is only a seed pad
  addBit(0,0,2,pad);
  m_pt = 0.;
  m_theta = 0.;
  m_phi = 0.;
  m_found = false;
  m_limited = false;
  m_extra1 = 0;
}

L0mTower::~L0mTower() {
}	

void L0mTower::drawStation(MsgStream& log, int st, int xrange, int yrange) {

  StationMap stmap = m_bitmap[st];
  
  int iy;
  for( iy = yrange; iy >= -yrange; iy--) {
    log << " St." << st+1;  
    std::string blanc((17-xrange)*2,' ');
    log << blanc << "|" ;
    for(int i=-xrange; i<(xrange+1); i++) {
      HitIndex ip(i,iy);
      if(stmap.find(ip) != stmap.end()) {
	log << "#|";
      } else {
	log << "0|";
      }	   
    }
    log << endreq;  
  }  
}

void L0mTower::draw(MsgStream& log) {

  log << "-----" << endreq;
  drawStation(log,4,10,1);
  log << endreq;
  drawStation(log,3,8,1);
  log << endreq;
  
  log << " St.3                                  |#|     "  
      <<  m_pad3->quarter() << "/" 
      <<  m_pad3->region() << "/" 
      <<  m_pad3->nX() << "/" 
      <<  m_pad3->nY() 
      << "  x=" << m_pad3->x() 
      << "  y=" << m_pad3->y() 
      << endreq << endreq;

  drawStation(log,1,8,0);
  if(m_extra1 > -19) {
    std::string blanc(39+m_extra1*2,' ');
    log << blanc << "|=|" << "   " << m_extra1 << endreq;
  } else {
    log << endreq;
  }  
  drawStation(log,0,16,0);
  if (m_limited) log << "Limited Y was applied" << endreq; 
}	   
		   
void L0mTower::addBit(int ix, int iy, int st, L0mPad* srp ) {   
 
  std::pair<int,int> bit(ix,iy);
 
//  cout << "inside addBit " << st << endl;
//  cout << m_bitmap[st].size()<< " " << srp << endl;
 
  m_bitmap[st][bit] = srp;
  
//  cout << m_bitmap[st].size() << endl;
  
}

L0mTower::HitIndex L0mTower::searchStation(bool& found, 
                                           int st, 
					   int foiX, 
					   int cindex,
					   int foiY ) {
					   
  StationMap stmap = m_bitmap[st];
  
  for (int iny=0; iny <= foiY; iny++ ) {
    int yindex = iny;
    for (int inx=0; inx <= foiX; inx++ ) {
      HitIndex iplus(cindex+inx,yindex);
      if( stmap.find(iplus) != stmap.end() ) {
	found = true;
	return iplus;
      }

      HitIndex iminus(cindex-inx,yindex);
      if( stmap.find(iminus) != stmap.end() ) {
	found = true;
	return iminus;
      }
    }
    yindex = -iny;
    if (yindex != 0) {
      for (int inx=0; inx <= foiX; inx++ ) {
	HitIndex iplus(cindex+inx,yindex);
	if( stmap.find(iplus) != stmap.end() ) {
	  found = true;
	  return iplus;
	}

	HitIndex iminus(cindex-inx,yindex);
	if( stmap.find(iminus) != stmap.end() ) {
	  found = true;
	  return iminus;
	}
      }
    }
  }      
  found = false;
  return HitIndex(999,999);
}

L0mPad* L0mTower::findTrack() {

  L0mPad* lpd = 0;
  bool foundHit;
  bool searchFailed = false;

  // confirm the candidate in M4 & M5
  
  HitIndex hd = searchStation(foundHit,4,
                              m_procUnit->m_foiX[4],
			      0,
			      m_procUnit->m_foiY[4]);
			      
  if(!foundHit) {
    searchFailed = true;
  } else {  
    m_indices[4] = hd;
  }  
  hd = searchStation(foundHit,3,
                     m_procUnit->m_foiX[3],
		     0,
		     m_procUnit->m_foiY[3]);
		     		     
  if(!foundHit) {
    searchFailed = true;
  } else {  
    m_indices[3] = hd;
  }
    
  // find the nearest hit in M2
  
  m_indices[1] = searchStation(foundHit,1,m_procUnit->m_foiX[1]);
    
  if(!foundHit) return 0;
      
  // extrapolate and find a hit in M1
  
  int indext;
  int ind2 = abs(m_indices[1].first);
  if(ind2<8) {
    indext = m_procUnit->m_extraM1[ind2]; 
  }  
  else {
    indext = 25;
  }  
  indext = (m_indices[1].first<0) ? -indext : indext;
  m_extra1 = indext;
  
  m_indices[0] = searchStation(foundHit,0,m_procUnit->m_foiX[0],indext);
  if(!foundHit) return 0;
  
  if(searchFailed) {
    return 0; 
  } else {
    m_found = true;     
    return m_bitmap[0].find(m_indices[0])->second;
  }  
}

bool L0mTower::isFull() {

    return (m_bitmap[0].size() != 0) &&
           (m_bitmap[1].size() != 0) &&
	   (m_bitmap[3].size() != 0) &&
           (m_bitmap[4].size() != 0);

}

bool L0mTower::trackFound() {
// indicates that the track finding was succesful
    return m_found;
}

L0MuonCandidate* L0mTower::createCandidate() {
  
//  m_procUnit->printParameters();
  
  if ( !trackFound() ) {
    findTrack();
  }    
  if(trackFound()) {
    ptcalc();
    SmartRefVector<L0mPad> srmps;
    srmps.push_back(pad(0));
    srmps.push_back(pad(1));
    srmps.push_back(pad(2));
    srmps.push_back(pad(3));
    srmps.push_back(pad(4));

    L0MuonCandidate* lmc = new L0MuonCandidate(pt(),
                        		       pad(0)->x(),
					       pad(0)->y(),
					       theta(),
					       phi(),
					       srmps,
					       L0Muon::OK) ;
    return lmc;			 
  } else {
    return 0;
  }  					   
}					   

double L0mTower::ptcalc() {
  double d1 = m_procUnit->m_ptParameters[0] ;
  double d2 = m_procUnit->m_ptParameters[1] ;
  double d3 = m_procUnit->m_ptParameters[2] ;
  double alpha = m_procUnit->m_ptParameters[3] ;
    
  double x1 = pad(0)->x();
  double y1 = pad(0)->y();
  double x2 = pad(1)->x();
  double x0 = x1 - d2*(x2-x1)/d3;
  double y0 = y1*d1/(d1+d2);
        
  double sq = (d1+d2)*x2-(d1+d2+d3)*x1;
  if(sq == 0.) {
      sq = 0.0001;
  } else {
      sq = 1./((d1+d2)*x2-(d1+d2+d3)*x1);
  }

  double sr = sqrt(((d2+d3)*x1 - d2*x2)*((d2+d3)*x1 - d2*x2) +
                   (d1*d3*y1/(d1+d2))*(d1*d3*y1/(d1+d2))   );
  double sc = sqrt((d1+d2)*(d1+d2)+y1*y1)/(d1+d2);
  double st = (d1*d3*d3+2.*(x1*(d3+d2)-x2*d2)*(x2-x1))/(d1*d3*d3);
  double ptm = fabs(alpha*sq*sr*sc*st);		     

  double xslope = (x2 - x1)/d3;
  double xin = x2 - xslope*(d1+d2+d3);
  if(xin < 0.) ptm = -ptm;

  // Pt should be in MeV
  m_pt = ptm*1000.;
  HepVector3D v(x0,y0,d1);
  m_theta = v.theta();
  m_phi = v.phi();
  
  return m_pt;
}

int L0mTower::nearest(int sta, int foiX, int yindex) {

  if( sta == 2 ) return 0;
  
  StationMap stmap = m_bitmap[sta];
  int xoff = 0;
  if( sta == 0 ) xoff = m_extra1;
  
  for (int inx=0; inx <= foiX; inx++ ) {
  
    HitIndex iplus(xoff+inx,yindex);
    if( stmap.find(iplus) != stmap.end() ) {
      return iplus.first;
    }
    
    HitIndex iminus(xoff-inx,yindex);
    if( stmap.find(iminus) != stmap.end() ) {
      return iminus.first;
    }
  }
  return 999;
}
//==============================================================
void L0mTower::limitedY() {
  
  MuonLayout pu_layout = MuonLayout(2,2);
  MuonLayout m3_layout = padM3()->layout();
  MuonTile mainPu = pu_layout.contains(*padM3());
  std::vector<MuonTile> tmpTile;  
  
  typedef StationMap::const_iterator SMI;
  typedef std::vector<MuonTile>::const_iterator CMTI;
  
  
  int puRegion = mainPu.region();
  // int puX = mainPu.nX();
  int puY = mainPu.nY();
  int nx = padM3()->nX();
  int ny = padM3()->nY();
    
  // Look if there are pads in stations 4 and 5 from other PU
  
  for(int ist = 3; ist < 5; ist++) {
    StationMap stmap = m_bitmap[ist];
    if(!m_bitmap[ist].empty()) {
      for(SMI im = stmap.begin(); im != stmap.end(); im++) {
	MuonTile pu = pu_layout.contains(*(im->second));
	if ( !(pu == mainPu) ) {
          // Check if the found PU is worth adding to the list
	  bool worth = false;
	  if( pu.region() == puRegion && pu.nY() != puY) worth = true;
	  if( pu.region() > puRegion ) {
	    if( pu.nY()==2 || 
	       (pu.nY()==1 && puY==1) ||
	       (pu.nY()==0 && puY==2) ) worth = true; 
	  }
	  if( pu.region() < puRegion ) {
	    if( puY == 2 ||
	        (pu.nY()==1 && puY==1) ||
	        (pu.nY()==2 && puY==0) ) worth = true; 
	  }
          // add the found pu to the list if it is not yet there
	  if (worth) {
	    if(tmpTile.empty()) {
	      tmpTile.push_back(pu);
	    } else {
	      CMTI itf = std::find(tmpTile.begin(),tmpTile.end(),pu);
	      if(itf == tmpTile.end()) {
		tmpTile.push_back(pu);
	      }
	    }  
	  }  
	}
      }
    }  

    // Add extra bits to the tower from the PU gross structure

    if ( !tmpTile.empty() ) {
      std::vector<MuonTile>::const_iterator it;
      for(it = tmpTile.begin(); it != tmpTile.end(); it++) {
	// M3 pads covered by the PU
	std::vector<MuonTile> m3Tile = m3_layout.tiles(*it);
	for(CMTI ip = m3Tile.begin(); ip != m3Tile.end(); ip++) {
          int iny = ip->nY() - ny;
	  if(abs(iny)<2) {
	    int inx = ip->nX();
	    addBit(inx-nx, iny, ist, padM3());	
	  }
	}
      }
      m_limited = true;
    }
    // This station is done, clean up
    tmpTile.clear();  
  }
}
//==============================================================
/// accessor to the resulting pad in M2
L0mPad* L0mTower::pad(int station) { 
  StationMap::iterator ism = m_bitmap[station].find(m_indices[station]);
  if(ism != m_bitmap[station].end()) {
    return m_bitmap[station].find(m_indices[station])->second; 
  } else {
    return 0;
  }  
}
