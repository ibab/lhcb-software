// $Id: HltSummaryWriter.cpp,v 1.8 2008-01-22 09:56:37 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "HltSummaryWriter.h"

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
  : HltBaseAlg ( name , pSvcLocator )
{
  declareProperty("Save", m_AselectionNames);
  declareProperty("SaveAll", m_saveAll = false);
  declareProperty("HltSummaryLocation", 
                  m_hltSummaryLocation = LHCb::HltSummaryLocation::Default);  
}
//=============================================================================
// Destructor
//=============================================================================
HltSummaryWriter::~HltSummaryWriter() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltSummaryWriter::initialize() {
  StatusCode sc = HltBaseAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_selectionNames.clear();
  m_selectionIDs.clear();
  if (m_saveAll) {
    m_selectionIDs = hltData().selectionIDs();      
  } else {
    std::vector<std::string> cromos = m_AselectionNames.value();
    for (std::vector<std::string>::iterator it = cromos.begin();
         it != cromos.end(); ++it) {
      std::string name = *it;
      if (!validHltSelectionName(name))
        error() << " Not valid selection name " << name << endreq;
      else {
        int id = hltSelectionID(name);
        m_selectionIDs.push_back(id);
      }
    }
  }
  
  for (std::vector<int>::iterator it = m_selectionIDs.begin();
       it != m_selectionIDs.end(); ++it) {
    int id = *it;
    std::string selname = hltSelectionName(id);
    info() << " write selection " << selname 
           << " in HltSummary " << endreq;
    m_selectionNames.push_back(selname);  
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltSummaryWriter::execute() {

  debug() << "==> Execute" << endmsg;

  m_summary = new HltSummary();
  put( m_summary, m_hltSummaryLocation );

  // loca = hltSummaryLocation()+"/Configuration";  
  // put(n,loca);

  for (std::vector<int>::iterator it = m_selectionIDs.begin();
       it != m_selectionIDs.end(); ++it) {
    int id = *it;
    if (hltData().hasSelection(id)) {
      if (hltData().selection(id).decision())
        writeSelection(id);
    } else {
      std::string comment = " No selection in HLT " +hltSelectionName(id);
      Warning(comment,1);
    }
  }

  setFilterPassed(true);
  return StatusCode::SUCCESS;
}

void HltSummaryWriter::writeSelection(int id) {
  Hlt::Selection& sel = hltData().selection(id);
  HltSelectionSummary& sum   = m_summary->selectionSummary(id);
  if (sel.classID() == LHCb::Track::classID()) {
    Hlt::TrackSelection& tsel = *(dynamic_cast<Hlt::TrackSelection*>(&sel));
    for (Hlt::TrackSelection::iterator it = tsel.begin(); it != tsel.end();
         ++it) sum.addData(*(*it));
  } else if (sel.classID() == LHCb::RecVertex::classID()) {
    Hlt::VertexSelection& tsel = *(dynamic_cast<Hlt::VertexSelection*>(&sel));
    for (Hlt::VertexSelection::iterator it = tsel.begin(); it != tsel.end();
         ++it) sum.addData(*(*it));
  }
  sum.setDecision(true);


  debug() << " selection  " << hltSelectionName(id) 
          << " [hltdata] decision " << sel.decision() 
          << " candidates " << sel.ncandidates() 
          << " [hltsummary]: decision " << sum.decision() 
          <<  " candidates " << sum.data().size() << endreq;
  
}



//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltSummaryWriter::finalize() {

  StatusCode sc = HltBaseAlg::finalize();
  debug() << " finalize " << sc << endreq;
  return sc;
}


