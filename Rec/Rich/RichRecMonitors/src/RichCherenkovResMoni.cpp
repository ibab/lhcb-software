
//---------------------------------------------------------------------------
/** @file RichCherenkovResMoni.cpp
 *
 *  Implementation file for algorithm class : RichCherenkovResMoni
 *
 *  CVS Log :-
 *  $Id: RichCherenkovResMoni.cpp,v 1.15 2010-02-11 19:57:25 jonrob Exp $
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
using namespace Gaudi::Units;

//---------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( CherenkovResMoni );

// Standard constructor, initializes variables
CherenkovResMoni::CherenkovResMoni( const std::string& name,
                                    ISvcLocator* pSvcLocator )
  : HistoAlgBase        ( name, pSvcLocator ),
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
  const StatusCode sc = HistoAlgBase::initialize();
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
  const Rich::HistoID hid;

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
    const double ptot = std::sqrt(segment->trackSegment().bestMomentum().Mag2());

    // CK resolution and angle for true type
    const double expCKres = m_ckAngleRes->ckThetaResolution(segment,mcType);
    const double expCKang = m_ckAngle->avgCherenkovTheta(segment,mcType);

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
        richHisto1D( hid(rad,hypo,"expCKang"), "Expected CK angle",
                     minCkTheta[rad], maxCkTheta[rad], nBins1D() )->fill(ckang);
        richHisto1D( hid(rad,hypo,"ckres"), "Calculated CKres",
                     0, ckResMax[rad], nBins1D() )->fill(ckres);
        richProfile1D( hid(rad,hypo,"ckresVcktheta"), "Calculated CKres V CKangle",
                       minCkTheta[rad], maxCkTheta[rad], nBins1D() )->fill(ckang,ckres);
        richProfile1D( hid(rad,hypo,"ckresVptotp"), "Calculated CKres V ptot",
                       1*GeV, 100*GeV, nBins1D() )->fill( ptot, ckres );
        richProfile2D( hid(rad,hypo,"ckresVptotVckang"), "Calculated CKres V ptot+ckang",
                       1*GeV, 100*GeV, nBins2D(),
                       minCkTheta[rad], maxCkTheta[rad], nBins2D() )->fill(ptot, ckang, ckres);
      }

    } // particle ID codes

      // loop over photons
    const LHCb::RichRecSegment::Photons & photons = photonCreator()->reconstructPhotons( segment );
    for ( LHCb::RichRecSegment::Photons::const_iterator iPhot = photons.begin();
          iPhot != photons.end();
          ++iPhot )
    {
      LHCb::RichRecPhoton * photon = *iPhot;

      // Reco angles
      const double thetaRec  = photon->geomPhoton().CherenkovTheta();
      const double ckExpPull = ( expCKres>0 ? (thetaRec-expCKang)/expCKres : -999 );

      // MC photon
      const LHCb::MCRichOpticalPhoton * mcPhot = m_richRecMCTruth->trueOpticalPhoton(photon);
      if ( mcPhot )
      {
        // MC Cherenkov angles
        const double thetaMC  = mcPhot->cherenkovTheta();
        const double delCK    = thetaRec-thetaMC;
        // diffs and pulls
        const double trueCKerr  = fabs(thetaMC-expCKang);
        const double recoCKerr  = fabs(thetaRec-expCKang);
        const double ckTruePull = ( expCKres>0 ? (thetaRec-thetaMC)/expCKres : -999 );

        richHisto1D( hid(rad,mcType,"recoCKang"), "Reconstructed CK angle | MC true photons",
                     minCkTheta[rad], maxCkTheta[rad], nBins1D() )->fill(thetaRec);
        richHisto1D( hid(rad,mcType,"mcCKang"), "MC CK angle | MC true photons",
                     minCkTheta[rad], maxCkTheta[rad], nBins1D() )->fill(thetaMC);
        richProfile1D( hid(rad,mcType,"calCKresVdelCKtheta"),
                       "Calculated CKres V Rec-True CKtheta | MC true photons",
                       0, ckResMax[rad], nBins1D() )->fill(delCK, expCKres);
        richProfile1D( hid(rad,mcType,"calCKresVtrueCKtheta"),
                       "Rec-True CKtheta V true CK angle | MC true photons",
                       minCkTheta[rad], maxCkTheta[rad], nBins1D() )->fill(expCKang,delCK);

        richHisto1D( hid(rad,mcType,"mcckres"),
                     "fabs(Theta MC - Exp CK theta) | MC true photons",
                     0, ckResMax[rad], nBins1D() )->fill(trueCKerr);

        richProfile1D( hid(rad,mcType,"ckresVmcres"),
                       "fabs(Theta MC - Exp CK theta) V Calculated CKres | MC true photons",
                       0, ckResMax[rad], nBins1D() )->fill(expCKres,trueCKerr);
        richProfile1D( hid(rad,mcType,"diffckVckang"),
                       "fabs(Rec-Exp) CKtheta V CKtheta | MC true photons",
                       minCkTheta[rad], maxCkTheta[rad], nBins1D() )->fill(expCKang,recoCKerr);
        richProfile1D( hid(rad,mcType,"diffckVPtot"),
                       "fabs(Rec-Exp) CKtheta V ptot | MC true photons",
                       1*GeV, 100*GeV, nBins1D() )->fill(ptot,recoCKerr);

        if ( expCKres>0 )
        {
          richHisto1D( hid(rad,mcType,"ckPullMC"), "(Rec-MC)/Res CKtheta | MC true photons",
                       -5, 5, nBins1D() )->fill(ckTruePull);
          richHisto1D( hid(rad,mcType,"ckPullExp"), "(Rec-Exp)/Res CKtheta | MC true photons",
                       -5, 5, nBins1D() )->fill(ckExpPull);


          richProfile1D( hid(rad,mcType,"ckPullMCVt"),
                         "(Rec-MC)/Res CKtheta V CKtheta | MC true photons",
                         minCkTheta[rad], maxCkTheta[rad], nBins1D() )->fill(thetaRec,ckTruePull);
          richProfile1D( hid(rad,mcType,"ckPullExpVt"),
                         "(Rec-Exp)/Res CKtheta V CKtheta | MC true photons",
                         minCkTheta[rad], maxCkTheta[rad], nBins1D() )->fill(thetaRec,ckExpPull);

          richProfile1D( hid(rad,mcType,"ckPullMCVp"),
                         "(Rec-MC)/Res CKtheta V ptot | MC true photons",
                         1*GeV, 100*GeV, nBins1D() )->fill(ptot,ckTruePull);
          richProfile1D( hid(rad,mcType,"ckPullExpVp"),
                         "(Rec-Exp)/Res CKtheta V ptot | MC true photons",
                         1*GeV, 100*GeV, nBins1D() )->fill(ptot,ckExpPull);


          richProfile1D( hid(rad,mcType,"fabsckPullMCVt"),
                         "fabs(Rec-MC)/Res CKtheta V CKtheta | MC true photons",
                         minCkTheta[rad], maxCkTheta[rad], nBins1D() )->fill(thetaRec,fabs(ckTruePull));
          richProfile1D( hid(rad,mcType,"fabsckPullExpVt"),
                         "fabs(Rec-Exp)/Res CKtheta V CKtheta | MC true photons",
                         minCkTheta[rad], maxCkTheta[rad], nBins1D() )->fill(thetaRec,fabs(ckExpPull));

          richProfile1D( hid(rad,mcType,"fabsckPullMCVp"),
                         "fabs(Rec-MC)/Res CKtheta V ptot | MC true photons",
                         1*GeV, 100*GeV, nBins1D() )->fill(ptot,fabs(ckTruePull));
          richProfile1D( hid(rad,mcType,"fabsckPullExpVp"),
                         "fabs(Rec-Exp)/Res CKtheta V ptot | MC true photons",
                         1*GeV, 100*GeV, nBins1D() )->fill(ptot,fabs(ckExpPull));

        }

      }

    }

  }

  return StatusCode::SUCCESS;
}

