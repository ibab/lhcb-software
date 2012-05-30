
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

DECLARE_ALGORITHM_FACTORY( RecoQC )

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

  // min max beta
  declareProperty( "MinBeta", m_minBeta );
  declareProperty( "MaxBeta", m_maxBeta );

  // Ch Theta Rec histogram limits: low, high -> aerogel, Rich1Gas, Rich2Gas
  declareProperty( "ChThetaRecHistoLimitMin",
                   m_ckThetaMin = list_of(0.150)(0.030)(0.010) );
  declareProperty( "ChThetaRecHistoLimitMax",
                   m_ckThetaMax = list_of(0.325)(0.065)(0.036) );
  declareProperty( "CKResHistoRange",
                   m_ckResRange = list_of(0.025)(0.005)(0.0025) );

  declareProperty( "Radiators", m_rads = list_of(true)(true)(true) );

  declareProperty( "MinRadSegs", m_minRadSegs = list_of (0)       (0)       (0)       );
  declareProperty( "MaxRadSegs", m_maxRadSegs = list_of (9999999) (9999999) (9999999) );

  declareProperty( "EnableAerogelTilePlots", m_aeroTilePlots = false );

  declareProperty( "EnablePerPDPlots",    m_pdResPlots    = false );
  declareProperty( "EnablePerPDColPlots", m_pdColResPlots = true  );

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

  // Get aerogel tiles, if active
  std::vector<const DeRichAerogelRadiator*> tiles;
  if ( m_aeroTilePlots && m_rads[Rich::Aerogel] )
  {
    // Aerogel DetElem
    const DeRichMultiSolidRadiator * aerogel
      = getDet<DeRichMultiSolidRadiator>( DeRichLocations::Aerogel );

    // List of active Aerogel tile IDs
    tiles.reserve( aerogel->radiators().size() );
    for ( DeRichRadiator::Vector::const_iterator dRad = aerogel->radiators().begin();
          dRad != aerogel->radiators().end(); ++dRad )
    {
      const DeRichAerogelRadiator* d = dynamic_cast<const DeRichAerogelRadiator*>(*dRad);
      if (!d) return Error( "Failed to cast to DeRichAerogelRadiator" );
      tiles.push_back( d );
    }
  }

  // Loop over radiators
  for ( Rich::Radiators::const_iterator rad = Rich::radiators().begin();
        rad != Rich::radiators().end(); ++rad )
  {
    if ( m_rads[*rad] )
    {
      richHisto1D( HID("ckResAllStereoRefit",*rad),
                   "Rec-Exp Cktheta | All photons | Stereographic Refit",
                   -m_ckResRange[*rad], m_ckResRange[*rad], nBins1D(),
                   "delta(Cherenkov theta) / rad" );
      richHisto1D( HID("ckResAllStereoRefitIsolated",*rad),
                   "Rec-Exp Cktheta | All photons | Stereographic Refit | Isolated Tracks",
                   -m_ckResRange[*rad], m_ckResRange[*rad], nBins1D(),
                   "delta(Cherenkov theta) / rad" );
      richHisto1D( HID("thetaRec",*rad), "Reconstructed Ch Theta | All photons",
                   m_ckThetaMin[*rad], m_ckThetaMax[*rad], nBins1D(),
                   "Cherenkov Theta / rad" );
      richHisto1D( HID("thetaExpect",*rad), "Expected Ch Theta | All Tracks",
                   m_ckThetaMin[*rad], m_ckThetaMax[*rad], nBins1D(),
                   "Cherenkov Theta / rad" );
      richHisto1D( HID("phiRec",*rad), "Reconstructed Ch Phi | All photons",
                   0.0, 2.0*Gaudi::Units::pi, nBins1D(),
                   "Cherenkov Phi / rad" );
      richHisto1D( HID("ckResAll",*rad), "Rec-Exp Cktheta | All photons",
                   -m_ckResRange[*rad], m_ckResRange[*rad], nBins1D(),
                   "delta(Cherenkov theta) / rad" );
      richHisto1D( HID("thetaRecIsolated",*rad),
                   "Reconstructed Ch Theta | All photons | Isolated Tracks",
                   m_ckThetaMin[*rad], m_ckThetaMax[*rad], nBins1D(),
                   "Cherenkov Theta / rad" );
      richHisto1D( HID("ckResAllIsolated",*rad),
                   "Rec-Exp Cktheta | All photons | Isolated Tracks",
                   -m_ckResRange[*rad], m_ckResRange[*rad], nBins1D(),
                   "delta(Cherenkov theta) / rad" );
      richHisto1D( HID("totalPhotons",*rad),"Reconstructed Photon Candidates",
                   0, 10000, 100, "# Photon Candidates" );
      richHisto1D( HID("totalSegments",*rad),"Track Radiator Segments",
                   -0.5, 500.5, 501, "# Track Radiator Segments" );
      richHisto1D( HID("totalPhotonsPerSeg",*rad),"Photons per Segment | All Tracks",
                   -0.5, 500.5, 501, "# Photon Candidates / Track Segment" );
      richHisto1D( HID("totalPhotonsPerSegIso",*rad), "Photon Yield | Isolated Tracks",
                   -0.5, 500.5, 501, "# Photon Candidates / Track Segment" );
      richHisto1D( HID("ckPullIso",*rad), "(Rec-Exp)/Res CKtheta | Isolated Tracks",
                   -4, 4, nBins1D(), "Cherenkov Theta pull" );
      richProfile1D( HID("ckPullVthetaIso",*rad),
                     "(Rec-Exp)/Res CKtheta Versus CKtheta | Isolated Tracks",
                     m_ckThetaMin[*rad], m_ckThetaMax[*rad], nBins1D(),
                     "Cherenkov Theta / rad", "Cherenkov Theta pull" );
      if ( m_aeroTilePlots && *rad == Rich::Aerogel )
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
          richHisto1D( HID(id.str(),*rad), title.str(),
                       -m_ckResRange[*rad], m_ckResRange[*rad], nBins1D(),
                       "delta(Cherenkov theta) / rad" );

          id.str("");
          title.str("");
          id << "tiles/thetaRec-Tile" << (*tile)->primaryTileID();
          title << "Reconstructed Ch Theta | All photons | Tile " << (*tile)->primaryTileID();
          richHisto1D( HID(id.str(),*rad), title.str(),
                       m_ckThetaMin[*rad], m_ckThetaMax[*rad], nBins1D(),
                       "Cherenkov Theta / rad" );

          if ( (*tile)->subTile() )
          {
            id.str("");
            title.str("");
            id << "subtiles/ckResAll-SubTile" << (*tile)->tileID();
            title << "Rec-Exp Cktheta | All photons | SubTile " << (*tile)->tileID();
            richHisto1D( HID(id.str(),*rad), title.str(),
                         -m_ckResRange[*rad], m_ckResRange[*rad], nBins1D(),
                         "delta(Cherenkov theta) / rad" );

            id.str("");
            title.str("");
            id << "subtiles/thetaRec-SubTile" << (*tile)->tileID();
            title << "Reconstructed Ch Theta | All photons | SubTile " << (*tile)->tileID();
            richHisto1D( HID(id.str(),*rad), title.str(),
                         m_ckThetaMin[*rad], m_ckThetaMax[*rad], nBins1D(),
                         "Cherenkov Theta / rad" );
          }

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

  // Count (selected) segments per radiator
  Rich::Map<Rich::RadiatorType,unsigned int> segsPerRad;
  for ( LHCb::RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    if ( !m_trSelector->trackSelected((*iSeg)->richRecTrack()) ) continue;
    ++segsPerRad[(*iSeg)->trackSegment().radiator()];
  }

  // Count photons per radiator
  Rich::Map<Rich::RadiatorType,unsigned int> photsPerRad;

  // Iterate over segments
  if ( msgLevel(MSG::DEBUG) )
  { debug() << "Found " << richSegments()->size() << " segments" << endmsg; }
  for ( LHCb::RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    LHCb::RichRecSegment * segment = *iSeg;

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
        for ( Rich::RadIntersection::Vector::const_iterator intersect = intersects.begin();
              intersect != intersects.end(); ++intersect )
        {
          const DeRichAerogelRadiator * d =
            dynamic_cast<const DeRichAerogelRadiator*>((*intersect).radiator());
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
      if ( UNLIKELY ( m_pdResPlots ) )
      {
        const LHCb::RichSmartID hpdID = photon->geomPhoton().smartID().pdID();
        const Rich::DAQ::HPDIdentifier hid(hpdID);
        std::ostringstream id,title;
        id << "PDs/pd-" << hid.number();
        title << "Rec-Exp Cktheta | All photons | " << hpdID;
        richHisto1D( HID(id.str(),rad), title.str(), 
                     -m_ckResRange[rad], m_ckResRange[rad], nBins1D(),
                     "delta(Cherenkov theta) / rad" ) -> fill( deltaTheta );
      }
      if ( UNLIKELY ( m_pdColResPlots ) )
      {
        const LHCb::RichSmartID hpdID = photon->geomPhoton().smartID().pdID();
        std::ostringstream id,title,col;
        col << hpdID.rich() << "-" << Rich::text(hpdID.rich(),hpdID.panel())
            << "-Col" << hpdID.pdCol();
        id << "PDCols/" << col.str();
        title << "Rec-Exp Cktheta | All photons | " << col.str();
        richHisto1D( HID(id.str(),rad), title.str(), 
                     -m_ckResRange[rad], m_ckResRange[rad], nBins1D(),
                     "delta(Cherenkov theta) / rad" ) -> fill( deltaTheta );
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
  for ( Rich::Radiators::const_iterator rad = Rich::radiators().begin();
        rad != Rich::radiators().end(); ++rad )
  {
    if ( m_rads[*rad] )
    {
      richHisto1D(HID("totalPhotons", *rad))->fill(photsPerRad[*rad]);
      richHisto1D(HID("totalSegments",*rad))->fill(segsPerRad[*rad]);
    }
  }

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
