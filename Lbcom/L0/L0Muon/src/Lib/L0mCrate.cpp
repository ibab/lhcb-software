// $Id: L0mCrate.cpp,v 1.8 2002-05-07 07:29:09 atsareg Exp $

#ifdef WIN32
// Disable warning C4786 identifier truncated to 255 characters in debug info.
#pragma warning ( disable : 4786 )
#endif // WIN32

#include <vector>
#include <algorithm>

#include "GaudiKernel/MsgStream.h"

#include "Event/L0MuonCandidate.h"
#include "L0Muon/L0mCrate.h"
#include "L0Muon/L0mProcUnit.h"
#include "L0Muon/L0mTrigger.h"

L0mCrate::L0mCrate(int quarter, const L0mTrigger& ltp) : 
      m_quarter(quarter), m_processor(ltp)  {

  std::vector<MuonTileID> vmt = MuonLayout(2,2).tiles(quarter);
  
  //========================================
  // Build all the units for this crate
  //========================================
  std::vector<MuonTileID>::const_iterator iv;
  for(iv = vmt.begin(); iv != vmt.end(); iv++ ) {
    L0mProcUnit* lpu = new L0mProcUnit(m_processor.m_ptParameters,
                        	      m_processor.m_foiXSize,
				      m_processor.m_foiYSize, 
				      m_processor.m_extraM1, 
				      m_processor.m_precision, 
				      m_processor.m_bins, *iv);
    lpu ->setStation(2);				      
    m_units.push_back(lpu );
				      
  }
}

L0mCrate::~L0mCrate() {
  std::vector<L0mProcUnit*>::iterator ip;
  for ( ip = m_units.begin(); ip != m_units.end(); ip++) {
    delete *ip;
  }
  m_units.clear();
}	

L0Muon::StatusCode L0mCrate::execute(MsgStream& log) {

  m_candidates.clear();

  // Execute all the Processing Units

  std::vector<L0mProcUnit*>::iterator it;    
  std::vector<L0MuonCandidate*> lmc_pu;
  std::vector<L0MuonCandidate*>::iterator ilmc;

  L0Muon::StatusCode lsc;
  m_status = L0Muon::OK;

  log << MSG::DEBUG << "Executing Crate " << quarter() 
                    << ". Total active units: " << m_nActive << endreq;
		    
  if( m_nActive>0 ) {		    
    for (it=m_units.begin();it!=m_units.end();it++) {

      // (*it)->printParameters(log);

      lsc = (*it)->execute(log);
      if(lsc == L0Muon::OK) {
	lmc_pu = (*it)->candidates();
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
  }  
  log << MSG::DEBUG << "Crate: PU execution done, " 
      << m_candidates.size() << " candidates found" << endreq;
      
  // Sort candidates if the status is OK
  if(m_status == L0Muon::OK) {
    if (m_candidates.size() > 2) {    
      //cout << " # of candidate " << m_candidates.size() << endl;
      // Now sort the candidates in the Pt descending order
      std::sort(m_candidates.begin(),m_candidates.end(),ComparePt());
      //cout << "After sort " << endl;
      // Erase candidates with smaller Pt
      for (ilmc = m_candidates.end()-1;ilmc != m_candidates.begin()+1; ilmc--) {
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
  
  // cout << "Crate execution done" << endl;
  return L0Muon::StatusCode(m_status);
}

void L0mCrate::clear() {

  std::vector<L0mProcUnit*>::iterator it;
  for (it=m_units.begin();it!=m_units.end(); it++) {
      (*it)->clear();
  }
  m_nActive = 0;
}

#include <typeinfo>

void L0mCrate::buildUnits(const std::vector<L0mTower*>& towers ) {

  std::vector<L0mTower*>::const_iterator it;
  std::vector<L0mProcUnit*>::iterator ipu;
  MuonLayout pu_layout(2,2);
  MuonTileID mtile;
  L0mProcUnit* pu;
  
  for(it=towers.begin(); it!=towers.end(); it++) {
  
    MuonTileID lmp = (*it)->padM3();
    if(lmp.quarter() == m_quarter) {
      mtile = pu_layout.contains(lmp);
      pu = 0;
      for(ipu = m_units.begin(); ipu != m_units.end(); ipu++) {
        if( **ipu == mtile ) {
	  pu = *ipu;
	  break;
	}
      }
      if(!pu) {
        // cout << "!!!!!!!! Crate: failed to find PU " << endl;
	return;
      }	     
      pu->addTower(*it);  
      (*it)->setProcUnit(pu);   
      if ( pu->towers() == 1 ) m_nActive++;      
    }
  }  
}


