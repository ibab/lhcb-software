// $Id: L0mProcUnit.cpp,v 1.15 2003-12-10 17:16:53 cattanem Exp $

#include <algorithm>
#include <math.h>

#include "GaudiKernel/MsgStream.h"

#include "L0Muon/L0mProcUnit.h"
#include "L0Muon/L0mPad.h"
#include "L0Muon/L0mCrate.h"

L0mProcUnit::L0mProcUnit() {
}

L0mProcUnit::L0mProcUnit(const std::vector<double>& ptpara,
                	 const std::vector<int>& foiX,
                	 const std::vector<int>& foiY,
			 const std::vector<int>& extM1,
			 double precision,
			 int bins,
			 const MuonTileID& mt):
			 MuonTile(mt),
			 m_ptParameters(ptpara),
			 m_foiX(foiX),
			 m_foiY(foiY),
			 m_extraM1(extM1),
			 m_precision(precision),
			 m_bins(bins),
			 m_seedClustering(false) {} 
		
L0mProcUnit::~L0mProcUnit() {}		
	

L0Muon::StatusCode L0mProcUnit::execute(MsgStream& log) {
					
    std::vector<L0MuonCandidate*> lmc;
    std::vector<L0MuonCandidate*>::iterator ilmc;
    std::vector<L0mTower*>::iterator it;
    L0MuonCandidate* lcd=0; 
    int nCandidate = 0;
    
    if(m_seedClustering) cleanAdjacentSeeds();
    
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
        log << "Pad in M3: " << (*it)->pad(2) << endreq;
        log << "Pad in M2: " << (*it)->pad(1) << endreq;
        log << "Pad in M1: " << (*it)->pad(0) << endreq;
        log << MSG::DEBUG << " Pt= "  << lcd->pt()                 
            << " xM1= " << (*it)->pad(0).nX() 
            << " yM1= " << (*it)->pad(0).nY() << endreq;
        m_candidates.push_back(lcd);
        nCandidate++;
      } 
    }
    
    if(nCandidate>2) {
      // PU overflow. Choose the 2 candidates with the highest Pt 
      std::sort(m_candidates.begin(),m_candidates.end(),ComparePt());
      // Erase candidates with smaller Pt
      for (ilmc = m_candidates.end()-1;ilmc != m_candidates.begin()+1;
           ilmc--) {
        delete *ilmc;
        m_candidates.erase(ilmc);
      }
      m_status = L0Muon::PU_OVERFLOW;  
      log << MSG::DEBUG << "PU overflow detected !" << endreq;    
    } else if ( nCandidate == 0 ) {
      m_status = L0Muon::PU_EMPTY;
    } else {
      m_status = L0Muon::OK;	
    }	
    
    return L0Muon::StatusCode(m_status);
}


void L0mProcUnit::clear() {
  m_towers.clear();
  m_candidates.clear();
  m_status=L0Muon::PU_EMPTY; 
}

void L0mProcUnit::printParameters(MsgStream& log) {

  log << "ProcUnit ID: " << id().quarter() << "/" 
                         << id().region() << "/"
			 << id().nX() << "/"
			 << id().nY() << " at " << this << endreq;
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

void L0mProcUnit::cleanAdjacentSeeds() { 
 
  int nsize = m_towers.size();
  if(nsize<2) return;
  
  //cout << "========= Removing seed clusters " << nsize << endl;  
  bool remove = false;
  std::vector<bool> flag(nsize,true);
  //for (int k = 0; k<nsize; k++ ) {    
  // cout << "Before cleaning " << m_towers[k]->padM3() << 
  //         " " << flag[k] << endl;
  //}
  
  for ( int i = 0; i<nsize-1; i++) {
    //cout << m_towers[i]->padM3() << endl;
    int nx1 = m_towers[i]->padM3().nX();
    int ny1 = m_towers[i]->padM3().nY();
    for ( int j = i+1; j < nsize; j++) {
      int nx2 = m_towers[j]->padM3().nX();
      int ny2 = m_towers[j]->padM3().nY();
      //      if(fabs(nx1-nx2)<2 && fabs(ny1-ny2)<2) {
      if(abs(nx1-nx2)<2 &&  ny1==ny2 ) {
        remove = true;
        if(nx1<nx2) {
          flag[j] = false;
        } else if (nx1>nx2) {
          flag[i] = false;
          //        } else if (ny1<ny2) {
          //flag[j] = false;
          //} else if (ny1>ny2) {
          //flag[i] = false;	  
        } else {
          //std::cout << "Wrong selection logic !! nx1, nx2 =" << nx1 
          //          << " " <<  nx2 << std::endl;
        }
      }
    }
  }
  //cout << m_towers[nsize-1]->padM3() << endl;
  
  if (!remove) {
    //cout << "===== Nothing removed" << endl;
    return;
  } else {
    //cout << "===== Removed seeds:" << endl;
  }
  std::vector<L0mTower*> tmp;
  std::vector<L0mTower*>::iterator itmp;

  for ( int it = 0; it<nsize; it++) {
    if (flag[it]) {
      tmp.push_back(m_towers[it]);
      //cout << "Retained tower " << m_towers[it]->padM3() << endl;
    } else {
      //cout << "Rejected tower " << m_towers[it]->padM3() << endl;
    }  
  }
  m_towers.clear();
  //std::copy(tmp.begin(),tmp.end(),m_towers.begin());
  for (itmp = tmp.begin(); itmp != tmp.end(); itmp ++) {
    m_towers.push_back(*itmp);
  }
  

  nsize = m_towers.size();
  //cout << "After cleaning nsize =" << nsize << endl;
  //cout << "After cleaning nsize tmp=" << tmp.size() << endl;
  

  //for (int k1 = 0; k1<nsize; k1++ ) {    
  //  cout << " -> After cleaning " << m_towers[k1]->padM3() << endl;   
  //}
}
