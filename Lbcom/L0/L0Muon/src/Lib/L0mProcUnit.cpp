// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/src/Lib/L0mProcUnit.cpp,v 1.2 2001-06-08 18:28:49 atsareg Exp $
#include "GaudiKernel/MsgStream.h"

#include "L0Muon/L0mProcUnit.h"
#include "L0Muon/L0mPad.h"
#include "L0Muon/L0MuonCandidate.h"

L0mProcUnit::L0mProcUnit() {
}

L0mProcUnit::L0mProcUnit(const std::vector<double>& ptpara,
                	 const std::vector<int>& foiX,
                	 const std::vector<int>& foiY,
			 double precision,
			 int bits,
			 MuonTile& mt):
			 MuonTile(mt),
			 m_ptParameters(ptpara),
			 m_foiX(foiX),
			 m_foiY(foiY),
			 m_precision(precision),
			 m_bits(bits) {} 
		
L0mProcUnit::~L0mProcUnit() {}		
	

L0Muon::StatusCode L0mProcUnit::execute(MsgStream& log) {
					
    std::vector<L0MuonCandidate*> lmc;
    std::vector<L0MuonCandidate*>::iterator ilmc;
    std::vector<L0mTower*>::iterator it;
    L0MuonCandidate* lcd=0; 
    int nCandidate = 0;
    
    for ( it=m_towers.begin(); it != m_towers.end(); it++ ) {
      lcd = 0;
      if ((*it)->isFull()) {
        lcd = (*it)->createCandidate(m_ptParameters,m_foiX,m_foiY);  
      }   
      if(lcd) {        
        //  Track found !
        log << MSG::DEBUG << "Track found" << endreq;
        (*it)->draw(log << MSG::DEBUG);
        (*it)->pad(2)->print(log );
        (*it)->pad(1)->print(log );
        (*it)->pad(0)->print(log );
        log << MSG::DEBUG << " Pt= "    << lcd->pt()
                          << " Theta= " << lcd->theta()
                          << " Phi= "   << lcd->phi() << endreq;
        m_candidates.push_back(lcd);
	nCandidate++;
      } 
    }
    
    // Take account of the limited to 7 bits Pt presentation
    precisionPt();
    
    if(nCandidate>2) {
      // PU overflow. We are responsible to do the cleanup
      for(ilmc=m_candidates.begin(); ilmc != m_candidates.end(); ilmc++) {
        delete *ilmc;
      }
      m_candidates.clear();
      m_status = L0Muon::PU_OVERFLOW;      
    } else if ( nCandidate == 0 ) {
      m_status = L0Muon::PU_EMPTY;
    } else {
      m_status = L0Muon::OK;	
    }	
    
    return L0Muon::StatusCode(m_status);
}

void L0mProcUnit::precisionPt() {

  // Take account of the limited to x bits Pt presentation
  std::vector<L0MuonCandidate*>::iterator ilmc;
  for (ilmc = m_candidates.begin(); ilmc != m_candidates.end(); ilmc++) {
    double realPt = (*ilmc)->pt();
    int roundedPt = int((fabs(realPt)+m_precision/2.)/m_precision);
    if ( roundedPt > m_bits ) roundedPt = m_bits;
    double newPt = roundedPt*m_precision;
    if ( realPt < 0.) newPt = -newPt;
    (*ilmc)->setPt(newPt);
  }
}

void L0mProcUnit::clear() {
  m_towers.clear();
  m_status=L0Muon::PU_EMPTY; 
}

bool operator==(L0mProcUnit pu1, L0mProcUnit pu2) {
    return pu1.quarter()==pu2.quarter() &&
           pu1.region()==pu2.region() &&
	   pu1.nX()==pu2.nX() &&
	   pu1.nY()==pu2.nY();
}
