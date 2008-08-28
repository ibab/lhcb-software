//-------------------------------------------------------------------------------
/** @file RichRecoQC.cpp
 *
 *  Implementation file for RICH reconstruction monitoring algorithm : Rich::Rec::MC::RecoQC
 *
 *  CVS Log :-
 *  $Id: RichRecoQC.cpp,v 1.42 2008-08-28 22:42:39 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-02
 */
//-------------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichRecoQC.h"

// namespaces
using namespace Rich::Rec::MC;

//-------------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( RecoQC );

// Standard constructor, initializes variables
RecoQC::RecoQC( const std::string& name,
                ISvcLocator* pSvcLocator )
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_richPartProp      ( NULL ),
    m_ckAngle           ( NULL ),
    m_ckRes             ( NULL ),
    m_richRecMCTruth    ( NULL ),
    m_trSelector        ( NULL ),
    m_isoTrack          ( NULL ),
    m_fitter            ( NULL ),
    m_minBeta           ( Rich::NRadiatorTypes, 0.999 ),
    m_truePhotCount     ( Rich::NRadiatorTypes, 0 ),
    m_nSegs             ( Rich::NRadiatorTypes, 0 )
{
  // Declare job options

  // min beta
  declareProperty( "MinBeta", m_minBeta );

  // Ch Theta Rec histogram limits: low, high -> aerogel, C4F10, Cf4
  declareProperty( "ChThetaRecHistoLimitMin",
                   m_ckThetaMin = boost::assign::list_of(0.1)(0.03)(0.01) );
  declareProperty( "ChThetaRecHistoLimitMax",
                   m_ckThetaMax = boost::assign::list_of(0.3)(0.08)(0.05) );
  declareProperty( "CKResHistoRange",
                   m_ckResRange = boost::assign::list_of(0.01)(0.005)(0.0025) );

  declareProperty( "NumberBins", m_nBins = 100 );

}

// Destructor
RecoQC::~RecoQC() { }

// Initialisation
StatusCode RecoQC::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // get tools
  acquireTool( "RichCherenkovAngle", m_ckAngle );
  acquireTool( "TrackSelector", m_trSelector, this );
  acquireTool( "RichStereoFitter",  m_fitter, this );
  acquireTool( "RichCherenkovResolution", m_ckRes );
  acquireTool( "RichIsolatedTrack", m_isoTrack );
  acquireTool( "RichParticleProperties", m_richPartProp );

  return sc;
}

