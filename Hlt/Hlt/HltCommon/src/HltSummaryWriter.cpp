// $Id: HltSummaryWriter.cpp,v 1.2 2007-06-25 21:57:17 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "HltSummaryWriter.h"
#include "HltBase/HltConfigurationHelper.h"
#include "HltBase/HltSummaryHelper.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltSummaryWriter
//
// 2006-07-25 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltSummaryWriter );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltSummaryWriter::HltSummaryWriter( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty("SaveAll", m_saveAll = false);
  declareProperty("Save", m_selectionNames);
  declareProperty("DataSummaryLocation", 
                  m_dataSummaryLocation = LHCb::HltSummaryLocation::Default);
  
}
//=============================================================================
// Destructor
//=============================================================================
HltSummaryWriter::~HltSummaryWriter() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltSummaryWriter::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

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

  m_selectionIDs.clear();
  if (m_saveAll) {
    m_selectionIDs = m_datasummary->selectionSummaryIDs();
  } else {
    std::vector<std::string> cromos = m_selectionNames.value();
    for (std::vector<std::string>::iterator it = cromos.begin();
         it != cromos.end(); ++it) {
      std::string name = *it;
      int id = HltConfigurationHelper::getID(*m_conf,"SelectionID",name);
      m_selectionIDs.push_back(id);
    }
  }

  info() << " Write in Summary : ";
  for (std::vector<int>::iterator it = m_selectionIDs.begin();
       it != m_selectionIDs.end(); ++it) 
    info() << HltConfigurationHelper::getName(*m_conf,"SelectionID",*it);
  info() << endreq;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltSummaryWriter::execute() {

  debug() << "==> Execute" << endmsg;

  m_summary = new HltSummary();
  std::string loca = m_dataSummaryLocation;
  put( m_summary, loca );

  loca = m_dataSummaryLocation+"/Configuration";  
  put(new Hlt::DataHolder<Hlt::Configuration>(*m_conf),loca);

  verbose() << " data decision " << m_datasummary->decision() << endreq;
  std::vector<int> ids = m_datasummary->selectionSummaryIDs();
  for (std::vector<int>::iterator it = ids.begin(); 
       it != ids.end(); ++it) {
    int id = *it; 
    // printInfo(*m_datasummary,id);
  }

  for (std::vector<int>::iterator it = m_selectionIDs.begin();
       it != m_selectionIDs.end(); ++it) {
    int id = *it;
    if (m_datasummary->hasSelectionSummary(id)) {
      bool ok = m_datasummary->selectionSummary(id).decision();
      if (ok) writeSelection(id);
    } else 
      error() << " No selection in summary Data! " << endreq;
  }

  debug() << " report decision " << m_summary->decision() << endreq;
  ids = m_summary->selectionSummaryIDs();
  for (std::vector<int>::iterator it = ids.begin(); 
       it != ids.end(); ++it) {
    int id = *it;
    printInfo(*m_summary,id);
  }
      
  return StatusCode::SUCCESS;
}

void HltSummaryWriter::writeSelection(int id) {
  HltSelectionSummary& sdata   = m_datasummary->selectionSummary(id);
  HltSelectionSummary& report = m_summary->selectionSummary(id);
  report.setPattern(sdata.pattern());
  debug() << " writed selection " << report.pattern() << " "
          << HltConfigurationHelper::getName(*m_conf,"SelectionID",id) 
          << endreq;
  if (sdata.data().size()>0) {
    ContainedObject* obj = sdata.data().front();
    report.addData(*obj);
    debug() << " writed selection data " << report.data().size() << endreq;
  }
}

void HltSummaryWriter::printInfo(const HltSummary& sum, int id) {
  bool ok = sum.hasSelectionSummary(id);
  std::string name = 
    HltConfigurationHelper::getName(*m_conf,"SelectionID",id);
  bool dec = false;
  int ncan  = 0;
  if (ok) {
    dec = sum.selectionSummary(id).decision();
    ncan = HltSummaryHelper::ncandidates(sum,id);
  }
  debug() << " selection \t" << name 
          << "\t decision \t" << dec << " ncandidates \t" << ncan << endreq;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltSummaryWriter::finalize() {

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
