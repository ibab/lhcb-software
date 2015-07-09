
/** @file RichAlignmentMonitor.cpp
 *
 *  Implementation file for algorithm class : RichAlignmentMonitor
 *
 *  @author Antonis Papanestis
 *  @date   2004-02-19
 */

// local
#include "RichAlignmentMonitor.h"

// namespaces
using namespace Rich::Rec::MC;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( AlignmentMonitor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AlignmentMonitor::AlignmentMonitor( const std::string& name,
                                    ISvcLocator* pSvcLocator)
: TupleAlgBase        ( name , pSvcLocator ),
  m_radiator          ( Rich::InvalidRadiator ),
  m_deltaThetaHistoRange ( 0 ),
  m_pType             ( Rich::Unknown ),
  m_pTypes            ( 7, 0 ),
  m_trSelector        ( NULL ),
  m_richRecMCTruth    ( NULL ),
  m_richPartProp      ( NULL ),
  m_ckAngle           ( NULL ),
  m_isoTrack          ( NULL ),
  m_plotAllHPDs       ( false     ),
  m_photonsInHPD      ( 220000, 0 )
{
  // Maximum number of tracks
  declareProperty( "MaxRichRecTracks",     m_maxUsedTracks = 200 );
  declareProperty( "DeltaThetaRange",      m_deltaThetaRange = 0.004 );
  declareProperty( "UseMCTruth",           m_useMCTruth     = false );
  declareProperty( "PreBookHistos",        m_preBookHistos );
  declareProperty( "ParticleType",         m_particleType = 2 ); // pion
  declareProperty( "Radiator",             m_radTemp = 2 ); // default is Rich2
  declareProperty( "HistoOutputLevel",     m_histoOutputLevel = 0 );
  declareProperty( "HPDList",              m_HPDList );
  declareProperty( "PhotonLimitPerHPD",    m_photonLimitPerHPD = 100 );
  declareProperty( "VetoedHPDs",           m_hpdVetoList );
}

//=============================================================================
// Destructor
//=============================================================================
AlignmentMonitor::~AlignmentMonitor() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode AlignmentMonitor::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = TupleAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  m_deltaThetaHistoRange = floor((m_deltaThetaRange + 0.0004)*1000)/1000.0;

  acquireTool( "RichCherenkovAngle",   m_ckAngle  );
  // get track selector
  acquireTool( "TrackSelector", m_trSelector, this );
  acquireTool( "RichIsolatedTrack", m_isoTrack     );

  if ( m_useMCTruth )
  {
    acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth );
    acquireTool( "RichParticleProperties", m_richPartProp );
  }

  m_pType = static_cast<Rich::ParticleIDType>(m_particleType);
  _ri_debug << "Fixed particle type:" << m_pType << endmsg;
  m_radiator = static_cast<Rich::RadiatorType>(m_radTemp);
  _ri_debug << "Radiator:" << m_radiator << " " << m_radTemp << endmsg;

  Rich::DetectorType rich;
  if ( m_radiator == Rich::Rich1Gas || m_radiator  == Rich::Aerogel )
    rich = Rich::Rich1;
  else
    rich = Rich::Rich2;

  const std::string RAD = Rich::text(m_radiator);

  // prebook histograms

  m_sideHistos.push_back( richHisto2D(HID("dThetavphiRecSide0"),"dTheta v phi "+ Rich::text( rich, Rich::Side(0) ),
                                      0.0, 2*Gaudi::Units::pi, 20, -m_deltaThetaHistoRange, m_deltaThetaHistoRange,50) );
  m_sideHistos.push_back( richHisto2D(HID("dThetavphiRecSide1"),"dTheta v phi "+ Rich::text( rich, Rich::Side(1) ),
                                      0.0, 2*Gaudi::Units::pi, 20, -m_deltaThetaHistoRange, m_deltaThetaHistoRange,50) );

  if ( m_histoOutputLevel > 0 )
  {
    richHisto1D( HID("Un_Amb"), "Ambigious/Unambigious photons", -0.5, 1.5, 2 );
    richHisto1D( HID("deltaThetaAmb"), "Ch angle error (Ambiguous photons)",
                 -m_deltaThetaHistoRange, m_deltaThetaHistoRange, nBins1D() );
    richHisto1D( HID("deltaThetaUnamb"),"Ch angle error (Unambigous photons)",
                 -m_deltaThetaHistoRange, m_deltaThetaHistoRange, nBins1D() );
    richHisto2D( HID("dThetavphiRecAll"), "dTheta v phi All", 0.0, 2*Gaudi::Units::pi, 20,
                 -m_deltaThetaHistoRange, m_deltaThetaHistoRange, 50 );
  }

  if ( m_histoOutputLevel > 1 )
  {
    richHisto2D( HID("dThetavphiRecIso"), "dTheta v phi Isolated", 0.0, 2*Gaudi::Units::pi, 20,
                 -m_deltaThetaHistoRange, m_deltaThetaHistoRange, 50 );
    m_sideIsolatedHistos.push_back( richHisto2D(HID("dThetavphiRecIsoSide0"),"dTheta v phi isolated "+ Rich::text( rich, Rich::Side(0) ),
                                                0.0, 2*Gaudi::Units::pi, 20, -m_deltaThetaHistoRange, m_deltaThetaHistoRange,50) );
    m_sideIsolatedHistos.push_back( richHisto2D(HID("dThetavphiRecIsoSide1"),"dTheta v phi isolated "+ Rich::text( rich, Rich::Side(1) ),
                                                0.0, 2*Gaudi::Units::pi, 20, -m_deltaThetaHistoRange, m_deltaThetaHistoRange,50) );
    // quarter histos
    m_quarterHistos.push_back( richHisto2D(HID("dThetavphiRecQuart0"),"dTheta v phi "+ Rich::text( rich, Rich::Side(0) )+" Pos",
                                           0.0, 2*Gaudi::Units::pi, 20, -m_deltaThetaHistoRange, m_deltaThetaHistoRange,50) );
    m_quarterHistos.push_back( richHisto2D(HID("dThetavphiRecQuart1"),"dTheta v phi "+ Rich::text( rich, Rich::Side(0) )+" Neg",
                                           0.0, 2*Gaudi::Units::pi, 20, -m_deltaThetaHistoRange, m_deltaThetaHistoRange,50) );
    m_quarterHistos.push_back( richHisto2D(HID("dThetavphiRecQuart2"),"dTheta v phi "+ Rich::text( rich, Rich::Side(1) )+" Pos",
                                           0.0, 2*Gaudi::Units::pi, 20, -m_deltaThetaHistoRange, m_deltaThetaHistoRange,50) );
    m_quarterHistos.push_back( richHisto2D(HID("dThetavphiRecQuart3"),"dTheta v phi "+ Rich::text( rich, Rich::Side(1) )+" Neg",
                                           0.0, 2*Gaudi::Units::pi, 20, -m_deltaThetaHistoRange, m_deltaThetaHistoRange,50) );
  }

  if ( m_histoOutputLevel > 2 )
  {
    // pre book mirror combinations
    /*
      Since the introduction of python-based configuration, elements of a vector
      like
      0000,0103,...
      in the configurable are no longer interpreted as expected, and therefore the
      vector of mirror segment combinations is now a comma-separated vector of
      strings, e.g.
      '0000','0103',...
      That vector is defined in
      /REC/REC_vXrY/Rich/RichRecQC/python/RichRecQC/Alignment.py
      while here it is m_preBookHistos.
      See below a second usage of this vector and further explanations therein.
      Anatoly Solomin 2008-11-01.
    */
    BOOST_FOREACH( std::string strCombi, m_preBookHistos )
    {
      std::string h_id = "dThetavphiRec"+strCombi;
      std::string sph  = strCombi.substr(0,2);
      std::string flat = strCombi.substr(2,2);

      std::ostringstream title;
      title << "Alignment Histogram: Sph " << sph << " flat " << flat << " R" << rich+1;
      richHisto2D( Rich::HistogramID(h_id,m_radiator), title.str(),
                   0.0, 2*Gaudi::Units::pi, 60, -m_deltaThetaHistoRange, // previously used nPhiBin values: 25, 20
                   m_deltaThetaHistoRange, 50 );
      if ( m_useMCTruth )
      {
        // use MC estimate for cherenkov angle
        h_id += "MC";
        title << " MC";
        richHisto2D( Rich::HistogramID(h_id,m_radiator), title.str(),
                     0.0, 2*Gaudi::Units::pi, 60, -m_deltaThetaHistoRange, // previously used nPhiBin values: 20
                     m_deltaThetaHistoRange, 50 );
        title << " TrueP";
        h_id += "TruP";
        richHisto2D( Rich::HistogramID(h_id,m_radiator), title.str(),
                     0.0, 2*Gaudi::Units::pi, 60, -m_deltaThetaHistoRange, // previously used nPhiBin values: 20
                     m_deltaThetaHistoRange, 50 );
      }
      // NOTE: The Histograms have only been pre-booked, not filled! This happens later in this code.
    }
  }

  // use of a single 0, plot all HPDs
  if (m_HPDList.size() == 1)
    if (m_HPDList[0] == 0 )
      m_plotAllHPDs = true;

  _ri_debug << "Finished Initialization" << endmsg;
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode AlignmentMonitor::execute()
{

  // Check Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // If any containers are empty, form them
  if ( richTracks()->empty() ) {
    if ( trackCreator()->newTracks().isFailure() )
    {
      return Error( "Problem Making Tracks" );
    }
    _ri_debug << "No tracks found : Created " << richTracks()->size()
              << " RichRecTracks " << richSegments()->size()
              << " RichRecSegments" << endmsg;
  }
  _ri_debug << " Found " << richTracks()->size() << " tracks" << endmsg;
  if ( (int)richTracks()->size() > m_maxUsedTracks )
  {
    _ri_debug << "Found " << richTracks()->size() << ">"
              << m_maxUsedTracks << " max usable tracks, stopping." << endmsg;
    return StatusCode::SUCCESS;
  }

  if ( richPixels()->empty() ) {
    if ( pixelCreator()->newPixels().isFailure() )
    {
      return Error( "Problem Making Pixels" );
    }
    _ri_debug << "No Pixels found : Created "
              << richPixels()->size() << " RichRecPixels" << endmsg;
  }

  if ( richPhotons()->empty() ) {
    photonCreator()->reconstructPhotons().ignore();
    _ri_debug << "No photons found : Created "
              << richPhotons()->size() << " RichRecPhotons" << endmsg;
  }

  // Iterate over segments
  for ( LHCb::RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    LHCb::RichRecSegment* segment = *iSeg;

    const Rich::DetectorType rich = segment->trackSegment().rich();

    // Radiator info
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    if ( m_radiator != rad ) continue;

    const std::string RAD = Rich::text(rad);

    // track selection
    if ( !m_trSelector->trackSelected(segment->richRecTrack()) ) continue;


    if ( m_histoOutputLevel > 1 ) plot( std::sqrt(segment->trackSegment().bestMomentum().Mag2())/Gaudi::Units::GeV,
                                        "momentum", "Momentum of seleceted tracks /GeV", 0.0, 150.0 );

    double thetaExpTrue(0.0), thetaExpected(0.0);
    if ( m_useMCTruth )
    {
      // Get true beta from true particle type
      const Rich::ParticleIDType mcType = m_richRecMCTruth->mcParticleType( segment );
      _ri_debug << "mcType:" << mcType << endmsg;
      if ( Rich::Unknown == mcType ) continue;
      plot( mcType, "mcType", "MC Particle type", -1.5, 5.5, 7 );
      ++m_pTypes[mcType];

      const double beta =
        m_richPartProp->beta( std::sqrt(segment->trackSegment().bestMomentum().Mag2()), mcType );
      richHisto1D( HID("beta"), "Beta of the track (MC)", 0.9, 1.0, nBins1D() )->fill(beta);

      // Expected Cherenkov theta angle for true particle type
      thetaExpTrue =  m_ckAngle->avgCherenkovTheta( segment, mcType );
    }

    thetaExpected =  m_ckAngle->avgCherenkovTheta( segment, m_pType);

    _ri_debug << " Found " << segment->richRecPhotons().size() << " in this segment" << endmsg;

    for ( LHCb::RichRecSegment::Photons::const_iterator iPhot = segment->richRecPhotons().begin();
          iPhot != segment->richRecPhotons().end(); ++iPhot )
    {
      LHCb::RichRecPhoton* photon = *iPhot;

      // get the geometrical photon
      const LHCb::RichGeomPhoton & gPhoton = photon->geomPhoton();

      // humber readable ID
      const int humID = Rich::DAQ::HPDIdentifier(gPhoton.smartID()).number();

      // Veto'ed HPD ?
      const bool hpdIsVetoed = ( !m_hpdVetoList.empty() &&
                                 std::find( m_hpdVetoList.begin(),
                                            m_hpdVetoList.end(),
                                            humID ) != m_hpdVetoList.end() );

      // Cherenkov angles
      const double thetaRec = gPhoton.CherenkovTheta();
      const double phiRec   = gPhoton.CherenkovPhi();
      if ( !gPhoton.primaryMirror() || !gPhoton.secondaryMirror() )
      {
        Error( "Mirror information not set in photon !" ).ignore();
        continue;
      }
      const MirrorNumber sphMirNum  = gPhoton.primaryMirror()->mirrorNumber();
      const MirrorNumber flatMirNum = gPhoton.secondaryMirror()->mirrorNumber();

      const double delTheta = thetaRec - thetaExpected;
      if ( fabs(delTheta) > m_deltaThetaRange ) continue;

      double delThetaTrue(0.0);
      bool trueParent( false );
      if ( m_useMCTruth )
      {
        delThetaTrue = thetaRec - thetaExpTrue;
        trueParent = ( NULL != m_richRecMCTruth->trueCherenkovPhoton( photon ) );
      }

      const bool unAmbiguousPhoton = photon->geomPhoton().unambiguousPhoton();

      if ( m_histoOutputLevel > 0 )
        richHisto1D(HID("Un_Amb"))->fill( static_cast<int>(unAmbiguousPhoton) );

      if ( m_useMCTruth && trueParent && !hpdIsVetoed )
      {
        richHisto1D( HID("deltaThetaTrueAll"), "Ch angle error MC ALL",
                     -m_deltaThetaHistoRange, m_deltaThetaHistoRange, nBins1D() )->fill(delThetaTrue);

        if ( unAmbiguousPhoton )
        {
          richHisto1D( HID("deltaThetaTrueUnamb"), "Ch angle error MC Unambiguous",
                       -m_deltaThetaHistoRange, m_deltaThetaHistoRange, nBins1D() )->fill(delThetaTrue);
          richHisto1D( HID("deltaThetaUnambTP"), "Ch angle error (Tru parent only) Unamb",
                       -m_deltaThetaHistoRange, m_deltaThetaHistoRange, nBins1D() )->fill(delTheta);
        }
        else
        {
          richHisto1D( HID("deltaThetaTrueAmb"), "Ch angle error MC Ambiguous",
                       -m_deltaThetaHistoRange, m_deltaThetaHistoRange, nBins1D() )->fill(delThetaTrue);
        }
      }

      if ( !hpdIsVetoed )
      {
        if ( !unAmbiguousPhoton )
        {
          if ( m_histoOutputLevel > 0 ) richHisto1D(HID("deltaThetaAmb"))->fill(delTheta);
          continue;
        }
        if ( m_histoOutputLevel > 0 ) richHisto1D(HID("deltaThetaUnamb"))->fill(delTheta);
      }

      int side, side2, quarter;

      if ( rich == Rich::Rich1 )
      {
        if ( m_histoOutputLevel > 1 )
        {
          richHisto1D( HID("sphMirR1"),"Sph Mirror Numbers Rich1",-0.5,3.5,4)->fill(sphMirNum);
          richHisto1D( HID("fltMirR1"),"Flat Mirror Numbers Rich1",-0.5,15.5,16)->fill(flatMirNum);
          richHisto2D( HID("sphMirReflR1"), "Spherical Mirror Refl point Rich1",
                       -700, 700, 100, -800, 800, 100 )
            -> fill(gPhoton.sphMirReflectionPoint().x(),gPhoton.sphMirReflectionPoint().y());
          richHisto2D(HID("flatMirReflR1"), "Flat Mirror Refl point Rich1",
                      -700, 700, 100, -1000, 1000, 100 )
            -> fill(gPhoton.flatMirReflectionPoint().x(),gPhoton.flatMirReflectionPoint().y());
        }
        side  = ( gPhoton.flatMirReflectionPoint().y() > 0.0 ? 0 : 1 );
        side2 = ( gPhoton.flatMirReflectionPoint().x() > 0.0 ? 0 : 1 );
        quarter = side*2+side2;
      }
      else
      {
        if ( m_histoOutputLevel > 1 )
        {
          richHisto1D( HID("sphMirR2"),"Sph Mirror Numbers Rich2",-0.5,55.5,56)->fill(sphMirNum);
          richHisto1D( HID("fltMirR2"),"Flat Mirror Numbers Rich2",-0.5,39.5,40)->fill(flatMirNum);
          richHisto2D( HID("sphMirReflR2"), "Spherical Mirror Refl point Rich2",
                       -1800, 1800, 100, -1500, 1500, 100)
            ->fill(gPhoton.sphMirReflectionPoint().x(),gPhoton.sphMirReflectionPoint().y());
          richHisto2D( HID("flatMirReflR2"), "Flat Mirror Refl point Rich2",
                       -3000, 3000, 100, -1000, 1000, 100)
            ->fill(gPhoton.flatMirReflectionPoint().x(),gPhoton.flatMirReflectionPoint().y());
        }
        side  = ( gPhoton.flatMirReflectionPoint().x() > 0.0 ? 0 : 1 );
        side2 = ( gPhoton.flatMirReflectionPoint().y() > 0.0 ? 0 : 1 );
        quarter = side*2+side2;
      }

      // a separate histogram for each side
      fill( m_sideHistos[side], phiRec, delTheta, 1 );
      if ( m_histoOutputLevel > 0 )
        richHisto2D( HID("dThetavphiRecAll") ) -> fill( phiRec, delTheta );

      if ( m_histoOutputLevel > 1 ) fill( m_quarterHistos[quarter], phiRec, delTheta, 1 );

      // test for isolation
      const bool isolated = m_isoTrack->isIsolated( segment, m_pType );
      if ( isolated && m_histoOutputLevel > 1 )
      {
        richHisto2D( HID("dThetavphiRecIso") ) -> fill( phiRec, delTheta );
        fill( m_sideIsolatedHistos[side], phiRec, delTheta, 1 );
      }


      // for minimal histo output (online) stop here
      if ( m_histoOutputLevel > 2 )
      {
        // now for individual mirror combinations
        std::ostringstream title;
        title << RAD << " Alignment Histogram: Sph " << sphMirNum
              << " flat " << flatMirNum << " R" << (rich+1);

        std::ostringstream h_id;
        h_id << "dThetavphiRec";

        std::ostringstream thisCombiNr; // only the 4-digit combination number (string)
        if ( sphMirNum > 9 )
          thisCombiNr << sphMirNum;
        else
          thisCombiNr << "0" << sphMirNum;
        if ( flatMirNum > 9 )
          thisCombiNr << flatMirNum;
        else
          thisCombiNr << "0" << flatMirNum;

        // depending on options, make plots only for prebooked mirror combimations.
        bool allowMirrorCombi( true );
        if ( m_histoOutputLevel < 5 )
        {
          // search to see if this mirror combination has been prebooked
          if ( m_preBookHistos.empty() ||
               std::find( m_preBookHistos.begin(),
                          m_preBookHistos.end(), thisCombiNr.str()) == m_preBookHistos.end() )
            allowMirrorCombi = false;
        }

        if ( allowMirrorCombi )
        {
          h_id << thisCombiNr.str();
          richHisto2D( HID(h_id.str(),rad), title.str(),
                       0.0, 2*Gaudi::Units::pi, 60, // formerly nPhiBins was 20, not sure if this is important to change here as the number of bins was prebooked, but doing it anyway
                       -m_deltaThetaHistoRange, m_deltaThetaHistoRange, 50 )
            ->fill( phiRec, delTheta );

          if ( m_histoOutputLevel > 3 && isolated )
            richHisto2D( HID(h_id.str()+"Iso",rad), title.str()+" Iso",
                         0.0, 2*Gaudi::Units::pi, 60, // formerly nPhiBins was 20, this one does NOT seem to be pre-booked
                         -m_deltaThetaHistoRange, m_deltaThetaHistoRange, 50 )
              ->fill( phiRec, delTheta );

          if ( m_useMCTruth )
          {
            // use MC estimate for cherenkov angle
            h_id << "MC";
            title << " MC";
            richHisto2D( HID(h_id.str(),rad), title.str(),
                         0.0, 2*Gaudi::Units::pi, 60, // formerly nPhiBins was 20, not sure if this is important to change here as the number of bins was prebooked, but doing it anyway
                         -m_deltaThetaHistoRange, m_deltaThetaHistoRange, 50 )
              ->fill(phiRec, delThetaTrue);
            // test to see if this photon was emitted from this track
            if ( trueParent )
            {
              h_id << "TruP";
              title << " TrueP";
              richHisto2D( HID(h_id.str(),rad), title.str(),
                           0.0, 2*Gaudi::Units::pi, 60, // formerly nPhiBins was 20, not sure if this is important to change here as the number of bins was prebooked, but doing it anyway
                           -m_deltaThetaHistoRange, m_deltaThetaHistoRange, 50 )
                ->fill(phiRec, delThetaTrue);
            }
          }
        }
        // end of filling prebooked and non-prebooked histograms for mirror combinations

        if ( m_histoOutputLevel > 2 )
        {
          const int hpd = ( m_plotAllHPDs ? humID : makePlotForHPD(gPhoton.smartID()) );

          if ( hpd != 0 )
          {
            std::ostringstream hpd_id;
            hpd_id << "HPD_" << hpd;
            richHisto2D( HID("HPDs/"+hpd_id.str()), hpd_id.str(),
                         0.0, 2*Gaudi::Units::pi, 20,
                         -m_deltaThetaHistoRange, m_deltaThetaHistoRange, 50 )
              ->fill(phiRec, delTheta);
          }
        }

        if ( produceNTuples() && (m_photonsInHPD[humID] < m_photonLimitPerHPD) )
        {
          Tuple myTuple = nTuple( "RichAlign", "Rich Alignment" );
          myTuple->column( "thetaRec"  , thetaRec   );
          myTuple->column( "phiRec"    , phiRec     );
          myTuple->column( "deltaTheta", delTheta   );
          myTuple->column( "sphMirror" , sphMirNum  );
          myTuple->column( "secMirror" , flatMirNum );
          myTuple->column( "hpd"       , humID      );
          myTuple->column( "quarter"   , quarter    );
          myTuple->column( "momentum"  , std::sqrt(segment->trackSegment().bestMomentum().Mag2())/Gaudi::Units::GeV );
          myTuple->write();
          m_photonsInHPD[humID]++;
        }

      }
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode AlignmentMonitor::finalize()
{
  if ( m_useMCTruth )
  {
    info() << "Number of pions:" << m_pTypes[Rich::Pion] << "; Kaons:"
           << m_pTypes[Rich::Kaon] << ";";
    if ( m_pTypes[Rich::Kaon] > 0 )
      info() << " ratio:" << m_pTypes[Rich::Pion] / m_pTypes[Rich::Kaon];
    info() << endmsg;
  }

  // Execute base class method
  return TupleAlgBase::finalize();
}

//=========================================================================
//  makePlotForHPD
//=========================================================================
int AlignmentMonitor::makePlotForHPD ( const LHCb::RichSmartID smartID ) const
{
  const int hpd = Rich::DAQ::HPDIdentifier( smartID ).number();
  return ( m_HPDList.empty() || std::find( m_HPDList.begin(),
                                           m_HPDList.end(),
                                           hpd ) == m_HPDList.end() ?
           0 : hpd );
}
