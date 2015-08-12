
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

// namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( TimeMonitor )

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

  // List of algorithms to time
  declareProperty( "Algorithms", m_algNames );

  // Timing boundaries
  declareProperty( "MaxEventTime",       m_maxTime          = 10000  );
  declareProperty( "MaxEventTimePerPID", m_maxTimePerPID    = 100    );
  declareProperty( "MaxTracks",          m_maxTracks        = 500    );
  declareProperty( "MaxPixels",          m_maxPixels        = 10000  );
  declareProperty( "MaxPhotons",         m_maxPhotons       = 500000 );
  declareProperty( "MaxPhotonsPerPID",   m_maxPhotonsPerPID = 1000   );
  declareProperty( "MaxPIDs",            m_maxPIDs          = 300    );

}

// Destructor
TimeMonitor::~TimeMonitor() {}

// Main execution
StatusCode TimeMonitor::execute()
{
  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Number of RichPIDs
  if ( !loadPIDData() ) return StatusCode::SUCCESS;
  const unsigned nPIDs = m_richPIDs.size();

  // Processing time - sum time for selected algorithms
  double time = 0;
  for ( AlgorithmNames::const_iterator name = m_algNames.begin();
        name != m_algNames.end(); ++name )
  {
    time += chronoSvc()->chronoDelta((*name)+":execute",IChronoStatSvc::ELAPSED)/1000;
  }
  const double timePerPID = ( nPIDs>0 ? time/(double)nPIDs : 0 );

  // # Photons per PID
  const double photsPerPID = ( nPIDs>0 ? (double)richPhotons()->size()/(double)nPIDs : 0 );

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
  richHisto1D( HID("pidTime"), m_name+" processing time / PID (ms)",
               0, m_maxTimePerPID, nBins1D() )->fill(timePerPID);

  richHisto1D( HID("nPIDs"), m_name+" total # PIDs",
               -0.5, m_maxPIDs+0.5, m_maxPIDs+1 )->fill(nPIDs);
  richHisto1D( HID("nPixels"), m_name+" total # Rich Pixels",
               -0.5, m_maxPixels+0.5, m_maxPixels+1 )->fill((double)richPixels()->size());
  richHisto1D( HID("nPhotons"), m_name+" total # Rich Photons",
               0, m_maxPhotons, nBins1D() )->fill((double)richPhotons()->size());
  richHisto1D( HID("nPhotonsPerPID"), m_name+" total # Rich Photons / PID",
               0, m_maxPhotonsPerPID, nBins1D() )->fill(photsPerPID);

  richProfile1D( HID("tottimeVnpids"), m_name+" total processing time (ms) V # RICH PIDs",
                 -0.5, m_maxTracks+0.5, m_maxTracks+1 )->fill(nPIDs,time);
  richProfile1D( HID("tottimeVnpixs"), m_name+" total processing time (ms) V # RICH Pixels",
                 -0.5,m_maxPixels+0.5,m_maxPixels+1 )->fill((double)richPixels()->size(),time);
  richProfile1D( HID("tottimeVnphots"), m_name+" total processing time (ms) V # RICH Photons",
                 0, m_maxPhotons, nBins1D() )->fill((double)richPhotons()->size(),time);
  richProfile1D( HID("tottimeVnphotsPerPID"), m_name+" total processing time (ms) V # RICH Photons / PID",
                 0, m_maxPhotonsPerPID, nBins1D() )->fill(photsPerPID,time);

  richProfile1D( HID("pidtimeVnpids"), m_name+" processing time per PID (ms) V # RICH PIDs",
                 -0.5,m_maxTracks+0.5,m_maxTracks+1 )->fill(nPIDs, timePerPID);
  richProfile1D( HID("pidtimeVnpixs"), m_name+" processing time per PID (ms) V # RICH Pixels",
                 -0.5,m_maxPixels+0.5,m_maxPixels+1 )->fill((double)richPixels()->size(),timePerPID);
  richProfile1D( HID("pidtimeVnphots"), m_name+" processing time per PID (ms) V # RICH Photons",
                 0, m_maxPhotons, nBins1D() )->fill((double)richPhotons()->size(),timePerPID);
  richProfile1D( HID("pidtimeVnphotsPerPID"), m_name+" processing time per PID (ms) V # RICH Photons / PID",
                 0, m_maxPhotonsPerPID, nBins1D() )->fill(photsPerPID,timePerPID);

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode TimeMonitor::finalize()
{
  // Printout timing info
  const long double evtTime = ( m_nEvents>0 ? m_totTime/m_nEvents : 0.0 );
  const long double pidTime = ( m_nPIDs>0   ? m_totTime/m_nPIDs   : 0.0 );
  info() << "Average timing : " << evtTime << " ms/event, "
         << pidTime << " ms/PID" << endmsg;

  // return
  return HistoAlgBase::finalize();
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
