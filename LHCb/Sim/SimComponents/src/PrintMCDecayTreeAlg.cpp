// $Id: PrintMCDecayTreeAlg.cpp,v 1.2 2008-04-11 07:34:32 jpalac Exp $
// Include files 
#include <stdlib.h>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from Event
#include "Event/MCParticle.h"

// from MCInterfaces
#include "MCInterfaces/IPrintMCDecayTreeTool.h"

// local
#include "PrintMCDecayTreeAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrintMCDecayTreeAlg
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PrintMCDecayTreeAlg );


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
PrintMCDecayTreeAlg::~PrintMCDecayTreeAlg() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode PrintMCDecayTreeAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialise" << endreq;

  m_printTool = tool<IPrintMCDecayTreeTool>( m_printToolName, this );

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrintMCDecayTreeAlg::execute() {
  
  debug() << "==> Execute" << endreq;

  LHCb::MCParticles* parts = 
    get<LHCb::MCParticles>( m_particleLocation);

  LHCb::MCParticles::iterator iPart;
  for( iPart = parts->begin(); iPart != parts->end(); iPart++ ) {
    if( ((*iPart)->originVertex() == NULL) ||
        ((*iPart)->originVertex()->mother() == NULL) )
      m_printTool->printTree( *iPart, -1 );
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PrintMCDecayTreeAlg::finalize() {

  debug() << "==> Finalize" << endreq;

  return GaudiAlgorithm::finalize();
}

//=============================================================================
