
/** @file RichAlignmentMonitor.cpp
 *
 *  Implementation file for algorithm class : RichAlignmentMonitor
 *
 *  CVS Log :-
 *  $Id: RichAlignmentMonitor.cpp,v 1.15 2010-01-18 16:02:49 papanest Exp $
 *
 *  @author Antonis Papanestis
 *  @date   2004-02-19
 */

// local
#include "RichAlignmentMonitor.h"
// From Gaudi
#include "GaudiKernel/SystemOfUnits.h"
// RichKernel
#include "RichKernel/RichHPDIdentifier.h"

// namespaces
using namespace Rich::Rec::MC;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( AlignmentMonitor );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AlignmentMonitor::AlignmentMonitor( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : RichRecHistoAlgBase ( name , pSvcLocator ),
    m_pTypes            ( 7, 0),
    m_trSelector        ( NULL ),
    m_richRecMCTruth    ( NULL ),
    m_richPartProp      ( NULL ),
    m_ckAngle           ( NULL ),
    m_isoTrack          ( NULL ),
    m_plotAllHPDs       ( false )
{
  // Maximum number of tracks
  declareProperty( "MaxRichRecTracks",    m_maxUsedTracks = 200 );
  declareProperty( "DeltaThetaRange",     m_deltaThetaRange = 0.004 );
  declareProperty( "UseMCTruth",          m_useMCTruth     = false );
  declareProperty( "PreBookHistos",       m_preBookHistos );
  declareProperty( "ParticleType",        m_particleType = 2 ); // pion
  declareProperty( "RichDetector",        m_richTemp = 1 ); // default is Rich2
  declareProperty( "MinimalHistoOutput",  m_minimalHistoOutput = true );
  declareProperty( "OnlyPrebookedMirrors",m_onlyPrebookedMirrors = true );
  declareProperty( "UseOnlyIsolatedTracks",m_useOnlyIsolatedTracks = true );
  declareProperty( "HPDList",             m_HPDList );
}

//=============================================================================
// Destructor
//=============================================================================
AlignmentMonitor::~AlignmentMonitor() {};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode AlignmentMonitor::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  m_deltaThetaHistoRange = floor((m_deltaThetaRange + 0.0014)*1000)/1000.0;

  acquireTool( "RichCherenkovAngle",   m_ckAngle  );
  // get track selector
  acquireTool( "TrackSelector", m_trSelector, this );
  acquireTool( "RichIsolatedTrack", m_isoTrack     );

  if ( m_useMCTruth ) {
    acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth );
    acquireTool( "RichParticleProperties", m_richPartProp );
  }

  m_pType = static_cast<Rich::ParticleIDType>(m_particleType);
  debug() << "Fixed particle type:" << m_pType << endmsg;
  m_rich = static_cast<Rich::DetectorType>(m_richTemp);
  debug() << "Detector:" << m_rich << endmsg;

  Rich::RadiatorType rad;
  if ( m_rich == Rich::Rich1 )
    rad = Rich::Rich1Gas;
  else
    rad = Rich::Rich2Gas;

  const std::string RAD = Rich::text(rad);

  // Rich Histo ID
  const RichHistoID hid;

  // prebook histograms
  book1D( "Un_Amb", "Ambigious/Unambigious photons", -0.5, 1.5, 2 );
  book1D( "deltaThetaAmb","Ch angle error (Ambiguous photons)", -m_deltaThetaHistoRange, m_deltaThetaHistoRange);
  book1D( "deltaThetaUnamb","Ch angle error (Unambigous photons)", -m_deltaThetaHistoRange, m_deltaThetaHistoRange);
  book2D( "dThetavphiRecAll", "dTheta v phi All", 0.0, 2*Gaudi::Units::pi, 20,
          -m_deltaThetaHistoRange, m_deltaThetaHistoRange, 50);
  book2D( "dThetavphiRecIso", "dTheta v phi Isolated", 0.0, 2*Gaudi::Units::pi, 20,
          -m_deltaThetaHistoRange, m_deltaThetaHistoRange, 50);

  m_sideHistos.push_back( book2D("dThetavphiRecSide0","dTheta v phi "+ Rich::text( m_rich, Rich::Side(0) ),
                                 0.0, 2*Gaudi::Units::pi, 20, -m_deltaThetaHistoRange, m_deltaThetaHistoRange,50) );
  m_sideHistos.push_back( book2D("dThetavphiRecSide1","dTheta v phi "+ Rich::text( m_rich, Rich::Side(1) ),
                                 0.0, 2*Gaudi::Units::pi, 20, -m_deltaThetaHistoRange, m_deltaThetaHistoRange,50) );

  m_sideIsolatedHistos.push_back( book2D("dThetavphiRecIsoSide0","dTheta v phi isolated "+ Rich::text( m_rich, Rich::Side(0) ),
                                         0.0, 2*Gaudi::Units::pi, 20, -m_deltaThetaHistoRange, m_deltaThetaHistoRange,50) );
  m_sideIsolatedHistos.push_back( book2D("dThetavphiRecIsoSide1","dTheta v phi isolated "+ Rich::text( m_rich, Rich::Side(1) ),
                                         0.0, 2*Gaudi::Units::pi, 20, -m_deltaThetaHistoRange, m_deltaThetaHistoRange,50) );
  // quarter histos
  m_quarterHistos.push_back( book2D("dThetavphiRecQuart0","dTheta v phi "+ Rich::text( m_rich, Rich::Side(0) )+" Pos",
                                    0.0, 2*Gaudi::Units::pi, 20, -m_deltaThetaHistoRange, m_deltaThetaHistoRange,50) );
  m_quarterHistos.push_back( book2D("dThetavphiRecQuart1","dTheta v phi "+ Rich::text( m_rich, Rich::Side(0) )+" Neg",
                                    0.0, 2*Gaudi::Units::pi, 20, -m_deltaThetaHistoRange, m_deltaThetaHistoRange,50) );
  m_quarterHistos.push_back( book2D("dThetavphiRecQuart2","dTheta v phi "+ Rich::text( m_rich, Rich::Side(1) )+" Pos",
                                    0.0, 2*Gaudi::Units::pi, 20, -m_deltaThetaHistoRange, m_deltaThetaHistoRange,50) );
  m_quarterHistos.push_back( book2D("dThetavphiRecQuart3","dTheta v phi "+ Rich::text( m_rich, Rich::Side(1) )+" Neg",
                                    0.0, 2*Gaudi::Units::pi, 20, -m_deltaThetaHistoRange, m_deltaThetaHistoRange,50) );
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
  if ( !m_minimalHistoOutput )
  {
    BOOST_FOREACH( std::string strCombi, m_preBookHistos ) {
      std::string h_id = "dThetavphiRec"+strCombi;
      std::string sph  = strCombi.substr(0,2);
      std::string flat = strCombi.substr(2,2);
      std::string title = "Alignment Histogram: Sph "+sph+" flat "+flat+" R"+(boost::lexical_cast<std::string>(m_rich+1));
      book2D( hid(rad, h_id), title, 0.0, 2*Gaudi::Units::pi, 20, -m_deltaThetaHistoRange,
              m_deltaThetaHistoRange, 50 );
      if ( m_useMCTruth ) {
        // use MC estimate for cherenkov angle
        h_id += "MC";
        title += " MC";
        book2D( hid(rad, h_id), title, 0.0, 2*Gaudi::Units::pi, 20, -m_deltaThetaHistoRange,
                m_deltaThetaHistoRange, 50 );
        title += " TrueP";
        h_id += "TruP";
        book2D( hid(rad, h_id), title, 0.0, 2*Gaudi::Units::pi, 20, -m_deltaThetaHistoRange,
                m_deltaThetaHistoRange, 50 );
      }
    }
  }

  // use of a single 0, plot all HPDs
  if (m_HPDList.size() == 1)
    if (m_HPDList[0] == 0 )
      m_plotAllHPDs = true;

  debug() << "Finished Initialization" << endmsg;
  return sc;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode AlignmentMonitor::execute() {

  debug() << "Execute" << endmsg;

  // Check Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Rich Histo ID
  const RichHistoID hid;

  // If any containers are empty, form them
  if ( richTracks()->empty() ) {
    if ( trackCreator()->newTracks().isFailure() ) return StatusCode::FAILURE;
    debug() << "No tracks found : Created " << richTracks()->size()
            << " RichRecTracks " << richSegments()->size()
            << " RichRecSegments" << endmsg;
  }
  if ( msgLevel(MSG::DEBUG) ) {
    debug() << " Found " << richTracks()->size() << " tracks" << endmsg;
  }

  if ( (int)richTracks()->size() > m_maxUsedTracks ) {
    debug() << "Found " << richTracks()->size() << ">"
            << m_maxUsedTracks << " max usable tracks, stopping." << endmsg;
    return StatusCode::SUCCESS;
  }
  if ( richPixels()->empty() ) {
    if ( !pixelCreator()->newPixels().isFailure() ) return StatusCode::FAILURE;
    debug() << "No Pixels found : Created "
            << richPixels()->size() << " RichRecPixels" << endmsg;
  }

  if ( richPhotons()->empty() ) {
    photonCreator()->reconstructPhotons().ignore();
    debug() << "No photons found : Created "
            << richPhotons()->size() << " RichRecPhotons" << endmsg;
  }

  // Iterate over segments
  for ( LHCb::RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg ) {
    LHCb::RichRecSegment* segment = *iSeg;

    const Rich::DetectorType rich = segment->trackSegment().rich();

    // Radiator info
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    if (rad == Rich::Aerogel || rich != m_rich) continue;

    const std::string RAD = Rich::text(rad);

    // track selection
    if ( !m_trSelector->trackSelected(segment->richRecTrack()) ) continue;

    double thetaExpTrue(0.0), thetaExpected(0.0);
    if ( m_useMCTruth ) {
      // Get true beta from true particle type
      const Rich::ParticleIDType mcType = m_richRecMCTruth->mcParticleType( segment );
      debug() << "mcType:" << mcType << endmsg;
      if ( Rich::Unknown == mcType ) continue;
      plot( mcType, "mcType", "MC Particle type", -1.5, 5.5, 7 );
      m_pTypes[mcType]++;

      const double beta =
        m_richPartProp->beta( sqrt(segment->trackSegment().bestMomentum().Mag2()), mcType );
      plot1D(beta, "beta", "Beta of the track (MC)", 0.9, 1.0);

      // Expected Cherenkov theta angle for true particle type
      thetaExpTrue =  m_ckAngle->avgCherenkovTheta( segment, mcType );
    }

    thetaExpected =  m_ckAngle->avgCherenkovTheta( segment, m_pType);

    if ( msgLevel(MSG::DEBUG) )
      debug() << " Found " << segment->richRecPhotons().size() << " in this segment" << endmsg;

    for ( LHCb::RichRecSegment::Photons::const_iterator iPhot = segment->richRecPhotons().begin();
          iPhot != segment->richRecPhotons().end(); ++iPhot )
    {
      LHCb::RichRecPhoton* photon = *iPhot;

      // get the geometrical photon
      const LHCb::RichGeomPhoton & gPhoton = photon->geomPhoton();

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

      double delTheta = thetaRec - thetaExpected;
      if ( fabs(delTheta) > m_deltaThetaRange ) continue;

      double delThetaTrue(0.0);
      bool trueParent( false );
      if ( m_useMCTruth ) {
        delThetaTrue = thetaRec - thetaExpTrue;
        trueParent = ( NULL != m_richRecMCTruth->trueCherenkovPhoton( photon ) );
      }

      const bool unAmbiguousPhoton = photon->geomPhoton().unambiguousPhoton();
      plot1D(static_cast<int>(unAmbiguousPhoton), "Un_Amb",
             "Ambigious/Unambigious photons",-0.5,1.5,2 );

      if (m_useMCTruth && trueParent ) {
        plot1D( delThetaTrue, "deltaThetaTrueAll", "Ch angle error MC ALL",
                -m_deltaThetaHistoRange, m_deltaThetaHistoRange);

        if ( unAmbiguousPhoton ) {
          plot1D( delThetaTrue, "deltaThetaTrueUnamb", "Ch angle error MC Unambiguous",
                  -m_deltaThetaHistoRange, m_deltaThetaHistoRange);
          plot1D( delTheta, "deltaThetaUnambTP", "Ch angle error (Tru parent only) Unamb",
                  -m_deltaThetaHistoRange, m_deltaThetaHistoRange);
        }
        else
          plot1D( delThetaTrue, "deltaThetaTrueAmb", "Ch angle error MC Ambiguous",
                  -m_deltaThetaHistoRange, m_deltaThetaHistoRange);
      }

      if ( !unAmbiguousPhoton )
      {
        plot1D( delTheta, "deltaThetaAmb","Ch angle error (Ambiguous photons)",
                -m_deltaThetaHistoRange, m_deltaThetaHistoRange);
        continue;
      }

      plot1D( delTheta, "deltaThetaUnamb","Ch angle error (Unambigous photons)",
              -m_deltaThetaHistoRange, m_deltaThetaHistoRange);

      int side, side2, quarter;

      if ( rich == Rich::Rich1 )
      {
        plot1D( sphMirNum, "sphMirR1","Sph Mirror Numbers Rich1",-0.5,3.5,4);
        plot1D( flatMirNum, "fltMirR1","Flat Mirror Numbers Rich1",-0.5,15.5,16);

        if ( !m_minimalHistoOutput )
        {
          plot2D( gPhoton.sphMirReflectionPoint().x(),gPhoton.sphMirReflectionPoint().y(),
                  "sphMirReflR1", "Spherical Mirror Refl point Rich1",
                  -700, 700, -800, 800, 100, 100);
          plot2D( gPhoton.flatMirReflectionPoint().x(),gPhoton.flatMirReflectionPoint().y(),
                  "flatMirReflR1", "Flat Mirror Refl point Rich1",
                  -700, 700, -1000, 1000, 100, 100);
        }
        side = ( gPhoton.flatMirReflectionPoint().y() > 0.0 ? 0 : 1 );
        side2 = ( gPhoton.flatMirReflectionPoint().x() > 0.0 ? 0 : 1 );
        quarter = side*2+side2;
      }
      else
      {
        plot1D( sphMirNum, "sphMirR2","Sph Mirror Numbers Rich2",-0.5,55.5,56);
        plot1D( flatMirNum, "fltMirR2","Flat Mirror Numbers Rich2",-0.5,39.5,40);
        if ( !m_minimalHistoOutput )
        {
          plot2D( gPhoton.sphMirReflectionPoint().x(),gPhoton.sphMirReflectionPoint().y(),
                  "sphMirReflR2", "Spherical Mirror Refl point Rich2",
                  -1800, 1800, -1500, 1500, 100, 100);
          plot2D( gPhoton.flatMirReflectionPoint().x(),gPhoton.flatMirReflectionPoint().y(),
                  "flatMirReflR2", "Flat Mirror Refl point Rich2",
                  -3000, 3000, -1000, 1000, 100, 100);
        }
        side = ( gPhoton.flatMirReflectionPoint().x() > 0.0 ? 0 : 1 );
        side2 = ( gPhoton.flatMirReflectionPoint().y() > 0.0 ? 0 : 1 );
        quarter = side*2+side2;
      }
      // test for isolation
      const bool isolated = m_isoTrack->isIsolated( segment, m_pType );

      plot2D( phiRec, delTheta, "dThetavphiRecAll", "dTheta v phi All", 0.0,
              2*Gaudi::Units::pi, -m_deltaThetaHistoRange, m_deltaThetaHistoRange, 20, 50);
      if ( isolated ) plot2D( phiRec, delTheta, "dThetavphiRecIso", "dTheta v phi Isolated", 0.0,
                              2*Gaudi::Units::pi, -m_deltaThetaHistoRange, m_deltaThetaHistoRange, 20, 50);

      // and a separate histogram for each side
      fill( m_sideHistos[side], phiRec, delTheta, 1 );
      fill( m_quarterHistos[quarter], phiRec, delTheta, 1 );

      if ( isolated ) fill( m_sideIsolatedHistos[side], phiRec, delTheta, 1 );

      // for minimal histo output (online) stop here
      if ( !m_minimalHistoOutput )
      {
        // now for individual mirror combinations
        std::string title = RAD+" Alignment Histogram: Sph " +
          boost::lexical_cast<std::string>(sphMirNum) + " flat " +
          boost::lexical_cast<std::string>(flatMirNum) + " R" +
          boost::lexical_cast<std::string>(rich+1);
        std::string h_id( "dThetavphiRec" );

        std::string thisCombiNr( "" ); // only the 4-digit combination number (string)

        if ( sphMirNum > 9 )
          thisCombiNr +=       boost::lexical_cast<std::string>( sphMirNum );
        else
          thisCombiNr += "0" + boost::lexical_cast<std::string>( sphMirNum );
        if ( flatMirNum > 9 )
          thisCombiNr +=       boost::lexical_cast<std::string>( flatMirNum );
        else
          thisCombiNr += "0" + boost::lexical_cast<std::string>( flatMirNum );

        // depending on options, make plots only for prebooked mirror combimations.
        bool allowMirrorCombi( true );
        if ( m_onlyPrebookedMirrors )
          // search to see if this mirror combination has been prebooked
          if ( m_preBookHistos.empty() ||
               std::find( m_preBookHistos.begin(),m_preBookHistos.end(), thisCombiNr) == m_preBookHistos.end() )
            allowMirrorCombi = false;

        if ( allowMirrorCombi )
        {
          h_id += thisCombiNr;
          plot2D( phiRec, delTheta, hid(rad,h_id), title, 0.0, 2*Gaudi::Units::pi,
                  -m_deltaThetaHistoRange, m_deltaThetaHistoRange, 20, 50 );

          if ( m_useOnlyIsolatedTracks && isolated )
            plot2D( phiRec, delTheta, hid(rad,h_id+"Iso"), title+" Iso", 0.0, 2*Gaudi::Units::pi,
                    -m_deltaThetaHistoRange, m_deltaThetaHistoRange, 20, 50 );

          if ( m_useMCTruth ) {
            // use MC estimate for cherenkov angle
            h_id += "MC";
            title += " MC";
            plot2D( phiRec, delThetaTrue, hid(rad,h_id), title, 0.0, 2*Gaudi::Units::pi,
                    -m_deltaThetaHistoRange, m_deltaThetaHistoRange, 20, 50 );
            // test to see if this photon was emitted from this track
            if ( trueParent ) {
              h_id += "TruP";
              title += " TrueP";
              plot2D( phiRec, delThetaTrue, hid(rad,h_id), title, 0.0, 2*Gaudi::Units::pi,
                      -m_deltaThetaHistoRange, m_deltaThetaHistoRange, 20, 50 );
            }
          }
        }
        // end of filling prebooked and non-prebooked histograms for mirror combinations

        const int hpd = ( m_plotAllHPDs ? Rich::DAQ::HPDIdentifier( gPhoton.smartID() ).number()
                          : makePlotForHPD(gPhoton.smartID()) );

        if ( hpd != 0 )
        {
          const std::string hpd_id( "HPD_"+boost::lexical_cast<std::string>(hpd) );
          plot2D( phiRec, delTheta, "HPDs/"+hpd_id, hpd_id, 0.0, 2*Gaudi::Units::pi,
                  -m_deltaThetaHistoRange, m_deltaThetaHistoRange, 20, 50 );
        }
      }
    }
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode AlignmentMonitor::finalize()
{
  if ( m_useMCTruth ) {
    info() << "Number of pions:" << m_pTypes[Rich::Pion] << "; Kaons:"
           << m_pTypes[Rich::Kaon] << ";";
    if ( m_pTypes[Rich::Kaon] > 0 )
      info() << " ratio:" << m_pTypes[Rich::Pion] / m_pTypes[Rich::Kaon];
    info() << endmsg;
  }

  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}

//=========================================================================
//  makePlotForHPD
//=========================================================================
int AlignmentMonitor::makePlotForHPD ( LHCb::RichSmartID smartID ) const
{
  int hpd = Rich::DAQ::HPDIdentifier( smartID ).number();

  if ( m_HPDList.empty() || std::find( m_HPDList.begin(),
                                       m_HPDList.end(),
                                       hpd) == m_HPDList.end() )
    return 0;

  else
    return hpd;

}
