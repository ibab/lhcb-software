// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/src/Lib/L0mCrate.cpp,v 1.2 2001-06-08 13:43:47 cattaneb Exp $

#include <vector>
#include <algorithm>

#include "GaudiKernel/MsgStream.h"

#include "L0Muon/L0MuonCandidate.h"
#include "L0Muon/L0mCrate.h"
#include "L0Muon/L0mProcUnit.h"

L0mCrate::L0mCrate(int quarter) : m_quarter(quarter)  {
}

L0mCrate::~L0mCrate() {
}	

L0Muon::StatusCode L0mCrate::execute(MsgStream& log) {

  m_candidates.clear();

  // Execute all the Processing Units

  std::vector<L0mProcUnit>::iterator it;    
  std::vector<L0MuonCandidate*> lmc_pu;
  std::vector<L0MuonCandidate*>::iterator ilmc;

  L0Muon::StatusCode lsc;
  m_status = L0Muon::OK;

  for (it=m_units.begin();it!=m_units.end();it++) {
    lsc = (*it).execute(log);
    if(lsc == L0Muon::OK) {
      lmc_pu = (*it).candidates();
      if (lmc_pu.size()>0) {
	  m_candidates.insert(m_candidates.end(),lmc_pu.begin(),lmc_pu.end());
      }
      m_status = L0Muon::OK;
    // Error condition is detected: erase candidates and set status  
    } else if (lsc == L0Muon::PU_OVERFLOW || lsc == L0Muon::PU_ERROR ) {
      clear();
      for (ilmc = m_candidates.begin(); ilmc != m_candidates.end(); ilmc++) {
        delete *ilmc;
      }
      m_candidates.clear();
      m_status = lsc;
      // Prepare two empty candidates with error status for transmission to DU
      m_candidates.push_back(new L0MuonCandidate(lsc));
      m_candidates.push_back(new L0MuonCandidate(lsc));
      // Nothing more to do because of the error
      break;
    } else if (lsc == L0Muon::PU_EMPTY ) {
      m_status = L0Muon::OK;
    }
  }

  // Sort candidates if the status is OK
  if(m_status == L0Muon::OK) {
    if (m_candidates.size() > 2) {
      // Now sort the candidates in the Pt descending order
      std::sort(m_candidates.begin(),m_candidates.end(),ComparePt());
      // Erase candidates with smaller Pt
      for (ilmc = m_candidates.begin()+2;ilmc != m_candidates.end(); ilmc++) {
        delete *ilmc;
	m_candidates.erase(ilmc);
      }
    } else if(m_candidates.size() == 1) {
      // Add one more empty candidate - we should have always two of them
      m_candidates.push_back(new L0MuonCandidate(L0Muon::EMPTY));
    } else if(m_candidates.size() == 0){
      // Nothing in the crate, add two empty candidates
      m_candidates.push_back(new L0MuonCandidate(L0Muon::EMPTY));
      m_candidates.push_back(new L0MuonCandidate(L0Muon::EMPTY));
    }	
  }  
  return L0Muon::StatusCode(m_status);
}

void L0mCrate::clear() {

  std::vector<L0mProcUnit>::iterator it;
  for (it=m_units.begin();it!=m_units.end(); it++) {
      (*it).clear();
  }
  m_units.clear();
}

void L0mCrate::buildUnits(const std::vector<double>& ptpara,
                          const std::vector<int>& foiX,
                          const std::vector<int>& foiY,
			  double precision,
			  int bits,
			  ObjectVector<L0mTower>* towers ) {

  ObjectVector<L0mTower>::iterator it;
  std::vector<L0mProcUnit>::iterator ipu;
  MuonLayout pu_layout(2,2);
  MuonTile mtile;
  L0mProcUnit pu;

  for(it=towers->begin(); it!=towers->end(); it++) {
    L0mPad* lmp = (*it)->padM3();
    if(lmp->quarter() == m_quarter) {
      mtile = pu_layout.contains(*lmp);
      pu = L0mProcUnit(ptpara, foiX, foiY, precision, bits, mtile);
      ipu = std::find(m_units.begin(),m_units.end(),pu);
      if(ipu == m_units.end()) {
        m_units.push_back(pu);
	ipu = m_units.end();
	ipu--;
      }
      (*ipu).addTower(*it);      
    }
  }  
}
