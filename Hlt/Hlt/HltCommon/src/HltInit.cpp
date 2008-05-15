// $Id: HltInit.cpp,v 1.14 2008-05-15 08:56:55 graven Exp $
// Include files

// from Gaudi
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/IDataManagerSvc.h" 
#include "GaudiKernel/AlgFactory.h"
// local
#include "HltInit.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltInit
//
// 2006-07-25 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_SERVICE_FACTORY( DataSvc );

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltInit );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltInit::HltInit( const std::string& name,
                  ISvcLocator* pSvcLocator)
  : HltBaseAlg ( name , pSvcLocator )
  , m_hltMan(0)
{
  
}
//=============================================================================
// Destructor
//=============================================================================
HltInit::~HltInit() {
  if (m_hltMan) {
    m_hltMan->clearStore();  // this deletes HLTData and HLTConf...
    m_hltMan->release();
    m_hltMan=0;
  }
  // we do it here, and not in finalize because
  // finalize is called in the same order as initialize
  // (as opposed to the opposite order)
  // and HltAlgorithms 'hang on' to their Selections, which live
  // in m_HLTData...
  // So because HltInit is  'first', this works in 'initialize'
  // but doesn't in 'finalize'
  // at some point I'll think of a solution to that...
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltInit::initialize() {
  StatusCode sc = HltBaseAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  // create the Hlt Data Svc
  if (!m_hltMan) {
  std::string name = "DataSvc/HltSvc";
  debug() << " creating hltSvc " << name << endreq;
  sc = serviceLocator()->service(name,m_hltMan,true);
      if ( sc.isFailure() ) return sc; 
  } else {
      m_hltMan->clearStore();
  }
  if (!m_hltMan) fatal() << " not able to create HltSvc " << endreq;
  info() << " created HltSvc " << endreq;
  m_hltMan->setRoot("/Event", new DataObject());
  
  put(&hltSvc(), new Hlt::Data(), hltDataLocation() );  
  info() << " stored Hlt::Data in HltSvc at " << hltDataLocation() << endreq;

  put(&hltSvc(), new Hlt::Configuration() , hltConfigurationLocation());
  info() << " stored Hlt::Configuration in HltSvc at " 
         << hltConfigurationLocation() << endreq;

  
  return StatusCode::SUCCESS;
}

StatusCode HltInit::reinitialize() {
  info() << " reinitialize! " << endreq;
  // StatusCode sc = finalize();
  // return sc.isSuccess()? initialize(): sc;
  return StatusCode::SUCCESS;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode HltInit::execute() {
  hltData().clean();
  setFilterPassed(true);
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltInit::finalize() {
  debug() << " finalize " << endreq;
  //m_hltMan->clearStore();  // this deletes HLTData 
  //                         // before other HltAlgorithms have finalized...
  return HltBaseAlg::finalize();
}

//=============================================================================
