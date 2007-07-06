// $Id: HltSelectionDataToTes.cpp,v 1.2 2007-07-06 16:55:57 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// local
#include "HltSelectionDataToTes.h"
#include "HltBase/HltConfigurationHelper.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltSelectionDataToTes
//
// 2006-09-01 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltSelectionDataToTes );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltSelectionDataToTes::HltSelectionDataToTes( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

  declareProperty("CopyAll",m_copyAll = false);
  
  declareProperty("Copy", m_selections);

  declareProperty("DataSummaryLocation",
                  m_dataSummaryLocation = LHCb::HltSummaryLocation::Default);

}
//=============================================================================
// Destructor
//=============================================================================
HltSelectionDataToTes::~HltSelectionDataToTes() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltSelectionDataToTes::initialize() {

  StatusCode sc = StatusCode::SUCCESS;

  // get the Hlt Data Svc
  m_hltSvc = NULL;
  sc = serviceLocator()->service("HltDataSvc/EventDataSvc",m_hltSvc);
  if (!m_hltSvc) fatal() << " not able to create Hlt Svc provider " << endreq;
  info() << " hltsvc " << (int) m_hltSvc << endreq;
  
  std::string loca = m_dataSummaryLocation;
  Hlt::DataHolder<LHCb::HltSummary>* sholder = 
    get<Hlt::DataHolder<LHCb::HltSummary> >(m_hltSvc,loca);
  if (!sholder) fatal() << " not able to retrieve data summary " << endreq;
  m_datasummary = &(sholder->object());

  loca = m_dataSummaryLocation+"/Configuration";
  Hlt::DataHolder<Hlt::Configuration>* holder = 
    get<Hlt::DataHolder<Hlt::Configuration> >(m_hltSvc,loca);
  if (!holder) fatal() << " not able to retrieve configuration " << endreq;
  m_conf = &(holder->object());

  m_tracksSelIDs.clear();
  m_verticesSelIDs.clear();
  std::vector<int> ids;
  if (m_copyAll) ids = m_datasummary->selectionSummaryIDs();
  else {
    std::vector<std::string> sels = m_selections.value();
    for (std::vector<std::string>::iterator it = sels.begin();
         it != sels.end(); ++it) {
      std::string name = *it;
      int id = HltConfigurationHelper::getID(*m_conf,"SelectionID",name);    
      ids.push_back(id);
    }
  }

  for (std::vector<int>::iterator it = ids.begin(); it != ids.end(); ++it) {
    int id = *it;
    const std::string name  = 
      HltConfigurationHelper::getName(*m_conf,"SelectionID",id);
    std::string type = " unknown";
    if (m_conf->has_key(name+"/SelectionType")) {
      type  = m_conf->retrieve<std::string>(name+"/SelectionType");
      if (type == "Tracks") m_tracksSelIDs.push_back(id);
      else if (type == "Vertices") m_verticesSelIDs.push_back(id);
    }
    info() << " write on TES selection data " << name << " ID "
           << id << " type " << type << endreq;
  }
  
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================

StatusCode HltSelectionDataToTes::execute() {

  StatusCode sc = StatusCode::SUCCESS;

  std::string loca = "Hlt/Track/";
  for (std::vector<int>::iterator it = m_tracksSelIDs.begin();
       it != m_tracksSelIDs.end(); ++it) {
    int id = *it;
    std::string name = 
      HltConfigurationHelper::getName(*m_conf,"SelectionID",id);
    debug() << " copying " << name << endreq;
    if (!exist<Tracks>(loca+name)) {
      Tracks* otracks = copy<Track,Tracks>(id);
      if (otracks) put(otracks,loca+name);
    } 
  }

  loca = "Hlt/Vertex/";
  for (std::vector<int>::iterator it = m_verticesSelIDs.begin();
       it != m_verticesSelIDs.end(); ++it) {
    int id = *it;
    std::string name = 
      HltConfigurationHelper::getName(*m_conf,"SelectionID",id);
    debug() << " copying " << name << endreq;
    if (!exist<RecVertices>(loca+name)) { 
      RecVertices* overtices = copy<RecVertex,RecVertices>(id);
      if (overtices) put(overtices,loca+name);
    } 
  }
  
  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltSelectionDataToTes::finalize() {

  debug() << "==> Finalize" << endmsg;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
