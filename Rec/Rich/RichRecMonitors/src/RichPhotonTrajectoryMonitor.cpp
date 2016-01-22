
//---------------------------------------------------------------------------
/** @file RichPhotonTrajectoryMonitor.cpp
 *
 *  Implementation file for algorithm class : RichPhotonTrajectoryMonitor
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

// Standard constructor, initializes variables
PhotonTrajectoryMonitor::PhotonTrajectoryMonitor( const std::string& name,
                                                  ISvcLocator* pSvcLocator )
  : Rich::Rec::TupleAlgBase ( name, pSvcLocator ) { }

// Destructor
PhotonTrajectoryMonitor::~PhotonTrajectoryMonitor() {}

//  Initialize
StatusCode PhotonTrajectoryMonitor::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::TupleAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool", m_richRecMCTruth );
  acquireTool( "TrackSelector",    m_trSelector, this );

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
  for ( auto * segment : *richSegments() )
  {

    // apply track selection
    if ( !m_trSelector->trackSelected(segment->richRecTrack()) ) continue;

    // Radiator info
    const auto rad = segment->trackSegment().radiator();

    // loop over photons
    const auto & photons = photonCreator()->reconstructPhotons( segment );
    for ( const auto * photon : photons )
    {
      
      // Associated MCRichOpticalPhoton
      const auto * mcPhoton = m_richRecMCTruth->trueOpticalPhoton(photon);
      if ( mcPhoton )
      {

        // Mirror data
        const auto & primMirrPnt   = photon->geomPhoton().sphMirReflectionPoint();
        const auto & secMirrPnt    = photon->geomPhoton().flatMirReflectionPoint();
        const auto & primMirrPntMC = mcPhoton->sphericalMirrorReflectPoint();
        const auto & secMirrPntMC  = mcPhoton->flatMirrorReflectPoint();

        // emission point data
        const auto & emissPtn   = photon->geomPhoton().emissionPoint();
        const auto & mcEmissPtn = mcPhoton->emissionPoint();

        plot1D( primMirrPnt.x() - primMirrPntMC.x(), hid(rad,"primMirrX"), "Primary Mirror X", -30, 30 );
        plot1D( primMirrPnt.y() - primMirrPntMC.y(), hid(rad,"primMirrY"), "Primary Mirror Y", -30, 30 );
        plot1D( primMirrPnt.z() - primMirrPntMC.z(), hid(rad,"primMirrZ"), "Primary Mirror Z", -10, 10 );

        plot1D( secMirrPnt.x() - secMirrPntMC.x(), hid(rad,"secMirrX"), "Secondary Mirror X", -30, 30 );
        plot1D( secMirrPnt.y() - secMirrPntMC.y(), hid(rad,"secMirrY"), "Secondary Mirror Y", -30, 30 );
        plot1D( secMirrPnt.z() - secMirrPntMC.z(), hid(rad,"secMirrZ"), "Secondary Mirror Z", -10, 10 );

        plot1D( emissPtn.x() - mcEmissPtn.x(), hid(rad,"emissPtnX"), "Rec-MC Emission Point X", -100,  100  );
        plot1D( emissPtn.y() - mcEmissPtn.y(), hid(rad,"emissPtnY"), "Rec-MC Emission Point Y", -100,  100  );
        plot1D( emissPtn.z() - mcEmissPtn.z(), hid(rad,"emissPtnZ"), "Rec-MC Emission Point Z", -1000, 1000 );

      } // mc photon

    } // loop over segment photons

  } // loop over segments

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PhotonTrajectoryMonitor )

//---------------------------------------------------------------------------
