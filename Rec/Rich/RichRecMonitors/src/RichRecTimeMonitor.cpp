
/** @file RichRecTimeMonitor.cpp
 *
 *  Implementation file for algorithm class : RichRecTimeMonitor
 *
 *  CVS Log :-
 *  $Id: RichRecTimeMonitor.cpp,v 1.2 2005-10-13 15:45:45 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */

// local
#include "RichRecTimeMonitor.h"

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichRecTimeMonitor>          s_factory ;
const        IAlgFactory& RichRecTimeMonitorFactory = s_factory ;

// Standard constructor, initializes variables
RichRecTimeMonitor::RichRecTimeMonitor( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : RichRecAlgBase ( name, pSvcLocator ),
    m_nEvents   ( 0 ),
    m_nPIDs     ( 0 ),
    m_totTime   ( 0 )
{

  // Location of RichPIDs in TES
  declareProperty( "RichPIDLocation", m_PIDLocation = RichPIDLocation::Default );

  // Name associated to algorithm(s)
  declareProperty( "TimingName", m_name );

  // List of algorithms
  declareProperty( "Algorithms", m_algNames );

  // histograms
  declareProperty( "NoHistograms", m_noHists = false );
  declareProperty( "MCHistoPath", m_mcHistPth = "RICH/TIMING/MC/" );
  declareProperty( "HistoPath", m_histPth = "RICH/TIMING/" );

  // Timing boundaries
  declareProperty( "MaxEventTime",       m_maxTime       = 3000 );
  declareProperty( "MaxEventTimePerPID", m_maxTimePerPID = 50   );

}

// Destructor
RichRecTimeMonitor::~RichRecTimeMonitor() {};

//  Initialize
StatusCode RichRecTimeMonitor::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Book histograms
  if ( !m_noHists && !bookHistograms() ) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

StatusCode RichRecTimeMonitor::bookHistograms()
{
  std::string title;
  const int maxTracks        = 150;
  const int maxPixels        = 5000;
  const int nBins            = 30;
  int id = 0;

  title = m_name+" total processing time (ms)";
  id = 1;
  m_time = histoSvc()->book(m_histPth,id,title,nBins,0.,m_maxTime);

  title = m_name+" processing time per PID (ms)";
  id = 2;
  m_timePerPID = histoSvc()->book(m_histPth,id,title,nBins,0.,m_maxTimePerPID);

  title = m_name+" total processing time (ms) V #PIDs";
  id = 3;
  m_timeVnPIDs = histoSvc()->book( m_histPth,id,title,
                                   1+maxTracks,0.5,maxTracks+0.5,
                                   nBins,0,m_maxTime );

  title = m_name+" total processing time (ms) V #Pixels";
  id = 4;
  m_timeVnPixels = histoSvc()->book( m_histPth,id,title,
                                     nBins,0.5,maxPixels+0.5,
                                     nBins,0,m_maxTime );

  title = m_name+" processing time per PID (ms) V #PIDs";
  id = 5;
  m_timePerPIDVnPIDs = histoSvc()->book( m_histPth,id,title,
                                         1+maxTracks,0.5,maxTracks+0.5,
                                         nBins,0,m_maxTimePerPID );

  title = m_name+" processing time per PID (ms) V #Pixels";
  id = 6;
  m_timePerPIDVnPixels = histoSvc()->book( m_histPth,id,title,
                                           nBins,0.5,maxPixels+0.5,
                                           nBins,0,m_maxTimePerPID );

  return StatusCode::SUCCESS;
}

// Main execution
StatusCode RichRecTimeMonitor::execute()
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
  const double timePerPID = time/static_cast<double>(nPIDs);
  debug() << m_name << " : Time = " << time << " ms for " << nPIDs << " PIDs. "
          << timePerPID << " ms/PID" << endreq;

  // Time counters
  m_totTime += time;
  ++m_nEvents;
  m_nPIDs += nPIDs;

  // Fill histograms
  if ( !m_noHists )
  {
    m_time->fill( time );
    m_timeVnPIDs->fill( nPIDs, time );
    m_timePerPID->fill( timePerPID );
    m_timeVnPixels->fill( richPixels()->size(), time );
    m_timePerPIDVnPIDs->fill( nPIDs, timePerPID );
    m_timePerPIDVnPixels->fill( richPixels()->size(), timePerPID );
  }

  return StatusCode::SUCCESS;
};

//  Finalize
StatusCode RichRecTimeMonitor::finalize()
{

  // Printout timing info
  const double evtTime = ( m_nEvents>0 ? m_totTime/static_cast<double>(m_nEvents) : 0 );
  const double pidTime = ( m_nPIDs>0   ? m_totTime/static_cast<double>(m_nPIDs)   : 0 );
  info() << "Average timing : " << evtTime << " sec/event, "
         << pidTime << " sec/PID" << endreq;

  return RichRecAlgBase::finalize();
}

StatusCode RichRecTimeMonitor::loadPIDData()
{
  // Load PIDs
  DataObject *pObject;
  if ( eventSvc()->retrieveObject( m_PIDLocation, pObject ) ) {
    if ( KeyedContainer<RichPID, Containers::HashMap> * pids =
         static_cast<KeyedContainer<RichPID, Containers::HashMap>*> (pObject) ) {
      m_richPIDs.erase( m_richPIDs.begin(), m_richPIDs.end() );
      pids->containedObjects( m_richPIDs );
      return StatusCode::SUCCESS;
    }
  }

  // If we get here, things went wrong
  return Warning( "Failed to located RichPIDs at " + m_PIDLocation );
}
