// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/src/Lib/L0mProcUnit.cpp,v 1.4 2001-07-10 10:21:33 atsareg Exp $
#include "GaudiKernel/MsgStream.h"

#include "L0Muon/L0mProcUnit.h"
#include "L0Muon/L0mPad.h"

L0mProcUnit::L0mProcUnit() {
}

L0mProcUnit::L0mProcUnit(const std::vector<double>& ptpara,
                	 const std::vector<int>& foiX,
                	 const std::vector<int>& foiY,
			 const std::vector<int>& extM1,
			 double precision,
			 int bins,
			 const MuonTile& mt):
			 MuonTile(mt),
			 m_ptParameters(ptpara),
			 m_foiX(foiX),
			 m_foiY(foiY),
			 m_extraM1(extM1),
			 m_precision(precision),
			 m_bins(bins) {} 
		
L0mProcUnit::~L0mProcUnit() {}		
	

L0Muon::StatusCode L0mProcUnit::execute(MsgStream& log) {
					
    std::vector<L0MuonCandidate*> lmc;
    std::vector<L0MuonCandidate*>::iterator ilmc;
    std::vector<L0mTower*>::iterator it;
    L0MuonCandidate* lcd=0; 
    int nCandidate = 0;
    
    if(m_towers.empty()) {
      m_status = L0Muon::PU_EMPTY;
      return L0Muon::StatusCode(m_status);
    }
    
    for ( it=m_towers.begin(); it != m_towers.end(); it++ ) {
      lcd = 0;      
      if ((*it)->isFull()) {
        lcd = (*it)->createCandidate();  
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
      // Take account of the limited to 7 bits Pt presentation
      precisionPt();
      m_status = L0Muon::OK;	
    }	
    
    return L0Muon::StatusCode(m_status);
}

void L0mProcUnit::precisionPt() {

  // Take account of the limited to x bits Pt presentation
  std::vector<L0MuonCandidate*>::iterator ilmc;
  if(!m_candidates.empty()) {
    for (ilmc = m_candidates.begin(); ilmc != m_candidates.end(); ilmc++) {
      double realPt = (*ilmc)->pt();
      int roundedPt = int((fabs(realPt)+m_precision/2.)/m_precision);
      if ( roundedPt > m_bins ) roundedPt = m_bins;
      double newPt = roundedPt*m_precision;
      if ( realPt < 0.) newPt = -newPt;
      (*ilmc)->setPt(newPt);
    }
  }  
}

void L0mProcUnit::clear() {
  m_towers.clear();
  m_candidates.clear();
  m_status=L0Muon::PU_EMPTY; 
}

void L0mProcUnit::printParameters(MsgStream& log) {

  log << "ProcUnit ID: " << quarter() << "/" 
                         << region() << "/"
			 << nX() << "/"
			 << nY() << " at " << this << endreq;
  std::vector<double>::const_iterator iv;    
  log << "Pt parameters: " ;
  for (iv = m_ptParameters.begin(); iv != m_ptParameters.end(); iv++) {
    log << *iv << " ";
  }
  log << endreq;

  std::vector<int>::const_iterator ii;  
  log << "Field of interest X: " ;
  for (ii = m_foiX.begin(); ii != m_foiX.end(); ii++) {
    log << *ii << " ";
  }
  log << endreq;
  log << "Field of interest Y: " ;
  for (ii = m_foiY.begin(); ii != m_foiY.end(); ii++) {
    log << *ii << " ";
  }
  log << endreq;
  log << "Extrapolation to M1: " ;
  for (ii = m_extraM1.begin(); ii != m_extraM1.end(); ii++) {
    log << *ii << " ";
  }
  log << endreq;
  log << "Pt precision: " << m_precision << endreq;
  log << "bins for Pt encoding: " << m_bins << endreq;  		   
}

void L0mProcUnit::addTower(L0mTower* lt) { 

  m_towers.push_back(lt); 
}

// bool operator==(L0mProcUnit pu1, L0mProcUnit pu2) {
//     return pu1.quarter()==pu2.quarter() &&
//            pu1.region()==pu2.region() &&
// 	   pu1.nX()==pu2.nX() &&
// 	   pu1.nY()==pu2.nY();
// }
