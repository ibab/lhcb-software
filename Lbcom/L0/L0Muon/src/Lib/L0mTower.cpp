// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/src/Lib/L0mTower.cpp,v 1.1 2001-06-07 16:35:55 atsareg Exp $
#include "GaudiKernel/MsgStream.h"

#include "L0Muon/L0mTower.h"
#include "L0Muon/L0mPad.h"
#include "L0Muon/L0MuonCandidate.h"

const CLID& CLID_L0mTower = 6005;  // User defined  

L0mTower::L0mTower()  {
  m_pad3 = 0;
  for (int i = 0; i<5; i++ ) {
    m_indices.push_back(HitIndex(999,999));
  }  
  m_indices[2] = HitIndex(0,0);
  // prepare the map of indices
  m_bitmap = std::vector<StationMap>(5);
  m_pt = 0.;
  m_theta = 0.;
  m_phi = 0.;
  m_found = false;
}

L0mTower::L0mTower(L0mPad* pad)  {
  m_pad3 = pad;
  for (int i = 0; i<5; i++ ) {
    m_indices.push_back(HitIndex(999,999));
  }
  m_indices[2] = HitIndex(0,0);
  // prepare the map of indices
  m_bitmap = std::vector<StationMap>(5);
  // in the 3-d station there is only a seed pad
  addBit(0,0,2,pad);
  m_pt = 0.;
  m_theta = 0.;
  m_phi = 0.;
  m_found = false;
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
      <<  m_pad3->nY() <<  endreq << endreq;

  drawStation(log,1,8,0);
  std::string blanc(39+m_extra1*2,' ');
  log << blanc << "|=|" << "   " << m_extra1 << endreq;
  drawStation(log,0,16,0);
   
}	   
		   
void L0mTower::addBit(int ix, int iy, int st, L0mPad* srp ) {   
 
  std::pair<int,int> bit(ix,iy);
 
  m_bitmap[st][bit] = srp;
  
}

L0mTower::HitIndex L0mTower::searchStation(bool& found, 
                                           int st, 
					   int foiX, 
					   int cindex,
					   int yindex ) {
					   
  StationMap stmap = m_bitmap[st];
  
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
  found = false;
  return HitIndex(999,999);
}

L0mPad* L0mTower::findTrack(std::vector<int> foiX, std::vector<int> foiY) {

  L0mPad* lpd = 0;
  bool foundHit;

  // confirm the candidate in M4 & M5

  HitIndex hd = searchStation(foundHit,4,foiX[4]);
  if(!foundHit) return 0;
  m_indices[3] = hd;
  hd = searchStation(foundHit,3,foiX[3]);
  if(!foundHit) return 0;
  m_indices[5] = hd;
    
  // find the nearest hit in M2
  
  m_indices[1] = searchStation(foundHit,1,foiX[1]);
  if(!foundHit) return 0;
      
  // extrapolate and find a hit in M1
  
  // round to the nearest integer
  int indext = (int)(abs(m_indices[1].first)*3.58+0.5);  
  indext = (m_indices[1].first<0) ? -indext : indext;
  m_extra1 = indext;
  m_indices[0] = searchStation(foundHit,0,foiX[0],indext);
  if(!foundHit) return 0;
  m_found = true;     
  return m_bitmap[0].find(m_indices[0])->second;
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

L0MuonCandidate* L0mTower::createCandidate(const std::vector<double>& ptpara,
                        		   const std::vector<int>& foiX, 
					   const std::vector<int>& foiY) {
  
  if ( !trackFound() ) {
    findTrack(foiX,foiY);
  }  
  if(trackFound()) {
    ptcalc(ptpara);
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

double L0mTower::ptcalc(const std::vector<double>& ptpara) {
  double d1 = ptpara[0] ;
  double d2 = ptpara[1] ;
  double d3 = ptpara[2] ;
  double alpha = ptpara[3] ;
    
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
  
    HitIndex iplus(xoff+inx,0);
    if( stmap.find(iplus) != stmap.end() ) {
      return iplus.first;
    }
    
    HitIndex iminus(xoff-inx,0);
    if( stmap.find(iminus) != stmap.end() ) {
      return iminus.first;
    }
  }
  return 999;
}

/// accessor to the resulting pad in M2
L0mPad* L0mTower::pad(int station) { 
  return m_bitmap[station].find(m_indices[station])->second; 
}
