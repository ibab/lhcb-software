
/** @file RichAlignmentMonitor.cpp
 *
 *  Implementation file for algorithm class : RichAlignmentMonitor
 *
 *  CVS Log :-
 *  $Id: RichAlignmentMonitor.cpp,v 1.11 2007-02-02 10:07:11 jonrob Exp $
 *
 *  @author Antonis Papanestis
 *  @date   2004-02-19
 */

// local
#include "RichAlignmentMonitor.h"

// From Gaudi
#include "GaudiKernel/SystemOfUnits.h"

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
    m_trSelector        ( 0 ),
    m_richRecMCTruth    ( 0 ),
    m_richPartProp      ( 0 ),
    m_ckAngle           ( 0 )
{

  // Maximum number of tracks
  declareProperty( "MaxRichRecTracks", m_maxUsedTracks = 200 );
  declareProperty( "DeltaThetaRange", m_deltaThetaRange = 0.004 );
  declareProperty( "UseMCTruth",   m_useMCTruth     = false );
  declareProperty( "PreBookHistos", m_preBookHistos );
  declareProperty( "ParticleType", m_particleType = 2 ); // default is pion
  declareProperty( "RichDetector", m_richTemp = 1 ); // default is Rich2

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

  // Rich Histo ID
  const RichHistoID hid;

  // prebook histograms
  for ( unsigned int hi=0; hi<m_preBookHistos.size(); ++hi ) {
    int combi = m_preBookHistos[hi];
    std::string title = "Alignment Histogram: Sph " +
      boost::lexical_cast<std::string>(combi/100) + " flat " +
      boost::lexical_cast<std::string>(combi%100) + " R" +
      boost::lexical_cast<std::string>(m_rich+1);
    std::string h_id("dThetavphiRec");
    if ( combi/100 > 9 )
      h_id += boost::lexical_cast<std::string>(combi);
    else
      h_id += "0" + boost::lexical_cast<std::string>(combi);

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

  debug() << "Finished Initialization" << endmsg;
  return sc;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode AlignmentMonitor::execute() {

  debug() << "Execute" << endreq;

  // Check Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Rich Histo ID
  const RichHistoID hid;

  // If any containers are empty, form them
  if ( richTracks()->empty() ) {
    if ( !trackCreator()->newTracks() ) return StatusCode::FAILURE;
    debug() << "No tracks found : Created " << richTracks()->size()
            << " RichRecTracks " << richSegments()->size()
            << " RichRecSegments" << endreq;
  }
  if ( msgLevel(MSG::DEBUG) ) {
    debug() << " Found " << richTracks()->size() << " tracks" << endreq;
  }

  if ( richTracks()->size() > m_maxUsedTracks ) {
    debug() << "Found " << richTracks()->size() << ">"
            << m_maxUsedTracks << " max usable tracks, stopping." << endreq;
    return StatusCode::SUCCESS;
  }
  if ( richPixels()->empty() ) {
    if ( !pixelCreator()->newPixels() ) return StatusCode::FAILURE;
    debug() << "No Pixels found : Created "
            << richPixels()->size() << " RichRecPixels" << endreq;
  }

  if ( richPhotons()->empty() ) {
    photonCreator()->reconstructPhotons();
    debug() << "No photons found : Created "
            << richPhotons()->size() << " RichRecPhotons" << endreq;
  }

  // Iterate over segments
  for ( LHCb::RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg ) {
    LHCb::RichRecSegment* segment = *iSeg;

    const Rich::DetectorType rich = segment->trackSegment().rich();

    // Radiator info
    const Rich::RadiatorType rad = segment->trackSegment().radiator();
    if (rad == Rich::Aerogel || rich != m_rich) continue;

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

    for ( LHCb::RichRecSegment::Photons::const_iterator iPhot = segment->richRecPhotons().begin();
          iPhot != segment->richRecPhotons().end(); ++iPhot )
    {
      LHCb::RichRecPhoton* photon = *iPhot;

      // get the geometrical photon
      const LHCb::RichGeomPhoton & gPhoton = photon->geomPhoton();

      // Cherenkov angles
      const double thetaRec = gPhoton.CherenkovTheta();
      const double phiRec = gPhoton.CherenkovPhi();
      const MirrorNumber sphMirNum = gPhoton.sphMirrorNum();
      const MirrorNumber flatMirNum = gPhoton.flatMirrorNum();

      double delTheta = thetaRec - thetaExpected;
      if ( fabs(delTheta) > m_deltaThetaRange ) continue;

      double delThetaTrue(0.0);
      bool trueParent( false );
      if ( m_useMCTruth ) {
        delThetaTrue = thetaRec - thetaExpTrue;
        trueParent = ( NULL != m_richRecMCTruth->trueCherenkovPhoton( photon ) );
      }

      bool unAmbiguousPhoton = photon->geomPhoton().mirrorNumValid();
      plot(static_cast<int>(unAmbiguousPhoton), "Un_Amb",
           "Ambigious/Unambigious photons",-0.5,1.5,2 );

      if (m_useMCTruth && trueParent ) {
        plot( delThetaTrue, "deltaThetaTrueAll", "Ch angle error MC ALL",
              -m_deltaThetaHistoRange, m_deltaThetaHistoRange);

        if ( unAmbiguousPhoton ) {
          plot( delThetaTrue, "deltaThetaTrueUnamb", "Ch angle error MC Unambiguous",
                -m_deltaThetaHistoRange, m_deltaThetaHistoRange);
          plot( delTheta, "deltaThetaUnambTP", "Ch angle error (Tru parent only) Unamb",
                -m_deltaThetaHistoRange, m_deltaThetaHistoRange);
        }
        else
          plot( delThetaTrue, "deltaThetaTrueAmb", "Ch angle error MC Ambiguous",
                -m_deltaThetaHistoRange, m_deltaThetaHistoRange);
      }

      if (!unAmbiguousPhoton) continue;

      plot( delTheta, "deltaThetaUnamb","Ch angle error (Unambigous photons)",
            -m_deltaThetaHistoRange, m_deltaThetaHistoRange);

      int side;

      if ( rich == Rich::Rich1 ) {
        plot( sphMirNum, "sphMirR1","Sph Mirror Numbers Rich1",-0.5,3.5,4);
        plot( flatMirNum, "fltMirR1","Flat Mirror Numbers Rich1",-0.5,15.5,16);
        plot2D( gPhoton.sphMirReflectionPoint().x(),gPhoton.sphMirReflectionPoint().y(),
                "sphMirReflR1", "Spherical Mirror Refl point Rich1",
                -700, 700, -800, 800, 100, 100);
        plot2D( gPhoton.flatMirReflectionPoint().x(),gPhoton.flatMirReflectionPoint().y(),
                "flatMirReflR1", "Flat Mirror Refl point Rich1",
                -700, 700, -1000, 1000, 100, 100);
        side = ( gPhoton.flatMirReflectionPoint().y() > 0.0 ? 0 : 1 );
      }
      else {
        plot( sphMirNum, "sphMirR2","Sph Mirror Numbers Rich2",-0.5,55.5,56);
        plot( flatMirNum, "fltMirR2","Flat Mirror Numbers Rich2",-0.5,39.5,40);
        plot2D( gPhoton.sphMirReflectionPoint().x(),gPhoton.sphMirReflectionPoint().y(),
                "sphMirReflR2", "Spherical Mirror Refl point Rich2",
                -1800, 1800, -1500, 1500, 100, 100);
        plot2D( gPhoton.flatMirReflectionPoint().x(),gPhoton.flatMirReflectionPoint().y(),
                "flatMirReflR2", "Flat Mirror Refl point Rich2",
                -3000, 3000, -1000, 1000, 100, 100);
        side = ( gPhoton.flatMirReflectionPoint().x() > 0.0 ? 0 : 1 );
      }
      plot2D( phiRec, delTheta, "dThetavphiRecAll", "dTheta v phi All", 0.0,
              2*Gaudi::Units::pi, -m_deltaThetaHistoRange, m_deltaThetaHistoRange, 20, 50);

      if ( 0 == side )
        plot2D( phiRec, delTheta, "dThetavphiRecSide0", "dTheta v phi Side 0", 0.0,
                2*Gaudi::Units::pi, -m_deltaThetaHistoRange, m_deltaThetaHistoRange, 20, 50);
      else
        plot2D( phiRec, delTheta, "dThetavphiRecSide1", "dTheta v phi Side 1", 0.0,
                2*Gaudi::Units::pi, -m_deltaThetaHistoRange, m_deltaThetaHistoRange, 20, 50);

      // now for individual mirror combinations
      std::string title = "Alignment Histogram: Sph " +
        boost::lexical_cast<std::string>(sphMirNum) + " flat " +
        boost::lexical_cast<std::string>(flatMirNum) + " R" +
        boost::lexical_cast<std::string>(rich+1);
      std::string h_id( "dThetavphiRec" );

      int combi = sphMirNum*100 + flatMirNum;
      if ( sphMirNum > 9 )
        h_id += boost::lexical_cast<std::string>(combi);
      else
        h_id += "0" + boost::lexical_cast<std::string>(combi);

      plot2D( phiRec, delTheta, hid(rad,h_id), title, 0.0, 2*Gaudi::Units::pi,
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

