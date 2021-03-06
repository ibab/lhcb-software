
//---------------------------------------------------------------------------
/** @file RichMonitorTemplate.cpp
 *
 *  Implementation file for algorithm class : RichMonitorTemplate
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichMonitorTemplate.h"

//---------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichMonitorTemplate>          s_factory ;
const        IAlgFactory& RichMonitorTemplateFactory = s_factory ;

// Standard constructor, initializes variables
RichMonitorTemplate::RichMonitorTemplate( const std::string& name,
                                          ISvcLocator* pSvcLocator )
  : HistoAlgBase        ( name, pSvcLocator ),
    m_richRecMCTruth    ( NULL ),
    m_ckAngle           ( NULL )
{
  // track selector
  declareProperty( "TrackSelection", m_trSelector.selectedTrackTypes() );
}

// Destructor
RichMonitorTemplate::~RichMonitorTemplate() {};

//  Initialize
StatusCode RichMonitorTemplate::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = HistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth );
  acquireTool( "RichCherenkovAngle",      m_ckAngle     );

  // Configure track selector
  if ( !m_trSelector.configureTrackTypes(msg()) )
    return Error( "Problem configuring track selection" );
  m_trSelector.printTrackSelection( info() );

  return sc;
}

// Main execution
StatusCode RichMonitorTemplate::execute()
{

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Histogramming
  const RichHistoID hid;
  MAX_CKTHETA_RAD;
  MIN_CKTHETA_RAD;

  // Iterate over segments
  for ( RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    RichRecSegment * segment = *iSeg;

    // apply track selection
    if ( !m_trSelector.trackSelected(segment->richRecTrack()) ) continue;

    // MC type
    const Rich::ParticleIDType mcType = m_richRecMCTruth->mcParticleType( segment );
    if ( mcType == Rich::Unknown ) continue;


  }

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichMonitorTemplate::finalize()
{
  // Execute base class method
  return HistoAlgBase::finalize();
}
