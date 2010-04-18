
//-------------------------------------------------------------------------------
/** @file RichRecoQC.cpp
 *
 *  Implementation file for RICH reconstruction monitor : Rich::Rec::MC::RecoQC
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
  : HistoAlgBase        ( name, pSvcLocator ),
    m_richPartProp      ( NULL ),
    m_ckAngle           ( NULL ),
    m_ckRes             ( NULL ),
    m_richRecMCTruth    ( NULL ),
    m_trSelector        ( NULL ),
    m_isoTrack          ( NULL ),
    m_fitter            ( NULL ),
    m_minBeta           ( Rich::NRadiatorTypes, 0.9999 ),
    m_maxBeta           ( Rich::NRadiatorTypes, 999.99 ),
    m_truePhotCount     ( Rich::NRadiatorTypes, 0 ),
    m_nSegs             ( Rich::NRadiatorTypes, 0 )
{
  using namespace boost::assign;

  // Declare job options

  // min beta
  declareProperty( "MinBeta", m_minBeta );
  declareProperty( "MaxBeta", m_maxBeta );

  // Ch Theta Rec histogram limits: low, high -> aerogel, C4F10, CF4
  declareProperty( "ChThetaRecHistoLimitMin",
                   m_ckThetaMin = list_of(0.100)(0.030)(0.010) );
  declareProperty( "ChThetaRecHistoLimitMax",
                   m_ckThetaMax = list_of(0.325)(0.065)(0.036) );
  declareProperty( "CKResHistoRange",
                   m_ckResRange = list_of(0.012)(0.006)(0.004) );

  setProperty( "NBins2DHistos", 100 );
}

// Destructor
RecoQC::~RecoQC() { }

// Initialisation
StatusCode RecoQC::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = HistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // get tools
  acquireTool( "RichCherenkovAngle",      m_ckAngle      );
  acquireTool( "TrackSelector",           m_trSelector, this );
  acquireTool( "RichStereoFitter",        m_fitter, this );
  acquireTool( "RichCherenkovResolution", m_ckRes        );
  acquireTool( "RichIsolatedTrack",       m_isoTrack     );
  acquireTool( "RichParticleProperties",  m_richPartProp );

  return sc;
}

StatusCode RecoQC::prebookHistograms()
{
  // Loop over radiators
  for ( Rich::Radiators::const_iterator rad = Rich::radiators().begin();
        rad != Rich::radiators().end(); ++rad )
  {
    richHisto1D( HID("ckResAllStereoRefit",*rad),
                 "Rec-Exp Cktheta | All photons | Stereographic Refit",
                 -m_ckResRange[*rad], m_ckResRange[*rad], nBins1D() );
    richHisto1D( HID("ckResAllStereoRefitIsolated",*rad),
                 "Rec-Exp Cktheta | All photons | Stereographic Refit | Isolated Tracks",
                 -m_ckResRange[*rad], m_ckResRange[*rad], nBins1D() );
    richHisto1D( HID("thetaRec",*rad), "Reconstructed Ch Theta | All photons",
                 m_ckThetaMin[*rad], m_ckThetaMax[*rad], nBins1D() );
    richHisto1D( HID("thetaExpect",*rad), "Expected Ch Theta | All Tracks",
                 m_ckThetaMin[*rad], m_ckThetaMax[*rad], nBins1D() );
    richHisto1D( HID("phiRec",*rad), "Reconstructed Ch Phi | All photons",
                 0.0, 2.0*Gaudi::Units::pi, nBins1D() );
    richHisto1D( HID("ckResAll",*rad), "Rec-Exp Cktheta | All photons",
                 -m_ckResRange[*rad], m_ckResRange[*rad], nBins1D() );
    richHisto1D( HID("thetaRecIsolated",*rad),"Reconstructed Ch Theta | All photons | Isolated Tracks",
                 m_ckThetaMin[*rad], m_ckThetaMax[*rad], nBins1D() );
    richHisto1D( HID("ckResAllIsolated",*rad),"Rec-Exp Cktheta | All photons | Isolated Tracks",
                 -m_ckResRange[*rad], m_ckResRange[*rad], nBins1D() );
    richHisto1D( HID("totalPhotons",*rad),"Photon Yield | All Tracks",
                 -0.5, 50.5, 51 );
    richHisto1D( HID("totalPhotonsIsolated",*rad),"Photon Yield | Isolated Tracks",
                 -0.5, 50.5, 51 );
    richHisto1D( HID("ckPullIso",*rad), "(Rec-Exp)/Res CKtheta | Isolated Tracks",
                 -4, 4, nBins1D() );
    richProfile1D( HID("ckPullVthetaIso",*rad), "(Rec-Exp)/Res CKtheta Versus CKtheta | Isolated Tracks",
                   m_ckThetaMin[*rad], m_ckThetaMax[*rad], nBins1D() );
  }

  return StatusCode::SUCCESS;
}

// Main execution
StatusCode RecoQC::execute()
{

  // Event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Rich Histo ID
  const Rich::HistoID hid;

  // Make sure all tracks and segments have been formed
  if ( trackCreator()->newTracks().isFailure() )
    return Error( "Problem creating RichRecTracks" );

  // Make sure RichRecPixels are ready
  if ( pixelCreator()->newPixels().isFailure() )
    return Error( "Problem creating RichRecPixels" );

  // Make sure photons are available
  if ( photonCreator()->reconstructPhotons().isFailure() )
    return Error( "Problem creating RichRecPhotons" );

  // Is MC available
  const bool mcTrackOK = richRecMCTool()->trackToMCPAvailable();
  const bool mcRICHOK  = richRecMCTool()->pixelMCHistoryAvailable();

  // Iterate over segments
  if ( msgLevel(MSG::DEBUG) )
  { debug() << "Found " << richSegments()->size() << " segments" << endmsg; }
  for ( LHCb::RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    LHCb::RichRecSegment * segment = *iSeg;

    // track selection
    if ( !m_trSelector->trackSelected(segment->richRecTrack()) ) continue;

    // Radiator info
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    // segment momentum
    const double pTot = std::sqrt(segment->trackSegment().bestMomentum().Mag2());

    double thetaExpTrue(0.0), resExpTrue(0.0);
    Rich::ParticleIDType mcType = Rich::Pion; // If MC not available, assume pion
    if ( mcTrackOK )
    {
      // True particle type
      mcType = richRecMCTool()->mcParticleType( segment );
      if ( Rich::Unknown  == mcType ) mcType = Rich::Pion;
      if ( Rich::Electron == mcType ) continue; // skip electrons which are reconstructed badly..
    }
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "   => type = " << mcType << endmsg;

    // Expected Cherenkov theta angle for 'true' particle type
    thetaExpTrue = m_ckAngle->avgCherenkovTheta( segment, mcType );
    richHisto1D( HID("thetaExpect",rad) ) -> fill ( thetaExpTrue );

    // Cherenkov angle resolution for 'true' type
    resExpTrue = m_ckRes->ckThetaResolution( segment, mcType );

    // beta
    const double beta = m_richPartProp->beta( pTot, mcType );
    // selection cuts
    if ( beta < m_minBeta[rad] || beta > m_maxBeta[rad] ) continue;
    //verbose() << "   => Passed beta cut" << endmsg;

    // isolated track ?
    const bool isolated = m_isoTrack->isIsolated( segment, mcType );

    if ( rad != Rich::Aerogel )
    {
      // do the stereographic re-fit
      IStereoFitter::Configuration config(mcType);
      const IStereoFitter::Result fitR = m_fitter->Fit( segment, config );
      // refitted CK theta
      if ( fitR.status == IStereoFitter::Result::Succeeded )
      {
        richHisto1D(HID("ckResAllStereoRefit",rad))->fill(fitR.thcFit-thetaExpTrue);
        if ( isolated )
        {
          richHisto1D(HID("ckResAllStereoRefitIsolated",rad))->fill(fitR.thcFit-thetaExpTrue);
        }
      }
    }

    // loop over photons for this segment
    unsigned int truePhotons(0);
    double avRecTrueTheta(0);
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> Found " << segment->richRecPhotons().size() << " photons" << endmsg;
    for ( LHCb::RichRecSegment::Photons::const_iterator iPhot = segment->richRecPhotons().begin();
          iPhot != segment->richRecPhotons().end(); ++iPhot )
    {
      LHCb::RichRecPhoton * photon = *iPhot;

      // reconstructed theta
      const double thetaRec = photon->geomPhoton().CherenkovTheta();
      // reconstructed phi
      const double phiRec   = photon->geomPhoton().CherenkovPhi();
      // delta theta
      const double deltaTheta = thetaRec-thetaExpTrue;

      richHisto1D(HID("thetaRec",rad))->fill(thetaRec);
      richHisto1D(HID("phiRec",rad))->fill(phiRec);
      richHisto1D(HID("ckResAll",rad))->fill(deltaTheta);

      // isolated segment ?
      if ( isolated )
      {
        richHisto1D(HID("thetaRecIsolated",rad))->fill(thetaRec);
        richHisto1D(HID("ckResAllIsolated",rad))->fill(deltaTheta);
      }

      // CK theta Pull plots
      const double ckPull = ( resExpTrue>0 ? deltaTheta/resExpTrue : -999 );
      if ( isolated && resExpTrue>0 )
      {
        richHisto1D( HID("ckPullIso",rad) ) -> fill( ckPull );
        richProfile1D( HID("ckPullVthetaIso",rad) ) -> fill( thetaRec, ckPull );
      }
      
      // MC based plots
      if ( mcTrackOK && mcRICHOK )
      {
        // Is this a true photon ?
        const LHCb::MCParticle * photonParent = richRecMCTool()->trueCherenkovPhoton( photon );
        if ( photonParent )
        {
          ++truePhotons;
          avRecTrueTheta += thetaRec;
          // resolution plot
          richHisto1D ( HID("ckResTrue",rad), "Rec-Exp Cktheta | MC true photons",
                        -m_ckResRange[rad], m_ckResRange[rad], nBins1D() ) -> fill( deltaTheta );
          if ( resExpTrue>0 )
          {
            // pull plots
            richHisto1D ( HID("ckPullTrueIso",rad), 
                          "(Rec-Exp)/Res Cktheta | MC true photons",
                          -4, 4, nBins1D() ) -> fill( ckPull );
            richProfile1D( HID("ckPullVthetaTrueIso",rad), 
                           "(Rec-Exp)/Res CKtheta Versus CKtheta | MC true photons",
                           m_ckThetaMin[rad], m_ckThetaMax[rad], nBins1D() ) 
              -> fill( thetaRec, ckPull );
          }
        }
        else // fake photon
        {
          richHisto1D( HID("ckResFake",rad), "Rec-Exp Cktheta | MC fake photons",
                       -m_ckResRange[rad], m_ckResRange[rad], nBins1D() ) -> fill( deltaTheta );
        }
      } // MC is available

    } // photon loop

    richHisto1D(HID("totalPhotons",rad))->fill(segment->richRecPhotons().size());
    if ( isolated )
    {
      richHisto1D(HID("totalPhotonsIsolated",rad))->fill(segment->richRecPhotons().size());
    }

    // number of true photons
    if ( truePhotons > 0 )
    {
      richHisto1D( HID("nCKphots",rad), "True # p.e.s",
                   -0.5, 50.5, 51 ) -> fill( truePhotons );
      richProfile1D( HID("nCKphotsVcktheta",rad), "True # p.e.s Versus CK theta",
                     m_ckThetaMin[rad], m_ckThetaMax[rad],
                     nBins1D() ) -> fill( avRecTrueTheta/(double)truePhotons, truePhotons );
      m_truePhotCount[rad] += truePhotons;
      ++m_nSegs[rad];
    }

  } // end loop over segments

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RecoQC::finalize()
{

  if ( m_truePhotCount[Rich::Aerogel]  > 0 ||
       m_truePhotCount[Rich::Rich1Gas] > 0 ||
       m_truePhotCount[Rich::Rich2Gas] > 0  )
  {

    // statistical tool
    const StatDivFunctor occ("%8.2f +-%5.2f");

    info() << "=============================================================================="
           << endmsg;

    // track selection
    info() << " Track Selection : " << m_trSelector->selectedTracks() << endmsg;
    info() << "                 : " << m_maxBeta << " > beta > " << m_minBeta << endmsg;

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
               << occ(m_truePhotCount[rad],m_nSegs[rad]) << " photons/segment" << endmsg;
      }
    }

    info() << "=============================================================================="
           << endmsg;
  }

  // Execute base class method
  return HistoAlgBase::finalize();
}
