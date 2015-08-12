
// from Gaudi
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Incident.h"
#include "GaudiAlg/IGenericTool.h"
#include "GaudiKernel/Memory.h"

// Event model
#include "Event/ProcessHeader.h"
#include "Event/ODIN.h"
#include "Event/RecHeader.h"
#include "Event/ProcStatus.h"
#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Event/Track.h"
#include "Event/ProtoParticle.h"

// local
#include "DaVinciInit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DaVinciInit
//
// 2008-03-02 : P. Koppenburg
//-----------------------------------------------------------------------------

//=============================================================================

DaVinciInit::DaVinciInit( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : LbAppInit    ( name , pSvcLocator ),
    m_memoryTool ( NULL               )
{
  declareProperty("PrintEvent", m_print = false, "Print Event and Run Number");
}

//=============================================================================

DaVinciInit::~DaVinciInit() {}

//=============================================================================

StatusCode DaVinciInit::initialize()
{
  const StatusCode sc = LbAppInit::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  // Private tool to plot the memory usage
  m_memoryTool = tool<IGenericTool>( "MemoryTool", "DaVinciMemory", this, true );

  return sc;
}

//=============================================================================

StatusCode DaVinciInit::execute()
{
  const StatusCode sc = LbAppInit::execute(); 
  if ( sc.isFailure() ) return sc; 

  // Plot the memory usage
  m_memoryTool->execute();

  // Get the run and event number from the ODIN bank
  if ( m_print )
  {
    const LHCb::ODIN * odin = getIfExists<LHCb::ODIN> ( LHCb::ODINLocation::Default );
    if ( odin )
    {
      this->printEventRun( odin->eventNumber(), odin->runNumber() );
    }
    else
    {
      Warning("No ODIN bank",StatusCode::SUCCESS,1).ignore();
    }
  }

  return sc;
}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DaVinciInit )

//=============================================================================
