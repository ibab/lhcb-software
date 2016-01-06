// Include files 
#include <stdlib.h>

// from Event
#include "Event/MCParticle.h"

// from MCInterfaces
#include "MCInterfaces/IPrintMCDecayTreeTool.h"

// local
#include "PrintMCDecayTreeAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrintMCDecayTreeAlg
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PrintMCDecayTreeAlg )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrintMCDecayTreeAlg::PrintMCDecayTreeAlg( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : 
  GaudiAlgorithm ( name , pSvcLocator ), m_printTool(0),
  m_printToolName(  "PrintMCDecayTreeTool" ),
  m_particleLocation( LHCb::MCParticleLocation::Default )
{
  declareProperty( "PrintTool", m_printToolName );
  declareProperty( "MCParticleLocation", m_particleLocation );
}

//=============================================================================
// Destructor
//=============================================================================
PrintMCDecayTreeAlg::~PrintMCDecayTreeAlg() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode PrintMCDecayTreeAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialise" << endmsg;

  m_printTool = tool<IPrintMCDecayTreeTool>( m_printToolName, this );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrintMCDecayTreeAlg::execute() {
  
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
    debug() << "==> Execute" << endmsg;
    if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
      verbose() << "Getting MCParticles from " << m_particleLocation << endmsg;
  }

  LHCb::MCParticles* parts = 
    get<LHCb::MCParticles>( m_particleLocation);

  LHCb::MCParticles::iterator iPart;
  for( iPart = parts->begin(); iPart != parts->end(); iPart++ ) {
    if( ((*iPart)->originVertex() == NULL) ||
        ((*iPart)->originVertex()->mother() == NULL) )
      m_printTool->printTree( *iPart, -1 );
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PrintMCDecayTreeAlg::finalize() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();
}

//=============================================================================
