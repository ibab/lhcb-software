// $Id: HltInit.cpp,v 1.13 2008-04-21 12:57:05 hernando Exp $
// Include files

// from Gaudi
#include <boost/lexical_cast.hpp>
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/IDataManagerSvc.h" 
#include "GaudiKernel/AlgFactory.h"
#include "Event/HltEnums.h"
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
{
  
}
//=============================================================================
// Destructor
//=============================================================================
HltInit::~HltInit() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltInit::initialize() {
  StatusCode sc = HltBaseAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  // create the Hlt Data Svc
  m_hltMan = NULL;
  std::string name = "DataSvc/HltSvc";
  debug() << " creating hltSvc " << name << endreq;
  sc = serviceLocator()->service(name,m_hltMan,true);
  if (!m_hltMan) fatal() << " not able to create HltSvc " << endreq;
  info() << " created HltSvc " << endreq;
  m_hltMan->setRoot("/Event", new DataObject());
  
  std::string loca = hltDataLocation();
  m_HLTData = new Hlt::Data();
  put(&hltSvc(), m_HLTData, loca);  
  info() << " stored Hlt::Data in HltSvc at " << hltDataLocation() << endreq;

  loca = hltConfigurationLocation();
  m_HLTConf = new Hlt::Configuration();
  put(&hltSvc(), m_HLTConf,loca);
  info() << " stored Hlt::Configuration in HltSvc at " 
         << hltConfigurationLocation() << endreq;

  saveConfiguration();
  
  return StatusCode::SUCCESS;
}


void HltInit::saveConfiguration() {

  bool info = m_info; m_info = 0;
  for (int i = 0; i < HltEnums::HltSelLastSelection; ++i) {
    std::string name = HltEnums::HltSelectionSummaryEnumToString(i);
    if (name != "selectionSummaryUnknown") {
      confregister(name,i,"SelectionID");
      std::string sid = boost::lexical_cast<std::string>(i);
      confregister(sid,name,"SelectionID");
    }
  }
  

  for (int i = 0; i <= HltEnums::Calo3DChi2; ++i) {
    std::string name = HltEnums::HltParticleInfoToString(i);
    if (name != "particleInfoUnknown") {
      confregister(name,i,"InfoID");
      std::string sid = boost::lexical_cast<std::string>(i);
      confregister(sid,name,"InfoID");
    }  
  }
  m_info = info;
  
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode HltInit::execute() {
  

  std::vector<Hlt::Selection*>& selections = m_HLTData->selections();
  for (std::vector<Hlt::Selection*>::iterator it = selections.begin();
       it != selections.end(); ++it) {
    Hlt::Selection& sel = *(*it);
    if (sel.decision()) sel.clean();
  }

  setFilterPassed(true);
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltInit::finalize() {

  m_hltMan->clearStore();

  StatusCode sc = HltBaseAlg::finalize();
  debug() << " finalize " << sc << endreq;
  return sc;
}

//=============================================================================
