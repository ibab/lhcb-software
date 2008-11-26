
//---------------------------------------------------------------------------
/** @file RichPhotonTrajectoryMonitor.cpp
 *
 *  Implementation file for algorithm class : RichPhotonTrajectoryMonitor
 *
 *  CVS Log :-
 *  $Id: RichPhotonTrajectoryMonitor.cpp,v 1.1 2008-11-26 13:42:38 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichPhotonTrajectoryMonitor.h"

// From Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// namespace
using namespace Rich::Rec::MC;

//---------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PhotonTrajectoryMonitor );

// Standard constructor, initializes variables
PhotonTrajectoryMonitor::PhotonTrajectoryMonitor( const std::string& name,
                                                  ISvcLocator* pSvcLocator )
  : Rich::Rec::TupleAlgBase ( name, pSvcLocator ),
    m_richRecMCTruth    ( NULL ),
    m_trSelector        ( NULL ) { }

// Destructor
PhotonTrajectoryMonitor::~PhotonTrajectoryMonitor() {}

//  Initialize
StatusCode PhotonTrajectoryMonitor::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::TupleAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth );
  acquireTool( "TrackSelector",      m_trSelector, this );

  return sc;
}

// Main execution
StatusCode PhotonTrajectoryMonitor::execute()
{

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Histogramming
  const Rich::HistoID hid;

  // Iterate over segments
  for ( LHCb::RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    LHCb::RichRecSegment * segment = *iSeg;

    // apply track selection
    if ( !m_trSelector->trackSelected(segment->richRecTrack()) ) continue;

    // Radiator info
    const Rich::RadiatorType rad = segment->trackSegment().radiator();
    
    // loop over photons
    const LHCb::RichRecSegment::Photons & photons = photonCreator()->reconstructPhotons( segment );
    for ( LHCb::RichRecSegment::Photons::const_iterator iPhot = photons.begin();
          iPhot != photons.end(); ++iPhot )
    {

      // Associated MCRichOpticalPhoton
      const LHCb::MCRichOpticalPhoton * mcPhoton = m_richRecMCTruth->trueOpticalPhoton(*iPhot);
      if ( mcPhoton )
      {
        
        const Gaudi::XYZPoint & primMirrPnt   = (*iPhot)->geomPhoton().sphMirReflectionPoint();
        const Gaudi::XYZPoint & secMirrPnt    = (*iPhot)->geomPhoton().flatMirReflectionPoint();
        const Gaudi::XYZPoint & primMirrPntMC = mcPhoton->sphericalMirrorReflectPoint();
        const Gaudi::XYZPoint & secMirrPntMC  = mcPhoton->flatMirrorReflectPoint();

        plot1D( primMirrPnt.x() - primMirrPntMC.x(), hid(rad,"primMirrX"), "Primary Mirror X", -30, 30 );
        plot1D( primMirrPnt.y() - primMirrPntMC.y(), hid(rad,"primMirrY"), "Primary Mirror Y", -30, 30 );
        plot1D( primMirrPnt.z() - primMirrPntMC.z(), hid(rad,"primMirrZ"), "Primary Mirror Z", -10, 10 );

        plot1D( secMirrPnt.x() - secMirrPntMC.x(), hid(rad,"secMirrX"), "Secondary Mirror X", -30, 30 );
        plot1D( secMirrPnt.y() - secMirrPntMC.y(), hid(rad,"secMirrY"), "Secondary Mirror Y", -30, 30 );
        plot1D( secMirrPnt.z() - secMirrPntMC.z(), hid(rad,"secMirrZ"), "Secondary Mirror Z", -10, 10 );

      } // mc photon

    } // loop over segment photons

  } // loop over segments

  return StatusCode::SUCCESS;
}

