// $Id: HltSelectionEntry.cpp,v 1.3 2007-11-20 10:21:21 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/L0CaloCandidate.h"
#include "Event/L0MuonCandidate.h"

// local
#include "HltSelectionEntry.h"
#include "Event/HltNames.h"

#include "boost/algorithm/string/classification.hpp"
#include "boost/algorithm/string/split.hpp"

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
    std::vector<std::string> sels;
    boost::split(sels,value,boost::is_any_of(","));
    std::vector<int> ids;
    for (std::vector<std::string>::iterator it2 = sels.begin();
         it2 != sels.end(); ++it2) {
      int id = HltConfigurationHelper::getID(*m_conf,"SelectionID",*it2);      
      info() << " \t selection " << *it2 << " id " << id << endreq;
      ids.push_back(id);
    }
    m_selectionNames.push_back(value);
    m_selectionIDs.push_back(ids);
    m_scounters.push_back(0);
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltSelectionEntry::execute() {
  
  bool ok = false;
  int isel = 0;
  for (std::vector< std::vector<int> >::iterator it = m_selectionIDs.begin();
       it != m_selectionIDs.end(); ++it,++isel) {
    std::vector<int> ids = *it;
    bool ok2 = true;
    for (std::vector<int>::iterator it2 = ids.begin(); 
         it2 != ids.end(); ++it2) {
      if (!m_datasummary->hasSelectionSummary(*it2)) ok2 = false;
      else ok2 = ok2 && (m_datasummary->selectionSummary(*it2).decision());
      debug() << " selection " << *it << ok2 << endreq;
    } 
    if (ok2) m_scounters[isel]+= 1;
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

  for (size_t i = 0; i < m_scounters.size(); ++i){
    std::string title = m_selectionNames[i];
    infoSubsetEvents(m_scounters[i],m_counterEntries,title);
  }
  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
