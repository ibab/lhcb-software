// $Id: HltInit.cpp,v 1.8 2007-10-11 16:43:51 hernando Exp $
// Include files 

// from Gaudi
#include <boost/lexical_cast.hpp>
#include "GaudiKernel/AlgFactory.h" 

// local
#include "HltInit.h"
#include "Event/HltSummary.h"
// #include "HltBase/ESequences.h"
#include "Event/HltNames.h"


using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltInit
//
// 2006-07-25 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltInit );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltInit::HltInit( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  // location of the summary and the summary box name
  declareProperty("TCKName", m_TCKName = "Default");

  // location of the summary and the summary box name
  declareProperty("DataSummaryLocation",
                  m_dataSummaryLocation = LHCb::HltSummaryLocation::Default);
  
}
//=============================================================================
// Destructor
//=============================================================================
HltInit::~HltInit() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltInit::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  evtSvc();
  
  // create the Hlt Data Svc
  m_hltSvc = NULL;
  std::string name = "HltDataSvc/EventDataSvc";
  sc = serviceLocator()->service(name,m_hltSvc,true);
  // m_hltSvc = svc<IDataManagerSvc>("HltDataSvc",true);  
  if (!m_hltSvc) fatal() << " not able to create Hlt Svc " << endreq;
  else info() << " created Hlt Data Svc! " << endreq;
  m_hltSvc->setRoot("/Event", new DataObject());

  IDataProviderSvc* hltsvc = NULL;
  sc = serviceLocator()->service("HltDataSvc/EventDataSvc",hltsvc);
  if (!hltsvc) error() << " not able to create Hlt Svc provider " << endreq;
  else debug() << " SUCCESSFULLY CREATED provider!! " << endreq;

  std::string loca = m_dataSummaryLocation;
  put(hltsvc, new Hlt::DataHolder<LHCb::HltSummary>(m_datasummary),loca);  
  info() << " stored hlt summary data " << endreq;

  loca = m_dataSummaryLocation+"/Configuration";
  put(hltsvc,new Hlt::DataHolder<Estd::dictionary>(m_hltConfiguration),loca);
  m_hltConfiguration.add("TCKName",m_TCKName);
  info() << " stored hlt configuration " << m_TCKName << endreq;


  // this two services should be different
  debug() << " hlt data svc " << (int) m_hltSvc << endreq;
  debug() << " event data svc " << (int) evtSvc() << endreq;

  saveConfiguration();
  
  return StatusCode::SUCCESS;
}


void HltInit::saveConfiguration() {


  // Temporally ON
  for (int i = 0; i < HltEnums::HltSelLastSelection; ++i) {
    std::string name = HltNames::selectionSummaryStr(i);
    if (name != "selectionSummaryUnknown") {
      m_hltConfiguration.add("SelectionID/"+name,i);
      info() << " HLT[SelectionID/" << name << "] = " << i << endreq;
      std::string sid = boost::lexical_cast<std::string>(i);
      m_hltConfiguration.add("SelectionID/"+sid,name);      
    }
  }
  
  for (int i = 0; i <= HltEnums::Calo3DChi2; ++i) {
    std::string name = HltNames::particleInfoStr(i);
    if (name != "particleInfoUnknown") {
      m_hltConfiguration.add("InfoID/"+name,i);
      info() << " HLT[InfoID/" << name << "] = " << i << endreq;
      std::string sid = boost::lexical_cast<std::string>(i);
      m_hltConfiguration.add("InfoID/"+sid,name);      
    }    
  }
  
  
  // store the Hlt configuration

  // Temporally OFF
  // std::vector<std::string> algonames =  HltNames::selectionSummaryNames();
//   for (std::vector<std::string>::const_iterator it = algonames.begin();
//        it != algonames.end(); ++it) {
//     const std::string& name = *it;
//     int id = HltNames::selectionSummaryID(name);
//     m_hltConfiguration.add("SelectionID/"+name,id);
//     std::string sid = boost::lexical_cast<std::string>(id);
//     m_hltConfiguration.add("SelectionID/"+sid,name);
//   }
  
//   std::vector<std::string> infonames =  HltNames::particleInfoNames();
//   for (std::vector<std::string>::const_iterator it = infonames.begin();
//        it != infonames.end(); ++it) {
//     const std::string& name = *it;
//     int id = HltNames::particleInfoID(name);
//     m_hltConfiguration.add("ExtraInfoID/"+name,id);
//     std::string sid = boost::lexical_cast<std::string>(id);
//     m_hltConfiguration.add("ExtraInfoID/"+sid,name);
//   }
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode HltInit::execute() {

  debug() << " decision " << m_datasummary.decision() << endreq;
  m_datasummary.setPattern(0);
  std::vector<int> ids = m_datasummary.selectionSummaryIDs();
  for (std::vector<int>::iterator it = ids.begin(); 
       it != ids.end(); ++it) {
    int id = *it;
    m_datasummary.selectionSummary(id).setPattern(0);
  }
      
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltInit::finalize() {

  m_hltSvc->clearStore();
  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
