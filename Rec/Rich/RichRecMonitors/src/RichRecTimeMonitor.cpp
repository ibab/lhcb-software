
//-----------------------------------------------------------------------------
/** @file RichRecTimeMonitor.cpp
 *
 *  Implementation file for algorithm class : RichRecTimeMonitor
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecTimeMonitor.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( TimeMonitor );

// Standard constructor, initializes variables
TimeMonitor::TimeMonitor( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : HistoAlgBase ( name, pSvcLocator ),
    m_nEvents    ( 0 ),
    m_nPIDs      ( 0 ),
    m_totTime    ( 0 )
{

  // Location of RichPIDs in TES
  declareProperty( "RichPIDLocation", m_PIDLocation = LHCb::RichPIDLocation::Default );

  // Name associated to algorithm(s)
  declareProperty( "TimingName", m_name );

  // List of algorithms
  declareProperty( "Algorithms", m_algNames );

  // Timing boundaries
  declareProperty( "MaxEventTime",       m_maxTime       = 10000 );
  declareProperty( "MaxEventTimePerPID", m_maxTimePerPID = 100   );
  declareProperty( "MaxTracks",          m_maxTracks     = 500   );
  declareProperty( "MaxPixels",          m_maxPixels     = 10000 );
  declareProperty( "MaxPIDs",            m_maxPIDs       = 300   );

}

// Destructor
TimeMonitor::~TimeMonitor() {};

// Main execution
StatusCode TimeMonitor::execute()
{
  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Acquire PID results
  if ( !loadPIDData() ) return StatusCode::SUCCESS;
  const unsigned nPIDs = m_richPIDs.size();

  // Processing time - sum time for selected algorithms
  double time = 0;
  for ( AlgorithmNames::const_iterator name = m_algNames.begin();
        name != m_algNames.end(); ++name )
  {
    time += chronoSvc()->chronoDelta((*name)+":execute",IChronoStatSvc::ELAPSED)/1000;
  }
  const double timePerPID = ( nPIDs>0 ? time/static_cast<double>(nPIDs) : 0 );

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << m_name << " : Time = " << time << " ms for " << nPIDs << " PIDs. "
            << timePerPID << " ms/PID" << endmsg;
  }

  // increment counters
  ++m_nEvents;
  m_totTime += time;
  m_nPIDs   += nPIDs;

  richHisto1D( HID("totTime"), m_name+" total processing time (ms)", 
               0, m_maxTime, nBins1D() )->fill(time);
  richHisto1D( HID("pidTime"), m_name+" processing time per PID (ms)",
               0, m_maxTimePerPID, nBins1D() )->fill(timePerPID);

  richHisto1D( HID("nPIDs"), m_name+" total # PIDs",
               -0.5, m_maxPIDs+0.5, m_maxPIDs+1 )->fill(nPIDs);
  richHisto1D( HID("nPixels"), m_name+" total # Rich Pixels",
               -0.5, m_maxPixels+0.5, m_maxPixels+1 )->fill(richPixels()->size());

  richProfile1D( HID("tottimeVnpids"), m_name+" total processing time (ms) V # RICH PIDs",
                 -0.5, m_maxTracks+0.5, m_maxTracks+1 )->fill(nPIDs,time);
  richProfile1D( HID("tottimeVnpixs"), m_name+" total processing time (ms) V # RICH Pixels",
                 -0.5,m_maxPixels+0.5,m_maxPixels+1 )->fill(richPixels()->size(),time);
  richProfile1D( HID("pidtimeVnpids"), m_name+" processing time per PID (ms) V # RICH PIDs",
                 -0.5,m_maxTracks+0.5,m_maxTracks+1 )->fill(nPIDs, timePerPID);
  richProfile1D( HID("pidtimeVnpixs"), m_name+" processing time per PID (ms) V # RICH Pixels",
                 -0.5,m_maxPixels+0.5,m_maxPixels+1 )->fill(richPixels()->size(),timePerPID);

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode TimeMonitor::finalize()
{
  // Printout timing info
  const double evtTime = ( m_nEvents>0 ? m_totTime/static_cast<double>(m_nEvents) : 0 );
  const double pidTime = ( m_nPIDs>0   ? m_totTime/static_cast<double>(m_nPIDs)   : 0 );
  info() << "Average timing : " << evtTime << " ms/event, "
         << pidTime << " ms/PID" << endmsg;

  // return
  return RichRecHistoAlgBase::finalize();
}

StatusCode TimeMonitor::loadPIDData()
{
  // Load PIDs
  DataObject *pObject;
  if ( eventSvc()->retrieveObject( m_PIDLocation, pObject ) )
  {
    if ( KeyedContainer<LHCb::RichPID, Containers::HashMap> * pids =
         static_cast<KeyedContainer<LHCb::RichPID, Containers::HashMap>*> (pObject) )
    {
      m_richPIDs.erase( m_richPIDs.begin(), m_richPIDs.end() );
      pids->containedObjects( m_richPIDs );
      return StatusCode::SUCCESS;
    }
  }

  // If we get here, things went wrong
  return Warning( "Failed to located RichPIDs at " + m_PIDLocation );
}
