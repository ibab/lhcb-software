
//---------------------------------------------------------------------------
/** @file RichCherenkovResMoni.cpp
 *
 *  Implementation file for algorithm class : RichCherenkovResMoni
 *
 *  CVS Log :-
 *  $Id: RichCherenkovResMoni.cpp,v 1.14 2007-10-09 15:35:44 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichCherenkovResMoni.h"

// From Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// namespace
using namespace Rich::Rec::MC;

//---------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( CherenkovResMoni );

// Standard constructor, initializes variables
CherenkovResMoni::CherenkovResMoni( const std::string& name,
                                    ISvcLocator* pSvcLocator )
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_richRecMCTruth    ( NULL ),
    m_ckAngle           ( NULL ),
    m_ckAngleRes        ( NULL ),
    m_trSelector        ( NULL )
{
  // job opts
}

// Destructor
CherenkovResMoni::~CherenkovResMoni() {};

//  Initialize
StatusCode CherenkovResMoni::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth );
  acquireTool( "RichCherenkovAngle",      m_ckAngle     );
  acquireTool( "RichCherenkovResolution", m_ckAngleRes  );
  acquireTool( "TrackSelector",      m_trSelector, this );

  return sc;
}

// Main execution
StatusCode CherenkovResMoni::execute()
{

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Histogramming
  const RichHistoID hid;

  // Histo ranges               Aero   Rich1Gas  Rich2Gas
  //const double ckResRange[] = { 0.015, 0.01,  0.005 };
  const double ckResMax[] =   { 0.011, 0.011, 0.003 };
  MAX_CKTHETA_RAD;
  MIN_CKTHETA_RAD;

  // Iterate over segments
  for ( LHCb::RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    LHCb::RichRecSegment * segment = *iSeg;

    // apply track selection
    if ( !m_trSelector->trackSelected(segment->richRecTrack()) ) continue;

    // MC type
    const Rich::ParticleIDType mcType = m_richRecMCTruth->mcParticleType( segment );
    if ( mcType == Rich::Unknown ) continue;

    // track segment
    const LHCb::RichTrackSegment & trackSeg = segment->trackSegment();

    // radiator
    const Rich::RadiatorType rad = trackSeg.radiator();

    // Segment momentum
    const double ptot = sqrt(segment->trackSegment().bestMomentum().Mag2()) / Gaudi::Units::GeV;

    // CK resolution and angle for true type
    const double trueCKres = m_ckAngleRes->ckThetaResolution(segment,mcType);
    const double trueCKang = m_ckAngle->avgCherenkovTheta(segment,mcType);

    // Loop over all particle codes
    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo )
    {
      const Rich::ParticleIDType hypo = static_cast<Rich::ParticleIDType>(iHypo);

      // CK resolution
      const double ckres = m_ckAngleRes->ckThetaResolution(segment,hypo);
      // CK angle
      const double ckang = m_ckAngle->avgCherenkovTheta(segment,hypo);

      if ( ckang > 0 )
      {
        // histograms
        plot1D( ckang, hid(rad,hypo,"expCKang"), "Expected CK angle",
                minCkTheta[rad], maxCkTheta[rad] );
        plot1D( ckres, hid(rad,hypo,"ckres"), "Calculated CKres", 0, ckResMax[rad] );
        profile1D( ckang, ckres, hid(rad,hypo,"ckresVcktheta"), "Calculated CKres V CKangle",
                   minCkTheta[rad], maxCkTheta[rad] );
        profile1D( ptot, ckres, hid(rad,hypo,"ckresVptotp"), "Calculated CKres V ptot", 0, 100 );
        profile2D( ptot, ckang, ckres, hid(rad,hypo,"ckresVptotVckang"),
                   "Calculated CKres V ptot+ckang", 0, 100, minCkTheta[rad], maxCkTheta[rad] );
      }

    } // particle ID codes

      // loop over photons
    const LHCb::RichRecSegment::Photons & photons = photonCreator()->reconstructPhotons( segment );
    for ( LHCb::RichRecSegment::Photons::const_iterator iPhot = photons.begin();
          iPhot != photons.end();
          ++iPhot )
    {
      LHCb::RichRecPhoton * photon = *iPhot;

      // MC photon
      const LHCb::MCRichOpticalPhoton * mcPhot = m_richRecMCTruth->trueOpticalPhoton(photon);
      if ( mcPhot )
      {
        // Cherenkov angles
        const double thetaRec = photon->geomPhoton().CherenkovTheta();
        const double thetaMC  = mcPhot->cherenkovTheta();
        const double delCK    = thetaRec-thetaMC;

        const double trueCKerr  = fabs(thetaMC-trueCKang);
        const double recoCKerr  = fabs(thetaRec-trueCKang);

        plot1D( thetaRec, hid(rad,mcType,"recoCKang"), "Reconstructed CK angle (true photons)",
                minCkTheta[rad], maxCkTheta[rad] );
        plot1D( thetaMC, hid(rad,mcType,"mcCKang"), "MC CK angle", minCkTheta[rad], maxCkTheta[rad] );
        profile1D( delCK, trueCKres, hid(rad,mcType,"trueVcalCKres"), "True V calculated CKres",
                   0, ckResMax[rad] );
        profile1D( trueCKang, delCK, hid(rad,mcType,"trueCKresVang"), "True CKres V true CK angle",
                   minCkTheta[rad], maxCkTheta[rad] );

        plot1D( trueCKerr, hid(rad,mcType,"mcckres"), "True MC CKres", 0, ckResMax[rad] );
        profile1D( trueCKres, trueCKerr, hid(rad,mcType,"ckresVmcres"), "True MC V Calculated CKres",
                   0, ckResMax[rad] );

        profile1D( trueCKang, recoCKerr, hid(rad,mcType,"diffckVckangP"), "fabs(Rec-Exp) Cktheta V CKangle",
                   minCkTheta[rad], maxCkTheta[rad] );
        profile1D( ptot, recoCKerr, hid(rad,mcType,"diffckVckangP"), "fabs(Rec-Exp) Cktheta V ptot",
                   0, 100 );

        if ( trueCKres>0 )
        {
          // pulls
          const double ckTruePull = (thetaRec-thetaMC)/trueCKres;
          const double ckExpPull  = (thetaRec-trueCKang)/trueCKres;

          plot1D( ckTruePull, hid(rad,mcType,"ckPullMC"),  "(Rec-MC)/Res Cktheta",  -5, 5 );
          plot1D( ckExpPull,  hid(rad,mcType,"ckPullExp"), "(Rec-Exp)/Res Cktheta", -5, 5 );

          profile1D( thetaRec, ckTruePull, hid(rad,mcType,"ckPullMCVt"),
                     "(Rec-MC)/Res Cktheta V theta",  minCkTheta[rad], maxCkTheta[rad], 50 );
          profile1D( thetaRec, ckExpPull, hid(rad,mcType,"ckPullExpVt"),
                     "(Rec-Exp)/Res Cktheta V theta", minCkTheta[rad], maxCkTheta[rad], 50 );
        }

      }
      //else // fake photons
      //{        
      //}

    }

  }

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode CherenkovResMoni::finalize()
{
  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}

