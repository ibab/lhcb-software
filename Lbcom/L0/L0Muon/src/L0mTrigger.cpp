/// Gaudi factories for algorithms and converters
#include "GaudiKernel/AlgFactory.h"

/// Gaudi base classes
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ObjectVector.h"

/// Utility classes
#include "GaudiKernel/MsgStream.h"

/// Private classes 
#include "L0Muon/L0MuonCandidate.h"
#include "L0mTrigger.h"

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
: Algorithm(name, pSvcLocator)
{
  
}


StatusCode L0mTrigger::initialize()   { 


  MsgStream log(msgSvc(), name());
  
  log << MSG::INFO << "L0mTrigger initialization starting..." << endreq;
  log << MSG::INFO << "Creating subAlgorithms..." << endreq;
  
  StatusCode sc;
  
  sc = createSubAlgorithm("L0mPadBuilder","L0mPadBuilder",m_padBuilder);
  sc = createSubAlgorithm("L0mTowerBuilder","L0mTowerBuilder",m_towerBuilder);
  sc = createSubAlgorithm("L0mTriggerProc","L0mTriggerProc",m_triggerProc);
  
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

// create L0mTowers  

  sc = m_towerBuilder->execute();  
  
  if( sc != StatusCode::SUCCESS ) {
    log << MSG::ERROR << "Towers creation failed !" << endreq;
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
	                    << (*ilcd)->padM1()->x() << "/"
			    << (*ilcd)->padM1()->y() << "/"
			    << (*ilcd)->padM1()->z() <<endreq; 
	  log << MSG::DEBUG << "    Pad M2: x/y/z: " 
	                    << (*ilcd)->padM2()->x() << "/"
			    << (*ilcd)->padM2()->y() << "/"
			    << (*ilcd)->padM2()->z() <<endreq; 	
	  log << MSG::DEBUG << "    Pad M3: x/y/z: " 
	                    << (*ilcd)->padM3()->x() << "/"
			    << (*ilcd)->padM3()->y() << "/"
			    << (*ilcd)->padM3()->z() <<endreq; 		    	    			    
      }
  }    
 
// Return 
  
  return StatusCode::SUCCESS;
}

StatusCode L0mTrigger::finalize()  {
  return StatusCode::SUCCESS;
}

