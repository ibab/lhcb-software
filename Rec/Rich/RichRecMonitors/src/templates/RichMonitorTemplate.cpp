
//---------------------------------------------------------------------------
/** @file RichMonitorTemplate.cpp
 *
 *  Implementation file for algorithm class : RichMonitorTemplate
 *
 *  CVS Log :-
 *  $Id: RichMonitorTemplate.cpp,v 1.1 2005-11-07 09:37:20 jonrob Exp $
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
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_richRecMCTruth    ( 0 ),
    m_ckAngle           ( 0 )
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
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth );
  acquireTool( "RichCherenkovAngle",      m_ckAngle     );

  // Configure track selector
  if ( !m_trSelector.configureTrackTypes() )
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
  return RichRecHistoAlgBase::finalize();
}
