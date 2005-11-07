
//---------------------------------------------------------------------------
/** @file RichCherenkovAngleMonitor.cpp
 *
 *  Implementation file for algorithm class : RichCherenkovAngleMonitor
 *
 *  CVS Log :-
 *  $Id: RichCherenkovAngleMonitor.cpp,v 1.1 2005-11-07 09:37:19 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichCherenkovAngleMonitor.h"

//---------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichCherenkovAngleMonitor>          s_factory ;
const        IAlgFactory& RichCherenkovAngleMonitorFactory = s_factory ;

// Standard constructor, initializes variables
RichCherenkovAngleMonitor::RichCherenkovAngleMonitor( const std::string& name,
                                                      ISvcLocator* pSvcLocator )
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_richRecMCTruth    ( 0 ),
    m_ckAngle           ( 0 )
{
  // track selector
  declareProperty( "TrackSelection", m_trSelector.selectedTrackTypes() );
}

// Destructor
RichCherenkovAngleMonitor::~RichCherenkovAngleMonitor() {};

//  Initialize
StatusCode RichCherenkovAngleMonitor::initialize()
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
StatusCode RichCherenkovAngleMonitor::execute()
{

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Histogramming
  const RichHistoID hid;
  //            Radiator          Aerogel  C4F10    CF4
  const double ckRange[]      = { 0.015,   0.01,    0.005   };
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

    // Radiator info
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    // Expected Cherenkov theta angle for true particle type
    const double thetaExpTrue = m_ckAngle->avgCherenkovTheta( segment, mcType );

    // Get photons for this segment
    const RichRecSegment::Photons & photons = photonCreator()->reconstructPhotons( segment );
    verbose() << " Found " << photons.size() << " photon candidates" << endreq;

    // loop over photons
    for ( RichRecSegment::Photons::const_iterator iPhot = photons.begin();
          iPhot != photons.end();
          ++iPhot )
    {
      RichRecPhoton * photon = *iPhot;

      // Cherenkov angles
      const double thetaRec = photon->geomPhoton().CherenkovTheta();
      const double phiRec   = photon->geomPhoton().CherenkovPhi();
      const double delTheta = thetaRec-thetaExpTrue;

      plot1D( thetaRec, hid(rad,"ckTheta"), "Reconstructed Cherenkov theta", 
              minCkTheta[rad], maxCkTheta[rad] );
      plot1D( phiRec, hid(rad,"ckPhi"), "Reconstructed Cherenkov phi", 0, 2*M_PI );
      plot1D( delTheta, hid(rad,"ckDiffAll"), "Rec-Exp CK theta all", 
              -ckRange[rad],ckRange[rad]);

      // true CK photon ?
      const MCParticle * photonParent = m_richRecMCTruth->trueCherenkovPhoton(photon);
      if ( photonParent )
      {

        // CK angles for true photons
        plot1D( thetaRec, hid(rad,mcType,"ckThetaTrue"), "Cherenkov theta",
                minCkTheta[rad], maxCkTheta[rad] );
        plot1D( phiRec,   hid(rad,mcType,"ckPhiTrue"), "Cherenkov phi", 0, 2*M_PI );
        plot1D( delTheta, hid(rad,"ckDiffTrue"), "Rec-Exp CK theta all", 
                -ckRange[rad],ckRange[rad]);

        // Hit point associated to photon
        const HepPoint3D & hitPnt = photon->richRecPixel()->globalPosition();

        // theta versus phi plots
        if ( hitPnt.y() < 0 && hitPnt.x() < 0 )
        {
          profile1D( thetaRec, phiRec, hid(rad,"thetaVphiR1"), "CK phi V theta : y<0 x<0",
                     minCkTheta[rad], maxCkTheta[rad] );
        }
        else if ( hitPnt.y() > 0 && hitPnt.x() < 0 )
        {
          profile1D( thetaRec, phiRec, hid(rad,"thetaVphiR2"), "CK phi V theta : y>0 x<0",
                     minCkTheta[rad], maxCkTheta[rad] );
        }
        else if ( hitPnt.y() < 0 && hitPnt.x() > 0 )
        {
          profile1D( thetaRec, phiRec, hid(rad,"thetaVphiR3"), "CK phi V theta : y<0 x>0",
                     minCkTheta[rad], maxCkTheta[rad] );
        }
        else if ( hitPnt.y() > 0 && hitPnt.x() > 0 )
        {
          profile1D( thetaRec, phiRec, hid(rad,"thetaVphiR4"), "CK phi V theta : y>0 x>0",
                     minCkTheta[rad], maxCkTheta[rad] );
        }

        // Associated MCRichOpticalPhoton
        const MCRichOpticalPhoton * mcPhot = m_richRecMCTruth->trueOpticalPhoton(photon);
        if ( mcPhot )
        {
          const double thetaMC       = mcPhot->cherenkovTheta();
          const double delThetaMC    = thetaRec-mcPhot->cherenkovTheta();
          const double delThetaExpMC = thetaExpTrue-mcPhot->cherenkovTheta();

          plot1D( thetaMC, hid(rad,"ckThetaMC"), "MC Cherenkov theta", 
                  minCkTheta[rad], maxCkTheta[rad] );
          plot1D( delThetaMC, hid(rad,"mcckDiffTrue"), "Rec-MC CK theta true", 
                  -ckRange[rad],ckRange[rad]);
          plot1D( delThetaExpMC, hid(rad,"mcExpDiff"), "MC-Exp CK theta true", 
                  -ckRange[rad],ckRange[rad]);
        } // mc photon

      } // true photon

    } // loop over segment photons

  } // loop over segments

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichCherenkovAngleMonitor::finalize()
{
  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}
