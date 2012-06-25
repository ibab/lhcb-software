
// from Gaudi
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Incident.h"
#include "GaudiAlg/IGenericTool.h"
#include "GaudiKernel/Memory.h"

// from EventBase
#include "Event/ProcessHeader.h"

#include "Event/ODIN.h"

// from RecEvent
#include "Event/RecHeader.h"
#include "Event/ProcStatus.h"

#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Event/ProtoParticle.h"
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/CaloHypo.h"

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
    m_memoryTool ( NULL               ),
    m_lastMem    ( 0                  )
{
  declareProperty("PrintEvent", m_print =  false, "Print Event and Run Number");
  declareProperty("Increment", m_increment = 100,
                  "Number of events to measure memory. This is aligned with PrintFreq in DaVinci");
  declareProperty("MemoryPurgeLimit", m_memPurgeLimit = 1000 * 1000 ); // 2GB
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
  const StatusCode sc = LbAppInit::execute(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by LbAppInit

  // Plot the memory usage
  m_memoryTool->execute();

  // Get the run and event number from the ODIN bank
  if ( m_print )
  {
    if ( exist<LHCb::ODIN> ( LHCb::ODINLocation::Default ))
    {
      const LHCb::ODIN * odin = get<LHCb::ODIN> ( LHCb::ODINLocation::Default );
      this->printEventRun( odin->eventNumber(), odin->runNumber() );
    }
    else
    {
      Warning("No ODIN bank",StatusCode::SUCCESS,1);
    }
  }

  ++counter("Events");

  const unsigned long long nev = counter("Events").nEntries();
  const unsigned long long mem = System::virtualMemory();
  if ( msgLevel(MSG::DEBUG) ) 
    debug() << nev << " memory: " << mem << " KB" << endmsg ;

  if ( 0 == m_lastMem )
  {
    m_lastMem = mem;
  }
  else if ( UNLIKELY( 0 == nev%m_increment && m_increment > 0 ) )
  {
    if ( UNLIKELY( m_lastMem != mem ) )
    {
      const long long memDiff = (long long)(mem-m_lastMem);
      info() << "Memory has changed from " << m_lastMem << " to " << mem << " KB"
             << " (" << memDiff << "KB, " << 100.*memDiff/m_lastMem << "%)"
             << " in last " << m_increment << " events" << endmsg ;
    }
//     if ( mem > m_memPurgeLimit )
//     {
//       info() << "Memory exceeds " << m_memPurgeLimit << " KB -> Purging pools" << endmsg;
//       releaseMemoryPools();
//     }
    m_lastMem = mem;
  }

  return sc;
}

//=============================================================================

StatusCode DaVinciInit::finalize()
{
  //releaseMemoryPools();
  return LbAppInit::finalize();
}

//=============================================================================

void DaVinciInit::releaseMemoryPools() const
{
#ifndef GOD_NOALLOC
  using namespace LHCb;
  boost::singleton_pool<Particle,sizeof(Particle)>::release_memory();
  boost::singleton_pool<Vertex,  sizeof(Vertex)  >::release_memory();
#endif
}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DaVinciInit )

//=============================================================================
