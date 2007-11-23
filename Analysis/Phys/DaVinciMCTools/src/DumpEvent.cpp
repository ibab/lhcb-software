// $Id: DumpEvent.cpp,v 1.4 2007-11-23 23:27:36 gligorov Exp $
// Include files 
#include <stdlib.h>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IToolSvc.h"

// from EventSys
#include "Event/Particle.h"
#include "Event/MCParticle.h"

// from DaVinci
#include "Kernel/IDebugTool.h"

// local
#include "DumpEvent.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DumpEvent
//
// 21/06/2004 : P. Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
//static const  AlgFactory<DumpEvent>          s_factory ;
//const        IAlgFactory& DumpEventFactory = s_factory ; 

DECLARE_ALGORITHM_FACTORY( DumpEvent );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DumpEvent::DumpEvent( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ), m_debug(0)
{
  declareProperty( "DebugTool", m_debug_name = "DebugTool" );
}

//=============================================================================
// Destructor
//=============================================================================
DumpEvent::~DumpEvent() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode DumpEvent::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialise" << endreq;

  m_debug = tool<IDebugTool>( m_debug_name, this );
  if( !m_debug ) {
    fatal() << "Unable to retrieve Debug tool '"
        << m_debug_name << "'" << endreq;
    return StatusCode::FAILURE;
  }  

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode DumpEvent::execute() {
  
  debug() << "==> Execute" << endreq;

  const LHCb::MCParticles* kmcparts = get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default );
  if( !kmcparts ){
    fatal() << "Unable to find MC particles at '"
        << LHCb::MCParticleLocation::Default << "'" << endreq;
    return StatusCode::FAILURE;
  }

  const std::vector<const LHCb::MCParticle*> mcparts(kmcparts->begin(), kmcparts->end());

  info() << "printEventAsTree( mcparts )" << std::flush << endreq;
  m_debug->printEventAsTree( mcparts );

  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode DumpEvent::finalize() {

  debug() << "==> Finalize" << endreq;

  return GaudiAlgorithm::finalize();
}

//=============================================================================
