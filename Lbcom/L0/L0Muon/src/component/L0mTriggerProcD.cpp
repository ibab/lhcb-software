// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/src/component/L0mTriggerProcD.cpp,v 1.2 2001-06-08 18:27:31 atsareg Exp $

/// Include files
/// Gaudi interfaces
#include "GaudiKernel/IDataProviderSvc.h"

/// Gaudi factories for algorithms 
#include "GaudiKernel/AlgFactory.h"

/// Gaudi base classes
#include "GaudiKernel/SmartDataPtr.h"

/// Utility classes
#include "GaudiKernel/MsgStream.h"

/// Private classes to the example
#include "L0Muon/L0MuonCandidate.h"
#include "L0Muon/L0mTriggerProcD.h"
#include "L0Muon/L0mTower.h"

//------------------------------------------------------------------------------
//
/// Implementation of class :  L0mTriggerProcD
//
/// Author: A.Tsaregorodtsev
//
//------------------------------------------------------------------------------


static const AlgFactory<L0mTriggerProcD>  Factory;
const IAlgFactory& L0mTriggerProcDFactory = Factory;

/// Algorithm parameters which can be set at run time must be declared.
/// This should be done in the constructor.
L0mTriggerProcD::L0mTriggerProcD(const std::string& name, ISvcLocator* pSvcLocator)
: L0mTriggerProc(name, pSvcLocator) {
  declareProperty("PtPrecision", m_precision);
  declareProperty("PtMaxBits", m_bits);
}

StatusCode L0mTriggerProcD::initialize()   { 

    MsgStream log(msgSvc(), name());

    log << MSG::INFO << "Initialization of a detailed Processor" << endreq;
    
    L0mTriggerProc::initialize();
    for( int iq=1; iq<=4; iq++) {
      m_crates.push_back(L0mCrate(iq));
    }  
    
    // Debug printout
    std::vector<double>::const_iterator iv;    
    log << MSG::DEBUG << "Pt parameters: " ;
    for (iv = m_ptParameters.begin(); iv != m_ptParameters.end(); iv++) {
      log << *iv << " ";
    }
    log << endreq;
    
    std::vector<int>::const_iterator ii;  
    log << MSG::DEBUG << "Field of interest X: " ;
    for (ii = m_foiXSize.begin(); ii != m_foiXSize.end(); ii++) {
      log << *ii << " ";
    }
    log << endreq;
    log << MSG::DEBUG << "Field of interest Y: " ;
    for (ii = m_foiYSize.begin(); ii != m_foiYSize.end(); ii++) {
      log << *ii << " ";
    }
    log << endreq;
    log << MSG::DEBUG << "Pt precision: " << m_precision << endreq;
    log << MSG::DEBUG << "bits for Pt encoding: " << m_bits << endreq;
    return StatusCode::SUCCESS;
}

StatusCode L0mTriggerProcD::execute() {

  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << "execute" << endreq;
  
  //===============================
  // get Muon digitisations
  //===============================
    
  log << MSG::DEBUG << "Retrieving L0mPads...  ";  
  
  SmartDataPtr< ObjectVector<L0mPad> > pads(eventSvc(),"/Event/MC/L0mPads");
  if(!pads) {
    return StatusCode::FAILURE;
  }
  log << "Done, # of L0mPads " << pads->size() << endl;;  
  ObjectVector<L0mPad>::const_iterator ip;
  
  //=========================================
  // create the collection of L0mTowers
  //=========================================
  StatusCode sc;
  
  log << MSG::DEBUG << "Creating towers...  "; 
   
  m_towers = new ObjectVector<L0mTower>;
  if ( !m_towers ) {
    log << MSG::ERROR << "Unable to create L0mTowers" << endreq;
    return StatusCode::FAILURE ;
  }
  
  /// Register towers to the transient store onle in Test mode
  if( m_mode == "Test" ) {
    sc = eventSvc()->registerObject("/Event/MC/L0mTowers",m_towers);
    if(sc.isFailure() ) {
      log << MSG::ERROR << "Unable to register L0mTowers in " << endreq;
      delete m_towers;
      return StatusCode::FAILURE ;
    }
  }
  
  L0mTower* lt; 
  
  for(ip=pads->begin(); ip != pads->end(); ip++ ) {
    if((*ip)->station() == 2) {
      lt = createTower(*ip, pads);
      m_towers->push_back(lt);
    }       
  }
  log << "Done, # of towers "<< m_towers->size() << endreq;
  //=========================================
  // register trigger candidates  
  //=========================================


  ObjectVector<L0MuonCandidate>* cand = new ObjectVector<L0MuonCandidate>;
  if( cand == 0 ) {
    log << MSG::ERROR << "Unable to create L0MuonCandidates" << endreq;
    return StatusCode::FAILURE ;
  }
  log << MSG::DEBUG << "Registering candidates ...  "  ;  
  sc = eventSvc()->registerObject(m_outputCandidates,cand);
  if(sc.isFailure() ) {
    log << MSG::ERROR << "Unable register L0MuonCandidates in " 
                      << m_outputCandidates << endreq;
    delete cand;
    return StatusCode::FAILURE ;
  }
  log << MSG::DEBUG << "done, status " << sc << endreq;  
  
  //===================
  // Build crates 
  //=================== 

  log << MSG::DEBUG << "Building crates "  << endreq;  

  std::vector<L0mCrate>::iterator ic;

  for(ic=m_crates.begin(); ic!=m_crates.end(); ic++) {
    (*ic).buildUnits(m_ptParameters,
                     m_foiXSize,
		     m_foiYSize,
		     m_precision,
		     m_bits,
		     m_towers);
  }
  
  //=====================
  // Execute crates 
  //=====================   
  
  log << MSG::DEBUG << "Executing crates "  << endreq;  

  std::vector<L0MuonCandidate*> lmcrate;
  std::vector<L0MuonCandidate*>::const_iterator ilmc;
  
  for(ic=m_crates.begin(); ic!=m_crates.end(); ic++) {
    (*ic).execute(log);
    lmcrate=(*ic).candidates();
    if (lmcrate.size()>0) {
      for(ilmc=lmcrate.begin(); ilmc!=lmcrate.end(); ilmc++) {
        cand->push_back(*ilmc);
      }  	
    }
    // Make cleanup after the work is done
    (*ic).clear();
  }
  
  //======================================
  // Execution is over. Clean up towers
  //======================================
  
  ObjectVector<L0mTower>::iterator it;
  if( m_mode != "Test" ) {
    for ( it=m_towers->begin(); it != m_towers->end(); it++ ) {
      delete *it;
    }	  
    delete m_towers;
  } 
  
  ObjectVector<L0MuonCandidate>::iterator im;
  for(im = cand->begin(); im != cand->end(); im++ ) {
    if ( (*im)->status() == L0Muon::OK ) {
      log << MSG::DEBUG << "Pt of the candidate = " 
                        << (*im)->pt() << endreq;   
    }			 
  }
  
  return StatusCode::SUCCESS;
}

StatusCode L0mTriggerProcD::finalize()  {
  return StatusCode::SUCCESS;
}



