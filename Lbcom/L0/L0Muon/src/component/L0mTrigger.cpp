// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/src/component/L0mTrigger.cpp,v 1.1 2001-06-07 16:34:18 atsareg Exp $

/// Gaudi factories for algorithms and converters
#include "GaudiKernel/AlgFactory.h"

/// Gaudi base classes
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ObjectVector.h"

/// Utility classes
#include "GaudiKernel/MsgStream.h"

/// Private classes 
#include "L0Muon/L0MuonCandidate.h"
#include "L0Muon/L0mTrigger.h"

//------------------------------------------------------------------------------
//
/// Implementation of the class :  L0mTrigger
//
/// Author: A.Tsaregorodtsev
//
//------------------------------------------------------------------------------


static const AlgFactory<L0mTrigger>  Factory;
const IAlgFactory& L0mTriggerFactory = Factory;

/// Algorithm parameters which can be set at run time must be declared.
/// This should be done in the constructor.
L0mTrigger::L0mTrigger(const std::string& name, ISvcLocator* pSvcLocator)
: Algorithm(name, pSvcLocator) {

  m_triggerProcMode = "Detailed";
  declareProperty("TriggerProcMode", m_triggerProcMode);
  
}


StatusCode L0mTrigger::initialize()   { 


  MsgStream log(msgSvc(), name());
  
  log << MSG::INFO << "L0mTrigger initialization starting..." << endreq;
  log << MSG::INFO << "Creating subAlgorithms for " 
                   << m_triggerProcMode << " mode"
		   << endreq;
  
  StatusCode sc;
  
  sc = createSubAlgorithm("L0mPadBuilder","L0mPadBuilder",m_padBuilder);
  if ( m_triggerProcMode == "Detailed" ) {
    sc = createSubAlgorithm("L0mTriggerProcD","L0mTriggerProc",m_triggerProc);
  } else {
    sc = createSubAlgorithm("L0mTriggerProc","L0mTriggerProc",m_triggerProc);
  }  
  
  return StatusCode::SUCCESS;
}

StatusCode L0mTrigger::execute() {

  MsgStream log(msgSvc(), name());
  StatusCode sc;
  
  log << MSG::DEBUG << "L0mTrigger executes" << endreq;
  
// create the collection of L0mPads

  sc = m_padBuilder->execute();  
 
  if( sc != StatusCode::SUCCESS ) {
    log << MSG::ERROR << "Pad creation failed !" << endreq;
    return sc;
  }
  
// Do the trigger now

  sc = m_triggerProc->execute();    
  
  if( sc != StatusCode::SUCCESS ) {
    log << MSG::ERROR << "Trigger processor algorithm failed !" << endreq;
    return sc;
  }
  
  
  // Debug printout
  
  SmartDataPtr< ObjectVector<L0MuonCandidate> > lcd(eventSvc(),"/Event/MC/L0MuonCandidates");
  
  if( !lcd ) {
    log << MSG::DEBUG << " No triggers, sorry..." << endreq;
  } else {
    ObjectVector<L0MuonCandidate>::iterator ilcd;
    for (ilcd=lcd->begin(); ilcd != lcd->end(); ilcd++) {
      log << MSG::DEBUG << " Trigger: Pt/x/y/theta/phi " 
			<< (*ilcd)->pt() <<  "/"
			<< (*ilcd)->x() <<  "/"
			<< (*ilcd)->y() <<  "/"
			<< (*ilcd)->theta() <<  "/"
			<< (*ilcd)->phi()  << endreq;

      log << MSG::DEBUG << " Pads used:  " << endreq;
      log << MSG::DEBUG << "    Pad M1: x/y/z: " 
	                << (*ilcd)->pad(0)->x() << "/"
			<< (*ilcd)->pad(0)->y() << "/"
			<< (*ilcd)->pad(0)->z() <<endreq; 
      log << MSG::DEBUG << "    Pad M2: x/y/z: " 
	                << (*ilcd)->pad(1)->x() << "/"
			<< (*ilcd)->pad(1)->y() << "/"
			<< (*ilcd)->pad(1)->z() <<endreq; 	
      log << MSG::DEBUG << "    Pad M3: x/y/z: " 
	                << (*ilcd)->pad(2)->x() << "/"
			<< (*ilcd)->pad(2)->y() << "/"
			<< (*ilcd)->pad(2)->z() <<endreq;						  
    }
  }    
 
// Return 
  
  return StatusCode::SUCCESS;
}

StatusCode L0mTrigger::finalize()  {
  return StatusCode::SUCCESS;
}

