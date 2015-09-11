
//-------------------------------------------------------------------------------
/** @file RichRecoQC.cpp
 *
 *  Implementation file for RICH reconstruction monitor : Rich::Rec::MC::RecoQC
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-02
 */
//-------------------------------------------------------------------------------

// local
#include "RichRecoQC.h"

// namespaces
using namespace Rich::Rec::MC;

//-------------------------------------------------------------------------------

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
    m_RichSys           ( NULL ),
    m_minBeta           ( Rich::NRadiatorTypes, 0.9999 ),
    m_maxBeta           ( Rich::NRadiatorTypes, 999.99 ),
    m_truePhotCount     ( Rich::NRadiatorTypes, 0 ),
    m_nSegs             ( Rich::NRadiatorTypes, 0 ),
    m_nEvts             ( 0    )
{
  using namespace boost::assign;

  // Declare job options

  // min max beta
  declareProperty( "MinBeta", m_minBeta );
  declareProperty( "MaxBeta", m_maxBeta );

  // Ch Theta Rec histogram limits: low, high             -> aerogel, Rich1Gas, Rich2Gas
  declareProperty( "ChThetaRecHistoLimitMin", m_ckThetaMin = { 0.150, 0.030, 0.010  } );
  declareProperty( "ChThetaRecHistoLimitMax", m_ckThetaMax = { 0.325, 0.065, 0.036  } );
  declareProperty( "CKResHistoRange",         m_ckResRange = { 0.025, 0.005, 0.0025 } );
  declareProperty( "Radiators", m_rads = { true, true, true } );

  declareProperty( "MinRadSegs", m_minRadSegs = { 0, 0, 0 } );
  declareProperty( "MaxRadSegs", m_maxRadSegs = { 9999999,9999999,9999999 } );

  declareProperty( "EnableAerogelTilePlots", m_aeroTilePlots = false );

  declareProperty( "EnablePerPDPlots",    m_pdResPlots    = false );
  declareProperty( "EnablePerPDColPlots", m_pdColResPlots = false );
  declareProperty( "EnablePerPDFittedResPlots", m_fittedPDResPlots = false );

  declareProperty( "RejectAmbiguousPhotons", m_rejectAmbigPhots = false );

  declareProperty( "HistoFitFreq", m_histFitFreq = -1 );

  setProperty( "NBins2DHistos", 100 );
  setProperty( "HistoPrint", false );
  //setProperty( "OutputLevel", 1 );
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

  // RichDet
  m_RichSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );

  return sc;
}

