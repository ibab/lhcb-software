// $Id: HltSummaryWriter.cpp,v 1.11 2008-06-23 11:22:58 graven Exp $
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

  m_selectionIDs.clear();
  if (m_saveAll) {
    m_selectionIDs = dataSvc().selectionKeys();      
  } else {
    std::vector<std::string> cromos = m_AselectionNames.value();
    for (std::vector<std::string>::iterator it = cromos.begin();
         it != cromos.end(); ++it) {
      stringKey name(*it);
      if (!validHltSelectionName(name)) {
        error() << " Not valid selection name " << name << endreq;
      } else {
        m_selectionIDs.push_back(name);
      }
    }
  }
  
  for (std::vector<stringKey>::iterator it = m_selectionIDs.begin();
       it != m_selectionIDs.end(); ++it) {
    info() << " write selection " << it->str() 
           << " in HltSummary " << endreq;
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltSummaryWriter::execute() {

  debug() << "==> Execute" << endmsg;

  HltSummary *summary = new HltSummary();
  put( summary, m_hltSummaryLocation );

  for (std::vector<stringKey>::iterator it = m_selectionIDs.begin();
       it != m_selectionIDs.end(); ++it) {
    if (dataSvc().hasSelection(*it)) {
      if (dataSvc().selection(*it,this).decision()) writeSelection(*summary,*it);
    } else {
      std::string comment = " No selection in HLT " + it->str();
      Warning(comment,1);
    }
  }

  setFilterPassed(true);
  return StatusCode::SUCCESS;
}

void HltSummaryWriter::writeSelection(HltSummary& summary, const stringKey& id) {

  Hlt::Selection& sel = dataSvc().selection(id,this);
  HltSelectionSummary& sum   = summary.selectionSummary(id.str());
  if (sel.classID() == LHCb::Track::classID()) {
    Hlt::TrackSelection& tsel = dynamic_cast<Hlt::TrackSelection&>(sel);
    for (Hlt::TrackSelection::iterator it = tsel.begin(); it != tsel.end();
         ++it) sum.addData(*(*it));
  } else if (sel.classID() == LHCb::RecVertex::classID()) {
    Hlt::VertexSelection& tsel = dynamic_cast<Hlt::VertexSelection&>(sel);
    for (Hlt::VertexSelection::iterator it = tsel.begin(); it != tsel.end();
         ++it) sum.addData(*(*it));
  }
  sum.setDecision(true);

  debug() << " selection  " << id
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


