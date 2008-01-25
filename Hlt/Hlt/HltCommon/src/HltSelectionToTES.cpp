// $Id: HltSelectionToTES.cpp,v 1.2 2008-01-25 15:35:57 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "HltSelectionToTES.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltSelectionToTES
//
// 2006-09-01 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltSelectionToTES );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltSelectionToTES::HltSelectionToTES( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : HltBaseAlg ( name , pSvcLocator )
{
  declareProperty("CopyAll",m_copyAll = false);
  declareProperty("Copy", m_AselectionNames);
  declareProperty("m_trackLocation", 
                  m_trackLocation = "Hlt/Selection/Track/");
  declareProperty("m_vertexLocation", 
                  m_vertexLocation = "Hlt/Selection/Vertex/");

}
//=============================================================================
// Destructor
//=============================================================================
HltSelectionToTES::~HltSelectionToTES() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltSelectionToTES::initialize() {
  
  StatusCode sc = HltBaseAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  m_selectionNames.clear();
  m_selectionIDs.clear();
  if (m_copyAll) {
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
    info() << " copy selection " << selname << " into TES "  << endreq;
    m_selectionNames.push_back(selname);  
  }
  
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================

StatusCode HltSelectionToTES::execute() {

  StatusCode sc = StatusCode::SUCCESS;
  
  for (std::vector<int>::iterator it = m_selectionIDs.begin();
       it != m_selectionIDs.end(); ++it) {
    int id = *it;
    if (!hltData().hasSelection(id)) {
      warning() << " no hlt selection to put in TES "
                << hltSelectionName(id) << endreq;
    } else {
      Hlt::Selection& sel = hltData().selection(id);
      std::string selname = hltSelectionName(id);
      if (sel.classID() == LHCb::Track::classID()) {
        LHCb::Tracks* tracks = 0;
        if (sel.decision()) tracks = copy<LHCb::Tracks,LHCb::Track>(id);
        else tracks = new LHCb::Tracks();
        put(tracks,m_trackLocation+selname);
      } else if (sel.classID() == LHCb::RecVertex::classID()) {
        LHCb::RecVertices* vertices = 0;
        if (sel.decision()) vertices = copy<LHCb::RecVertices,LHCb::RecVertex>(id);
        else vertices = new LHCb::RecVertices();
        put(vertices,m_vertexLocation+selname);
      }
    }
  }

  setFilterPassed(true);  
  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltSelectionToTES::finalize() {

  debug() << "==> Finalize" << endmsg;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