StatusCode RecoQC::prebookHistograms()
{
  // List of HPDs
  const LHCb::RichSmartID::Vector& hpds = m_RichSys->allPDRichSmartIDs();

  // Number of HPDs
  const unsigned int nHPDsR1 = m_RichSys->nPDs(Rich::Rich1);
  const unsigned int nHPDsR2 = m_RichSys->nPDs(Rich::Rich2);

  // Get aerogel tiles, if active
  std::vector<const DeRichAerogelRadiator*> tiles;
  if ( m_aeroTilePlots && m_rads[Rich::Aerogel] )
  {
    // Aerogel DetElem
    const DeRichMultiSolidRadiator * aerogel
      = getDet<DeRichMultiSolidRadiator>( DeRichLocations::Aerogel );

    // List of active Aerogel tile IDs
    tiles.reserve( aerogel->radiators().size() );
    for ( const auto dRad : aerogel->radiators() )
    {
      const DeRichAerogelRadiator* d = dynamic_cast<const DeRichAerogelRadiator*>(dRad);
      if (!d) return Error( "Failed to cast to DeRichAerogelRadiator" );
      tiles.push_back( d );
    }
  }

  // Loop over radiators
  for ( const auto rad : Rich::radiators() )
  {
    if ( m_rads[rad] )
    {
      // Which RICH ?
      const Rich::DetectorType rich = ( rad == Rich::Rich2Gas ?
                                        Rich::Rich2 : Rich::Rich1 );

      richHisto1D( HID("ckResAllStereoRefit",rad),
                   "Rec-Exp Cktheta | All photons | Stereographic Refit",
                   -m_ckResRange[rad], m_ckResRange[rad], nBins1D(),
                   "delta(Cherenkov theta) / rad" );
      richHisto1D( HID("ckResAllStereoRefitIsolated",rad),
                   "Rec-Exp Cktheta | All photons | Stereographic Refit | Isolated Tracks",
                   -m_ckResRange[rad], m_ckResRange[rad], nBins1D(),
                   "delta(Cherenkov theta) / rad" );
      richHisto1D( HID("thetaRec",rad), "Reconstructed Ch Theta | All photons",
                   m_ckThetaMin[rad], m_ckThetaMax[rad], nBins1D(),
                   "Cherenkov Theta / rad" );
      richHisto1D( HID("thetaExpect",rad), "Expected Ch Theta | All Tracks",
                   m_ckThetaMin[rad], m_ckThetaMax[rad], nBins1D(),
                   "Cherenkov Theta / rad" );
      richHisto1D( HID("phiRec",rad), "Reconstructed Ch Phi | All photons",
                   0.0, 2.0*Gaudi::Units::pi, nBins1D(),
                   "Cherenkov Phi / rad" );
      richHisto1D( HID("ckResAll",rad), "Rec-Exp Cktheta | All photons",
                   -m_ckResRange[rad], m_ckResRange[rad], nBins1D(),
                   "delta(Cherenkov theta) / rad" );
      richHisto1D( HID("thetaRecIsolated",rad),
                   "Reconstructed Ch Theta | All photons | Isolated Tracks",
                   m_ckThetaMin[rad], m_ckThetaMax[rad], nBins1D(),
                   "Cherenkov Theta / rad" );
      richHisto1D( HID("ckResAllIsolated",rad),
                   "Rec-Exp Cktheta | All photons | Isolated Tracks",
                   -m_ckResRange[rad], m_ckResRange[rad], nBins1D(),
                   "delta(Cherenkov theta) / rad" );
      richHisto1D( HID("totalPhotons",rad),"Reconstructed Photon Candidates",
                   0, 10000, 100, "# Photon Candidates" );
      richHisto1D( HID("totalSegments",rad),"Track Radiator Segments",
                   -0.5, 500.5, 501, "# Track Radiator Segments" );
      richHisto1D( HID("totalPhotonsPerSeg",rad),"Photons per Segment | All Tracks",
                   -0.5, 500.5, 501, "# Photon Candidates / Track Segment" );
      richHisto1D( HID("totalPhotonsPerSegIso",rad), "Photon Yield | Isolated Tracks",
                   -0.5, 500.5, 501, "# Photon Candidates / Track Segment" );
      richHisto1D( HID("ckPullIso",rad), "(Rec-Exp)/Res CKtheta | Isolated Tracks",
                   -4, 4, nBins1D(), "Cherenkov Theta pull" );
      richProfile1D( HID("ckPullVthetaIso",rad),
                     "(Rec-Exp)/Res CKtheta Versus CKtheta | Isolated Tracks",
                     m_ckThetaMin[rad], m_ckThetaMax[rad], nBins1D(),
                     "Cherenkov Theta / rad", "Cherenkov Theta pull" );
      richHisto1D( HID("tkMomentum",rad), "Track Segment Momentum",
                   0, 100, nBins1D(), "Track Segment Momentum / GeV" );
      if ( m_aeroTilePlots && rad == Rich::Aerogel )
      {
        // Book a few plots for each aerogel tile
        for ( std::vector<const DeRichAerogelRadiator*>::const_iterator tile = tiles.begin();
              tile != tiles.end(); ++tile )
        {
          std::ostringstream id,title;

          id.str("");
          title.str("");
          id << "tiles/ckResAll-Tile" << (*tile)->primaryTileID();
          title << "Rec-Exp Cktheta | All photons | Tile " << (*tile)->primaryTileID();
          richHisto1D( HID(id.str(),rad), title.str(),
                       -m_ckResRange[rad], m_ckResRange[rad], nBins1D(),
                       "delta(Cherenkov theta) / rad" );

          id.str("");
          title.str("");
          id << "tiles/thetaRec-Tile" << (*tile)->primaryTileID();
          title << "Reconstructed Ch Theta | All photons | Tile " << (*tile)->primaryTileID();
          richHisto1D( HID(id.str(),rad), title.str(),
                       m_ckThetaMin[rad], m_ckThetaMax[rad], nBins1D(),
                       "Cherenkov Theta / rad" );

          if ( (*tile)->subTile() )
          {
            id.str("");
            title.str("");
            id << "subtiles/ckResAll-SubTile" << (*tile)->tileID();
            title << "Rec-Exp Cktheta | All photons | SubTile " << (*tile)->tileID();
            richHisto1D( HID(id.str(),rad), title.str(),
                         -m_ckResRange[rad], m_ckResRange[rad], nBins1D(),
                         "delta(Cherenkov theta) / rad" );

            id.str("");
            title.str("");
            id << "subtiles/thetaRec-SubTile" << (*tile)->tileID();
            title << "Reconstructed Ch Theta | All photons | SubTile " << (*tile)->tileID();
            richHisto1D( HID(id.str(),rad), title.str(),
                         m_ckThetaMin[rad], m_ckThetaMax[rad], nBins1D(),
                         "Cherenkov Theta / rad" );
          }

        }
      }

      // Loop over PDs
      for ( const auto PD : hpds )
      {
        if ( PD.rich() != rich ) continue;
        std::ostringstream title;
        title << "Rec-Exp Cktheta | All photons | " << PD;
        const HID hID(pdResPlotID(PD),rad);
        const PDPlotKey key(rad,PD);
        if ( m_pdResPlots )
        {
          TH1D * h =
            Gaudi::Utils::Aida2ROOT::aida2root( richHisto1D( hID, title.str(),
                                                             -m_ckResRange[rad],
                                                             m_ckResRange[rad],
                                                             nBins1D(),
                                                             "delta(Cherenkov theta) / rad" ) );
          if ( !h ) { return Error( "Failed to book histogram" ); }
          m_pdPlots[key] = h;
        }
        else if ( m_fittedPDResPlots )
        {
          m_pdPlots[key] = new TH1D( (name()+hID.fullid()).c_str(),
                                     (name()+title.str()).c_str(),
                                     nBins1D(),
                                     -m_ckResRange[rad],
                                     m_ckResRange[rad] );
        }
      }

      if ( m_fittedPDResPlots )
      {
        const unsigned int min = ( rad == Rich::Rich2Gas ? nHPDsR1           : 0         );
        const unsigned int max = ( rad == Rich::Rich2Gas ? nHPDsR1+nHPDsR2-1 : nHPDsR1-1 );
        richProfile1D( HID("ckResVPDCopyNum",rad),
                       "CKtheta Resolution Versus PD Copy Number",
                       min-0.5, max+0.5, max-min+1,
                       "PD Copy Number", "Cherenkov Theta Resolution / rad" );
      }

      // Resolutions per PD column
      if ( m_pdColResPlots )
      {
        // Loop over PDs
        for ( const auto PD : hpds )
        {
          if ( PD.rich() != rich ) continue;
          std::ostringstream id,title,col;
          col << PD.rich() << "-" << Rich::text(PD.rich(),PD.panel())
              << "-Col" << PD.pdCol();
          id << "PDCols/" << col.str();
          title << "Rec-Exp Cktheta | All photons | " << col.str();
          richHisto1D( HID(id.str(),rad), title.str(),
                       -m_ckResRange[rad], m_ckResRange[rad], nBins1D(),
                       "delta(Cherenkov theta) / rad" );
        }
      }

    }
  }

  return StatusCode::SUCCESS;
}

