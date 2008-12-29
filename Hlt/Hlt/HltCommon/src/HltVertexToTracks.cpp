// $Id: HltVertexToTracks.cpp,v 1.7 2008-12-29 12:56:09 graven Exp $
// Include files 
#include <algorithm>
#include "boost/foreach.hpp"

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "HltVertexToTracks.h"
#include "HltBase/ESequences.h"
#include "Event/RecVertex.h"

using namespace LHCb;

namespace {
  template <class INPUT,class OBJECT>
  inline bool extend(INPUT& cont, OBJECT& obj) {
    if (std::find(cont.begin(),cont.end(),obj) != cont.end()) return false;
    cont.push_back(obj);
    return true;
  }
}

//-----------------------------------------------------------------------------
// Implementation file for class : HltVertexToTracks
//
// 2007-08-03 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltVertexToTracks );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltVertexToTracks::HltVertexToTracks( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
  , m_selections(*this)
{
    m_selections.declareProperties();
}
//=============================================================================
// Destructor
//=============================================================================
HltVertexToTracks::~HltVertexToTracks() {
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltVertexToTracks::initialize() {
  debug() << "==> Initialize" << endmsg;
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first

  m_selections.retrieveSelections();
  m_selections.registerSelection();

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltVertexToTracks::execute() {
  debug() << "==> Execute" << endmsg;

  BOOST_FOREACH( LHCb::RecVertex* vertex, *m_selections.input<1>()) {
    SmartRefVector< LHCb::Track >  trks = vertex->tracks();
    if(trks.size()!=2) return StatusCode::FAILURE;
    extend(*m_selections.output(),trks[0]);
    extend(*m_selections.output(),trks[1]);
  }

  return StatusCode::SUCCESS;
}