// Main execution
StatusCode RecoQC::execute()
{
  // Event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Rich Histo ID
  const RichHistoID hid;

  // Make sure all tracks and segments have been formed
  if ( trackCreator()->newTracks().isFailure() )
    return Error( "Problem creating RichRecTracks" );

  // make sure RichrecPixels are ready
  if ( pixelCreator()->newPixels().isFailure() )
    return Error( "Problem creating RichRecPixels" );

  // make sure photons are available
  if ( photonCreator()->reconstructPhotons().isFailure() )
    return Error( "Problem creating RichRecPhotons" );

  // Is MC available
  const bool mcTrackOK = richRecMCTool()->trackToMCPAvailable();
  const bool mcRICHOK  = richRecMCTool()->pixelMCHistoryAvailable();

  // Iterate over segments
  debug() << "Found " << richSegments()->size() << " segments" << endreq;
  for ( LHCb::RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    LHCb::RichRecSegment * segment = *iSeg;

    // track selection
    if ( !m_trSelector->trackSelected(segment->richRecTrack()) ) continue;

    // Radiator info
    const Rich::RadiatorType rad = segment->trackSegment().radiator();
    const std::string& RAD = Rich::text(rad);

    // segment momentum
    const double pTot = std::sqrt(segment->trackSegment().bestMomentum().Mag2());

    double thetaExpTrue(0.0), resExpTrue(0.0);
    Rich::ParticleIDType mcType = Rich::Pion; // If MC not available, assume pion
    if ( mcTrackOK )
    {
      // True particle type
      mcType = richRecMCTool()->mcParticleType( segment );
      //if ( Rich::Unknown  == mcType ) continue; // skip tracks with unknown MC type
      if ( Rich::Unknown  == mcType ) mcType = Rich::Pion;
      if ( Rich::Electron == mcType ) continue; // skip electrons which are reconstructed badly..
    }
    verbose() << "   => type = " << mcType << endreq;

    // Expected Cherenkov theta angle for 'true' particle type
    thetaExpTrue = m_ckAngle->avgCherenkovTheta( segment, mcType );

    // Cherenkov angle resolution for 'true' type
    resExpTrue = m_ckRes->ckThetaResolution( segment, mcType );

    // beta
    const double beta = m_richPartProp->beta( pTot, mcType );
    // selection cuts
    if ( beta < m_minBeta[rad] ) continue; // skip non-saturated tracks

    // isolated track ?
    const bool isolated = m_isoTrack->isIsolated( segment, mcType );

    if ( rad != Rich::Aerogel )
    {
      // do the stereographic re-fit
      IStereoFitter::Configuration config(mcType);
      const IStereoFitter::Result fitR = m_fitter->Fit( segment, config );
      // refitted CK theta
      if ( fitR.status == IStereoFitter::Result::Succeeded )
        plot1D( fitR.thcFit-thetaExpTrue,
                hid(rad,"ckResAllStereoRefit"),
                RAD+" Rec-Exp Cktheta : All photons : Stereographic Refit",
                -m_ckResRange[rad], m_ckResRange[rad], m_nBins );
    }

    // loop over photons for this segment 
    unsigned int truePhotons(0);
    double avRecTrueTheta(0);
    verbose() << " -> Found " << segment->richRecPhotons().size() << " photons" << endreq;
    for ( LHCb::RichRecSegment::Photons::const_iterator iPhot = segment->richRecPhotons().begin();
          iPhot != segment->richRecPhotons().end(); ++iPhot )
    {
      LHCb::RichRecPhoton * photon = *iPhot;

      // reconstructed theta
      const double thetaRec = photon->geomPhoton().CherenkovTheta();
      // reconstructed phi
      const double phiRec   = photon->geomPhoton().CherenkovPhi();

      plot1D( thetaRec, hid(rad,"thetaRec"), RAD+" Reconstructed Ch Theta : All photons",
              m_ckThetaMin[rad], m_ckThetaMax[rad], m_nBins );
      plot1D( phiRec, hid(rad,"phiRec"), RAD+" Reconstructed Ch Phi : All photons",
              0.0, 2*Gaudi::Units::pi, m_nBins );
      plot1D( thetaRec-thetaExpTrue,
              hid(rad,"ckResAll"), RAD+" Rec-Exp Cktheta : All photons",
              -m_ckResRange[rad], m_ckResRange[rad], m_nBins );

      // isolated segment ?
      if ( isolated )
      {
        plot1D( thetaRec, hid(rad,"thetaRecIsolated"), 
                RAD+" Reconstructed Ch Theta : All photons : Isolated Tracks",
                m_ckThetaMin[rad], m_ckThetaMax[rad], m_nBins );
        plot1D( thetaRec-thetaExpTrue,
                hid(rad,"ckResAllIsolated"),
                RAD+" Rec-Exp Cktheta : All photons : Isolated Tracks",
                -m_ckResRange[rad], m_ckResRange[rad], m_nBins );
      }

      if ( mcTrackOK && mcRICHOK )
      {
        // Is this a true photon ?
        const LHCb::MCParticle * photonParent = richRecMCTool()->trueCherenkovPhoton( photon );
        if ( photonParent )
        {
          ++truePhotons;
          avRecTrueTheta += thetaRec;
          // resolution plot
          plot1D( thetaRec-thetaExpTrue,
                  hid(rad,"ckResTrue"), RAD+" Rec-Exp Cktheta : MC true photons",
                  -m_ckResRange[rad], m_ckResRange[rad], m_nBins );
          if ( resExpTrue>0 )
          {
            // pull plot
            const double ckPull = (thetaRec-thetaExpTrue)/resExpTrue;
            plot1D( ckPull, hid(rad,"ckPull"), 
                    RAD+" (Rec-Exp)/Res Cktheta", -4, 4, m_nBins );
          }
        }
        else // fake photon
        {
          plot1D( thetaRec-thetaExpTrue,
                  hid(rad,"ckResFake"), 
                  RAD+" Rec-Exp Cktheta : MC fake photons",
                  -m_ckResRange[rad], m_ckResRange[rad], m_nBins );
        }
      } // MC is available

    } // photon loop

    plot1D( segment->richRecPhotons().size(),
            hid(rad,"totalPhotons"),
            RAD+" Photon Yield : All Tracks",
              -0.5, 30.5, 31 );
    if ( isolated )
    {
      plot1D( segment->richRecPhotons().size(),
              hid(rad,"totalPhotonsIsolated"),
              RAD+" Photon Yield : Isolated Tracks",
              -0.5, 30.5, 31 );
    }
    
      // number of true photons
    if ( truePhotons > 0 )
    {
      plot1D( truePhotons, hid(rad,"nCKphots"), 
              RAD+" True # p.e.s", -0.5, 50, 51 );
      profile1D( avRecTrueTheta/(double)truePhotons,
                 truePhotons, hid(rad,"nCKphotsVcktheta"),
                 RAD+" True # p.e.s Versus CK theta",
                 m_ckThetaMin[rad], m_ckThetaMax[rad] );
      m_truePhotCount[rad] += truePhotons;
      ++m_nSegs[rad];
    }

  } // end loop over segments

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RecoQC::finalize()
{

  // statistical tool
  const StatDivFunctor occ("%8.2f +-%5.2f");

  info() << "=============================================================================="
         << endreq;

  // track selection
  info() << " Track Selection : " << m_trSelector->selectedTracks() << endreq;
  info() << "                 : beta > " << m_minBeta << endreq;

  // loop over radiators
  for ( int irad = 0; irad < Rich::NRadiatorTypes; ++irad )
  {
    const Rich::RadiatorType rad = (Rich::RadiatorType)irad;

    // rad name
    std::string radName = Rich::text(rad);
    radName.resize(15,' ');
    // photon count
    if ( m_truePhotCount[rad]>0 )
    {
      info() << " " << radName << " Av. # CK photons = "
             << occ(m_truePhotCount[rad],m_nSegs[rad]) << " photons/segment" << endreq;
    }    
  }

  info() << "=============================================================================="
         << endreq;

  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}