// Main execution
StatusCode RecoQC::execute()
{
  // Event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

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

  // Count (selected) segments per radiator
  Rich::Map<Rich::RadiatorType,unsigned int> segsPerRad;
  for ( LHCb::RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    if ( !m_trSelector->trackSelected((*iSeg)->richRecTrack()) ) continue;
    ++segsPerRad[(*iSeg)->trackSegment().radiator()];
  }

  // Count events
  ++m_nEvts;

  // Count photons per radiator
  Rich::Map<Rich::RadiatorType,unsigned int> photsPerRad;

  // Iterate over segments
  if ( msgLevel(MSG::DEBUG) )
  { debug() << "Found " << richSegments()->size() << " segments" << endmsg; }
  for ( LHCb::RichRecSegment * segment : *(richSegments()) )
  {
    // Radiator info
    const Rich::RadiatorType rad = segment->trackSegment().radiator();
    if ( !m_rads[rad] ) continue;

    // # Segment cuts
    if ( segsPerRad[rad] < m_minRadSegs[rad] ||
         segsPerRad[rad] > m_maxRadSegs[rad]  ) continue;

    // track selection
    if ( !m_trSelector->trackSelected(segment->richRecTrack()) ) continue;

    // segment momentum
    const double pTot = std::sqrt(segment->trackSegment().bestMomentum().Mag2());
    richHisto1D( HID("tkMomentum",rad) ) -> fill( pTot / Gaudi::Units::GeV );

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
    for ( const auto* photon : segment->richRecPhotons() )
    {
      // photon sel
      if ( m_rejectAmbigPhots && !photon->geomPhoton().unambiguousPhoton() )
      { continue; }

      // count photons
      ++photsPerRad[rad];

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

      // Aerogel tiles
      if ( m_aeroTilePlots && Rich::Aerogel == rad )
      {
        // get the list of radiator intersections
        const LHCb::RichTrackSegment & tkSeg = segment->trackSegment();
        const Rich::RadIntersection::Vector & intersects = tkSeg.radIntersections();
        for ( auto intersect : intersects )
        {
          const DeRichAerogelRadiator * d =
            dynamic_cast<const DeRichAerogelRadiator*>(intersect.radiator());
          if (!d) return Error( "Failed to cast to DeRichAerogelRadiator" );
          if ( d->subTile() )
          {
            std::ostringstream ida,idb;
            ida << "subtiles/ckResAll-SubTile" << d->tileID();
            richHisto1D(HID(ida.str(),rad))->fill(deltaTheta);
            idb << "subtiles/thetaRec-SubTile" << d->tileID();
            richHisto1D(HID(idb.str(),rad))->fill(thetaRec);
          }
          std::ostringstream idc,idd;
          idc << "tiles/ckResAll-Tile" << d->primaryTileID();
          richHisto1D(HID(idc.str(),rad))->fill(deltaTheta);
          idd << "tiles/thetaRec-Tile" << d->primaryTileID();
          richHisto1D(HID(idd.str(),rad))->fill(thetaRec);
        }
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

      // Plots per PD
      if ( m_pdResPlots || m_fittedPDResPlots )
      {
        const LHCb::RichSmartID hID = photon->geomPhoton().smartID().pdID();
        TH1D * h = m_pdPlots[PDPlotKey(rad,hID)];
        if ( h ) { h -> Fill( deltaTheta ); }
        else
        {
          std::ostringstream mess;
          mess << "Failed to locate PD plot for " << hID;
          Warning( mess.str() ).ignore();
        }
      }

      // Plots per PD column
      if ( m_pdColResPlots )
      {
        const LHCb::RichSmartID hpdID = photon->geomPhoton().smartID().pdID();
        std::ostringstream id,col;
        col << hpdID.rich() << "-" << Rich::text(hpdID.rich(),hpdID.panel())
            << "-Col" << hpdID.pdCol();
        id << "PDCols/" << col.str();
        richHisto1D( HID(id.str(),rad) ) -> fill( deltaTheta );
      }

    } // photon loop

    richHisto1D(HID("totalPhotonsPerSeg",rad))->fill((double)segment->richRecPhotons().size());
    if ( isolated )
    {
      richHisto1D(HID("totalPhotonsPerSegIso",rad))->fill((double)segment->richRecPhotons().size());
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

  // fill total number of photons plots
  for ( const Rich::RadiatorType rad : Rich::radiators() )
  {
    if ( m_rads[rad] )
    {
      richHisto1D(HID("totalPhotons", rad))->fill(photsPerRad[rad]);
      richHisto1D(HID("totalSegments",rad))->fill(segsPerRad[rad]);
    }
  }

  // Fit resolution plots ?
  if ( UNLIKELY ( m_fittedPDResPlots && m_nEvts>0 &&
                  m_histFitFreq>0 && m_nEvts%m_histFitFreq == 0 ) )
  {
    fitResolutionHistos();
  }

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RecoQC::finalize()
{

  // Fit resolution plots ?
  if ( m_fittedPDResPlots ) { fitResolutionHistos(); }

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
    for ( const Rich::RadiatorType rad : Rich::radiators() )
    {
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

  // Clean up temporary histograms
  if ( !m_pdResPlots )
  {
    for ( PDPlotMap::iterator iH = m_pdPlots.begin();
          iH != m_pdPlots.end(); ++iH ) { delete iH->second; }
  }
  m_pdPlots.clear();

  // Execute base class method
  return HistoAlgBase::finalize();
}

//-------------------------------------------------------------------------------

void RecoQC::fitResolutionHistos()
{
  using namespace Gaudi::Utils;

  _ri_debug << "Fitting HPD resolution plots" << endmsg;

  // List of HPDs
  const LHCb::RichSmartID::Vector& hpds = m_RichSys->allPDRichSmartIDs();

  // Loop over radiators
  for ( const Rich::RadiatorType rad : Rich::radiators() )
  {
    if ( !m_rads[rad] ) continue;

    // Which RICH ?
    const Rich::DetectorType rich = ( rad == Rich::Rich2Gas ?
                                      Rich::Rich2 : Rich::Rich1 );

    // Get the profile and reset to refill
    TProfile * p = Aida2ROOT::aida2root( richProfile1D(HID("ckResVPDCopyNum",rad)) );
    if ( p ) { p->Reset(); }
    else
    {
      std::ostringstream mess;
      mess << "Failed to find HPD CK resolution profile for " << rad;
      Warning( mess.str() ).ignore();
      continue;
    }

    // Loop over PDs
    for ( const auto& PD : hpds )
    {
      if ( rich != PD.rich() ) continue;
      
      // Get the resolution plot for this HPD
      TH1D * h = m_pdPlots[PDPlotKey(rad,PD)];
      if ( h )
      {
      
        // fit the histogram
        const FitResult res = fit(h,rad);
        if ( res.OK )
        {
          _ri_debug << "Successfully fitted CK resolution plot for " << PD << endmsg;
          
          // PD Copy Number
          const Rich::DAQ::HPDCopyNumber copyN = m_RichSys->copyNumber( PD );
          
          // Get the profile bin for this HPD and directly set the contents and error
          //const int bin = p->FindBin( copyN.data() );
          //p->SetBinContent( bin, res.resolution );
          //p->SetBinError  ( bin, res.reserror   );
          //p->SetBinEntries( bin, 1              );
          
          // fill the res into the correct bin
          p->Fill( copyN.data(), res.resolution );
        }
        else
        {
          _ri_debug << "FAILED to fit CK resolution plot for " << PD << endmsg;
        }

      }
      else
      {
        std::ostringstream mess;
        mess << "Failed to find HPD resolution plot for " << PD << " " << rad;
        Warning( mess.str() ).ignore();
      }

    }
  }

}

//-------------------------------------------------------------------------------

RecoQC::FitResult RecoQC::fit( TH1D * hist,
                               const Rich::RadiatorType rad  )
{
  using namespace boost::assign;

  // Various hardcoded parameters. Could maybe become JOs ...
  typedef std::vector<double> VD;
  const VD preFitDelta   = list_of(0.010)(0.00105)(0.0025);
  const VD initPreFitRes = list_of(0.010)(0.0015)(0.0007);
  const VD minRes        = list_of(0.005)(0.0008)(0.0004);
  const VD maxRes        = list_of(0.010)(0.0050)(0.0030);
  const VD maxShift      = list_of(0.005)(0.0020)(0.0010);
  const double maxMeanErrorForOK = 1e-3;
  const double maxResErrorForOK  = 5e-4;
  const unsigned int nPolFull = 3;
  const unsigned int minHistEntries = 3000;

  // The final fit result to return
  FitResult res;

  // Skip aerogel ...
  if ( rad == Rich::Aerogel ) return res;

  if ( hist )
  {
    // Min entries
    _ri_verbo << " Found " << hist->GetEntries() << " entries" << endmsg;
    if ( hist->GetEntries() >= minHistEntries )
    {
      // Get x position of max bin
      const double xPeak = hist->GetBinCenter(hist->GetMaximumBin());

      // Fit funcs to clean up at the end
      std::vector<TF1*> trash;

      // Pre fit
      const std::string preFitFType = "gaus";
      _ri_verbo << "Fitting " << preFitFType << endmsg;
      const std::string preFitName  = Rich::text(rad) + "PreFitF";
      TF1 * preFitF = new TF1( preFitName.c_str(), preFitFType.c_str(),
                               xPeak-preFitDelta[rad], xPeak+preFitDelta[rad] );
      trash.push_back(preFitF);
      preFitF->SetParameter(1,0);
      preFitF->SetParameter(2,initPreFitRes[rad]);
      hist->Fit(preFitF,"QRS0");

      TF1 * lastFitF = preFitF;
      for ( unsigned int nPol = 1; nPol <= nPolFull+1; ++nPol )
      {
        // Fit function
        std::ostringstream fFuncType, fitName;
        fFuncType << "gaus(0)+pol" << nPol << "(3)";
        _ri_verbo << "Fitting " << fFuncType.str() << endmsg;
        fitName << rad << "FitF" << nPol;
        TF1 * fFitF = new TF1( fitName.str().c_str(),
                               fFuncType.str().c_str(),
                               -m_ckResRange[rad], m_ckResRange[rad] );
        trash.push_back(fFitF);

        // Set parameter names
        fFitF->SetParName(0,"Gaus Constant");
        fFitF->SetParName(1,"Gaus Mean");
        fFitF->SetParName(2,"Gaus Sigma");

        // Initialise parameters from last fit
        const unsigned int nParamsToSet = ( nPol > 1 ? 3 + nPol : 3 );
        _ri_verbo << "Setting " << nParamsToSet << " parameters from previous fit" << endmsg;
        for ( unsigned int p = 0; p < nParamsToSet; ++p )
        {
          const auto param = lastFitF->GetParameter(p);
          _ri_verbo << " -> Setting parameter " << p << " to " << param << endmsg;
          fFitF->SetParameter(p,param);
        }

        // Run the fit
        hist->Fit(fFitF,"MQRSE0");

        // save last fit
        lastFitF = fFitF;

        // Fit OK ?
        const bool fitOK =
          ( fabs(fFitF->GetParameter(1)) < maxShift[rad]     &&
            fFitF->GetParameter(2)       < maxRes[rad]       &&
            fFitF->GetParameter(2)       > minRes[rad]       &&
            fFitF->GetParError(1)        < maxMeanErrorForOK &&
            fFitF->GetParError(2)        < maxResErrorForOK  );
        if ( fitOK )
        {
          res.OK         = true;
          res.resolution = fFitF->GetParameter(2);
          res.reserror   = fFitF->GetParError(2);
          res.mean       = fFitF->GetParameter(1);
          res.meanerror  = fFitF->GetParError(1);
        }
        else { break; }

      }

      // Clean up
      for ( TF1* h : trash ) { delete h; }

    }

  }

  // return the final fit result
  return res;
}

//-------------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( RecoQC )

//-------------------------------------------------------------------------------
