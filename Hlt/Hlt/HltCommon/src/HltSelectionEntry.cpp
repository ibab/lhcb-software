// $Id: HltSelectionEntry.cpp,v 1.1 2007-07-27 21:41:00 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/L0CaloCandidate.h"
#include "Event/L0MuonCandidate.h"

// local
#include "HltSelectionEntry.h"
#include "Event/HltNames.h"
#include "HltBase/EParser.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltSelectionEntry
//
// 2006-01-16 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltSelectionEntry );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltSelectionEntry::HltSelectionEntry( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{

  declareProperty("Selections", m_selections);  
}
//=============================================================================
// Destructor
//=============================================================================
HltSelectionEntry::~HltSelectionEntry() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltSelectionEntry::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_selectionNames.clear();
  m_selectionIDs.clear();

  const std::vector< std::string > values = m_selections.value();
  for (std::vector<std::string>::const_iterator it = values.begin();
       it != values.end(); ++it) {
    const std::string& value = *it;
    info() << " selections: " << value << endreq;
    std::vector<std::string> sels = EParser::parse(value,",");
    std::vector<int> ids;
    for (std::vector<std::string>::iterator it2 = sels.begin();
         it2 != sels.end(); ++it2) {
      const std::string sel = *it2;
      int id = HltConfigurationHelper::getID(*m_conf,"SelectionID",sel);      
      info() << " \t selection " << sel << " id " << id << endreq;
      ids.push_back(id);
    }
    m_selectionNames.push_back(sels);
    m_selectionIDs.push_back(ids);
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltSelectionEntry::execute() {
  
  bool ok = false;
  for (std::vector< std::vector<int> >::iterator it = m_selectionIDs.begin();
       it != m_selectionIDs.end(); ++it) {
    std::vector<int> ids = *it;
    bool ok2 = true;
    for (std::vector<int>::iterator it2 = ids.begin(); 
         it2 != ids.end(); ++it2) {
      if (!m_datasummary->hasSelectionSummary(*it2)) ok2 = false;
      else ok2 = ok2 && (m_datasummary->selectionSummary(*it2).decision());
      debug() << " selection " << *it << ok2 << endreq;
    } 
    ok = ok || ok2;
  }
  
  debug() << " decision of selections " << ok << endreq; 
  if (ok) setDecision(true);

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltSelectionEntry::finalize() {

  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
