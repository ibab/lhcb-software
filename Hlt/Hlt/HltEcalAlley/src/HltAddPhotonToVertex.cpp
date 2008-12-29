// $Id: HltAddPhotonToVertex.cpp,v 1.4 2008-12-29 16:20:59 graven Exp $
// Include files 


// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "boost/foreach.hpp"

// local
#include "HltAddPhotonToVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltAddPhotonToVertex
//
// 2008-07-10 : Mariusz Witek
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltAddPhotonToVertex );

using namespace LHCb;
//using namespace boost::lambda;
// #namespace bl = boost::lambda;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltAddPhotonToVertex::HltAddPhotonToVertex( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
  , m_selections(*this)
{
    m_selections.declareProperties();
}

//=============================================================================
// Initialization
//=============================================================================

StatusCode HltAddPhotonToVertex::initialize() {
  StatusCode sc = HltAlgorithm::initialize();
  m_selections.retrieveSelections();
  m_selections.registerSelection();
  return sc;
};



//=============================================================================
// Main execution
//=============================================================================
StatusCode HltAddPhotonToVertex::execute() {


  if ( msgLevel(MSG::DEBUG) ) debug() << "HltAddPhotonToVertex: Execute" << endmsg;

  RecVertices* overtices = new RecVertices();
  put(overtices,"Hlt/Vertex/"+m_selections.output()->id().str());

  if (( m_selections.input<1>()->empty() || m_selections.input<2>()->empty() )) {
    if ( msgLevel(MSG::DEBUG)  ) debug() << " no tracks or no vertices to run on  " << endreq;
    return StatusCode::SUCCESS;
  } 

  BOOST_FOREACH( const LHCb::RecVertex* vx_in, *m_selections.input<1>() ) {
    BOOST_FOREACH( const LHCb::Track* track, *m_selections.input<2>() ) {
      LHCb::RecVertex* vx_out = new RecVertex( *vx_in );
      vx_out->addToTracks( const_cast<Track*>(track));
      overtices->insert(vx_out);
      m_selections.output()->push_back(vx_out);
    }
  }
  return StatusCode::SUCCESS;  
}

