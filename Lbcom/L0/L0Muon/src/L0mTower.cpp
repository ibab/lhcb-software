// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/src/L0mTower.cpp,v 1.3 2001-05-04 10:22:27 cattaneb Exp $
#include "GaudiKernel/MsgStream.h"

#include "L0mTower.h"
#include "L0Muon/L0mPad.h"
#include "L0Muon/L0MuonCandidate.h"

L0mTower::L0mTower()  {
  m_pad1 = 0;
  m_pad2 = 0;
  m_pad3 = 0;
  m_ind1 = EMPTY_INDEX;
  m_ind2 = EMPTY_INDEX;
  m_bitmap = std::vector<StationMap>(5);
}

L0mTower::L0mTower(L0mPad* pad)  {
  m_pad1 = 0;
  m_pad2 = 0;
  m_pad3 = pad;
  m_ind1 = EMPTY_INDEX;
  m_ind2 = EMPTY_INDEX;
  m_bitmap = std::vector<StationMap>(5);
}

L0mTower::~L0mTower() {
}	

void L0mTower::drawStation(MsgStream& log, int st, int range) {

  StationMap stmap = m_bitmap[st];
  log << " St." << st+1;  
  std::string blanc((17-range)*2,' ');
  log << blanc << "|" ;
  for(int i=-range; i<(range+1); i++) {
    HitIndex ip(i,0);
    if(stmap.find(ip) != stmap.end()) {
      log << "#|";
    } else {
      log << "0|";
    }	   
  }
  log << endreq;
}

void L0mTower::draw(MsgStream& log) {

  log << "-----" << endreq;
  drawStation(log,4,10);
  drawStation(log,3,8);

  log << " St.3                                  |#|     "  
      <<  m_pad3->quarter() << "/" 
      <<  m_pad3->region() << "/" 
      <<  m_pad3->nX() << "/" 
      <<  m_pad3->nY() <<  endreq;

  drawStation(log,1,8);
  drawStation(log,0,16);
   
}	   
		   
void L0mTower::addBit(int ix, int iy, int st, L0mPad* srp ) {   
 
  std::pair<int,int> bit(ix,iy);
 
  m_bitmap[st][bit] = srp;
  
}

L0mTower::HitIndex L0mTower::searchStation(int st, int foiX, int cindex ) {

  StationMap stmap = m_bitmap[st];
  
  for (int inx=0; inx <= foiX; inx++ ) {
  
    HitIndex iplus(cindex+inx,0);
    if( stmap.find(iplus) != stmap.end() ) {
      return iplus;
    }
    
    HitIndex iminus(cindex-inx,0);
    if( stmap.find(iminus) != stmap.end() ) {
      return iminus;
    }
  }    
  return EMPTY_INDEX;
}

L0mPad* L0mTower::findTrack(std::vector<int> foiX, std::vector<int> foiY) {

  L0mPad* lpd = 0;

  // confirm the candidate in M4 & M5

  HitIndex hd = searchStation(4,foiX[4]);
  if(hd == EMPTY_INDEX) return 0;
  hd = searchStation(3,foiX[3]);
  if(hd == EMPTY_INDEX) return 0;
    
  // find the nearest hit in M2
  
  m_ind2 = searchStation(1,foiX[1]);
  if(m_ind2 == EMPTY_INDEX) return 0;
  m_pad2 = m_bitmap[1].find(m_ind2)->second;
      
  // extrapolate and find a hit in M1
  
  // round to the nearest integer
  int indext = (int)(abs(m_ind2.first)*3.58+0.5);  
  indext = (m_ind2.first<0) ? -indext : indext;
  m_ind1 = searchStation(0,foiX[0],indext);
  if(m_ind1 == EMPTY_INDEX) return 0;
  m_pad1 = m_bitmap[0].find(m_ind1)->second;        
  return m_pad1;
}

bool L0mTower::isFull() {

    return (m_bitmap[0].size() != 0) &&
           (m_bitmap[1].size() != 0) &&
	   (m_bitmap[3].size() != 0) &&
           (m_bitmap[4].size() != 0);

}

bool L0mTower::trackFound() {
// indicates that the track finding was succesful
    return isFull() && m_pad2 && m_pad1;
}

L0MuonCandidate* L0mTower::createCandidate(const std::vector<double>& ptpara,
                        		   const std::vector<int>& foiX, 
					   const std::vector<int>& foiY) {
  
  if ( !trackFound() ) {
    findTrack(foiX,foiY);
  }  
  if(trackFound()) {
    double d1 = ptpara[0] ;
    double d2 = ptpara[1] ;
    double d3 = ptpara[2] ;
    double alpha = ptpara[3] ;
  
    L0MuonCandidate* lmc = new L0MuonCandidate(pt(ptpara),
                        		       m_pad1->x(),
					       m_pad1->y(),
					       theta(ptpara),
					       phi(ptpara),
					       m_pad1,
					       m_pad2,
					       m_pad3) ;
    return lmc;			 
  } else {
    return 0;
  }  					   
}					   

double L0mTower::pt(const std::vector<double>& ptpara) {
  double d1 = ptpara[0] ;
  double d2 = ptpara[1] ;
  double d3 = ptpara[2] ;
  double alpha = ptpara[3] ;
    
  double x1 = m_pad1->x();
  double y1 = m_pad1->y();
  double x2 = m_pad2->x();
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

  return ptm;
}

double L0mTower::theta(const std::vector<double>& ptpara) {

  double d1 = ptpara[0] ;
  double d2 = ptpara[1] ;
  double d3 = ptpara[2] ;

  double x1 = m_pad1->x();
  double y1 = m_pad1->y();
  double x2 = m_pad2->x();
  double x0 = x1 - d2*(x2-x1)/d3;
  double y0 = y1*d1/(d1+d2);

  HepVector3D v(x0,y0,d1);	

  return v.theta();
}

double L0mTower::phi(const std::vector<double>& ptpara) {

  double d1 = ptpara[0] ;
  double d2 = ptpara[1] ;
  double d3 = ptpara[2] ;

  double x1 = m_pad1->x();
  double y1 = m_pad1->y();
  double x2 = m_pad2->x();
  double x0 = x1 - d2*(x2-x1)/d3;
  double y0 = y1*d1/(d1+d2);

  HepVector3D v(x0,y0,d1);	

  return v.phi();
}
