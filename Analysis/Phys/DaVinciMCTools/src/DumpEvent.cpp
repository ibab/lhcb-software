// $Id: DumpEvent.cpp,v 1.1 2004-09-14 11:59:07 pkoppenb Exp $
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
#include "DaVinciMCTools/IDebugTool.h"

// local
#include "DumpEvent.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DumpEvent
//
// 21/06/2004 : P. Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<DumpEvent>          s_factory ;
const        IAlgFactory& DumpEventFactory = s_factory ; 


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

  debug() << "==> Initialise" << endreq;

  StatusCode sc = toolSvc()->retrieveTool( m_debug_name, m_debug, this );
  if( sc.isFailure() ) {
    fatal() << "Unable to retrieve Debug tool '"
        << m_debug_name << "'" << endreq;
    return sc;
  }  

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode DumpEvent::execute() {
  
  debug() << "==> Execute" << endreq;

  SmartDataPtr<MCParticles> kmcparts(eventSvc(), MCParticleLocation::Default );
  if( !kmcparts )
  {
    fatal() << "Unable to find MC particles at '"
        << MCParticleLocation::Default << "'" << endreq;
    return StatusCode::FAILURE;
  }

  std::vector<MCParticle*> mcparts(kmcparts->begin(), kmcparts->end());

  info() << "printEventAsTree( mcparts )" << std::flush << endreq;
  m_debug->printEventAsTree( mcparts );

  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode DumpEvent::finalize() {

  debug() << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
