
/** @file RichAlignmentMonitor.cpp
 *
 *  Implementation file for algorithm class : RichAlignmentMonitor
 *
 *  CVS Log :-
 *  $Id: RichAlignmentMonitor.cpp,v 1.6 2006-01-23 14:10:48 jonrob Exp $
 *
 *  @author Antonis Papanestis
 *  @date   2004-02-19
 */

// local
#include "RichAlignmentMonitor.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichAlignmentMonitor>          s_factory ;
const        IAlgFactory& RichAlignmentMonitorFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichAlignmentMonitor::RichAlignmentMonitor( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : RichRecHistoAlgBase ( name , pSvcLocator ), m_histCounter(0)
{

  declareProperty( "HistoPath", m_histPth = "RICH/ALIGN/" );
  // Maximum number of tracks
  declareProperty( "MaxRichRecTracks", m_maxUsedTracks = 200 );
  // energy for saturated track
  declareProperty( "Rich1SaturationEnergy", m_saturationEnergyR1 = 20*GeV );
  declareProperty( "Rich2SaturationEnergy", m_saturationEnergyR2 = 30*GeV );
  declareProperty( "Rich2SaturationEnergy2", m_2ndSaturationEnergyR2 = 90*GeV );
  declareProperty( "DeltaThetaRange", m_deltaThetaRange = 0.004 );
  declareProperty( "UseMCTruth",   m_useMCTruth     = false );
  declareProperty( "UseAllTracks", m_useAllTracks   = false );
  declareProperty( "AssocTrackPhoton", m_assocTrackPhoton = false );
  declareProperty( "HighSatEnergyRich2", m_highSatEnergyRich2= false );
  declareProperty( "PreBookHistos", m_preBookHistos );

  m_saturationEnergy[0] = m_saturationEnergyR1;
  m_saturationEnergy[1] = m_saturationEnergyR2;

  m_deltaThetaHistoRange = floor((m_deltaThetaRange + 0.0014)*1000)/1000.0;

  m_histPthMCLevel[0] = "NO_MC_";
  m_histPthMCLevel[1] = "NO_MC_Rich2";
  m_histPthMCLevel[2] = "SAT_TR_TRU_PHOTON";
  m_histPthMCLevel[3] = "ALL_TR_MC_ANGLE";

  for( int l=0; l<4; ++l)
    for( int r=0; r<2; ++r)
      for (int i=0; i<6000; ++i)
        m_histIndex[l][r][i] = -1;

}
//=============================================================================
// Destructor
//=============================================================================
RichAlignmentMonitor::~RichAlignmentMonitor() {};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode RichAlignmentMonitor::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  acquireTool( "RichRefractiveIndex",  m_richRefIndexTool );
  acquireTool( "RichCherenkovAngle",   m_ckAngle          );

  if ( m_useMCTruth ) {
    acquireTool( "RichRecMCTruthTool",    m_richRecMCTruth );
    acquireTool( "RichParticleProperties", m_richPartProp );
  }

  m_saturatedAngle[0] = acos( 1/m_richRefIndexTool->
                              refractiveIndex(Rich::C4F10) );

  m_saturatedAngle[1] = acos( 1/m_richRefIndexTool->
                              refractiveIndex(Rich::CF4) );

  debug() << "Saturated angle for Rich1 " << m_saturatedAngle[0] <<  endmsg
          << "Saturated angle for Rich2 " << m_saturatedAngle[1] << endmsg;

  if (!m_useMCTruth) {
    if ( m_useAllTracks || m_assocTrackPhoton )
      warning() << "All MCTruth info will be ignored" << endmsg;

    m_useAllTracks = false;
    m_assocTrackPhoton = false;
  }

  if ( !bookHistos() ) return StatusCode::FAILURE;

  debug() << "Finished Initialization" << endmsg;
  return sc;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode RichAlignmentMonitor::execute() {

  debug() << "Execute" << endreq;

  // Check Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

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

  if ( m_buildPhotons && richPhotons()->empty() ) {
    photonCreator()->reconstructPhotons();
    debug() << "No photons found : Created "
            << richPhotons()->size() << " RichRecPhotons" << endreq;
  }

  // Iterate over segments
  for ( RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg ) {
    RichRecSegment* segment = *iSeg;

    // Radiator info
    const Rich::RadiatorType rad = segment->trackSegment().radiator();
    if (rad == Rich::Aerogel) continue;

    Rich::Track::Type trType = segment->richRecTrack()->trackID().trackType();

    const Rich::DetectorType rich = segment->trackSegment().rich();
    int hrich = rich+1;

    // Segment momentum
    const double ptot = sqrt(segment->trackSegment().bestMomentum().Mag2());
    debug() << "Momentum " << ptot << endmsg;

    bool saturated( false );
    if ( ptot > m_saturationEnergy[rich] ) saturated = true;

    if ( !m_useAllTracks && !saturated) continue;

    double thetaExpTrue(0.0), thetaExpected(0.0);
    if ( m_useMCTruth ) {
      // Get true beta from true particle type
      const Rich::ParticleIDType mcType = m_richRecMCTruth->
        mcParticleType( segment );
      if ( Rich::Unknown == mcType ) continue;
      debug() << "mcType:" << mcType << endmsg;
      const double beta =
        m_richPartProp->beta( sqrt(segment->trackSegment().bestMomentum().Mag2()), mcType );
      m_trackBeta[rich]->fill(beta);
      // Expected Cherenkov theta angle for true particle type

      thetaExpTrue =  m_ckAngle->avgCherenkovTheta( segment, mcType );
    }

    thetaExpected =  m_ckAngle->avgCherenkovTheta( segment, Rich::Pion);

    // Get photons for this segment
    const RichRecSegment::Photons& photons = photonCreator()->
      reconstructPhotons( segment );
    if ( msgLevel(MSG::DEBUG) ) {
      debug()
        << " Found " << photons.size() << " photon candidates in "
        << rich << endreq;
    }

    for ( RichRecSegment::Photons::const_iterator iPhot = photons.begin();
          iPhot != photons.end();
          ++iPhot ) {
      RichRecPhoton* photon = *iPhot;

      // get the geometrical photon
      RichGeomPhoton gPhoton = photon->geomPhoton();

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
        trueParent = m_richRecMCTruth->trueCherenkovPhoton( photon );
      }

      bool unAmbiguousPhoton = photon->geomPhoton().mirrorNumValid();
      m_ambigMirrorsHist[rich]->fill(static_cast<int>(unAmbiguousPhoton));

      if (m_useMCTruth && trueParent && trType == Rich::Track::Forward) {
        plot( delThetaTrue, hrich*100+90, "Ch angle error MC forward ALL",
              -m_deltaThetaHistoRange, m_deltaThetaHistoRange);

        if ( unAmbiguousPhoton )
          plot( delThetaTrue, hrich*100+91, "Ch angle error MC forward Unambiguous",
                -m_deltaThetaHistoRange, m_deltaThetaHistoRange);
        else
          plot( delThetaTrue, hrich*100+92, "Ch angle error MC forward Ambiguous",
                -m_deltaThetaHistoRange, m_deltaThetaHistoRange);
      }


      if (!unAmbiguousPhoton) continue;


      if ( rich == Rich::Rich2 ){
        if ( ptot > 30*GeV ) {
          plot( delTheta, hrich*100+10,"Ch angle error > 30GeV",
                -m_deltaThetaHistoRange, m_deltaThetaHistoRange);
          if (trType == Rich::Track::Forward)
            plot( delTheta, hrich*100+12,"Ch angle error > 30GeV Forward",
                  -m_deltaThetaHistoRange, m_deltaThetaHistoRange);
          if ( trueParent )
            plot( delTheta, hrich*100+11,"Ch angle error > 30GeV TrueParent",
                  -m_deltaThetaHistoRange, m_deltaThetaHistoRange);
        }

        if ( ptot > 40*GeV ) {
          plot( delTheta, hrich*100+20,"Ch angle error > 40GeV",
                -m_deltaThetaHistoRange, m_deltaThetaHistoRange);
          if (trType == Rich::Track::Forward)
            plot( delTheta, hrich*100+22,"Ch angle error > 40GeV Forward",
                  -m_deltaThetaHistoRange, m_deltaThetaHistoRange);
          if ( trueParent )
            plot( delTheta, hrich*100+21,"Ch angle error > 40GeV TrueParent",
                  -m_deltaThetaHistoRange, m_deltaThetaHistoRange);
        }

        if ( ptot > 60*GeV ) {
          plot( delTheta, hrich*100+30,"Ch angle error > 60GeV",
                -m_deltaThetaHistoRange, m_deltaThetaHistoRange);
          if (trType == Rich::Track::Forward)
            plot( delTheta, hrich*100+32,"Ch angle error > 60GeV Forward",
                  -m_deltaThetaHistoRange, m_deltaThetaHistoRange);
          if ( trueParent )
            plot( delTheta, hrich*100+31,"Ch angle error > 60GeV TrueParent",
                  -m_deltaThetaHistoRange, m_deltaThetaHistoRange);
        }

        if ( ptot > 90*GeV ) {
          plot( delTheta, hrich*100+40,"Ch angle error > 90GeV",
                -m_deltaThetaHistoRange, m_deltaThetaHistoRange);
          if (trType == Rich::Track::Forward)
            plot( delTheta, hrich*100+42,"Ch angle error > 90GeV Forward",
                  -m_deltaThetaHistoRange, m_deltaThetaHistoRange);
          if ( trueParent )
            plot( delTheta, hrich*100+41,"Ch angle error > 90GeV TrueParent",
                  -m_deltaThetaHistoRange, m_deltaThetaHistoRange);
        }

      }

      int combiNumber = sphMirNum*100 + flatMirNum;

      if ( saturated ) {

        m_ChAngleError[rich]->fill(delTheta);

        if ( m_useMCTruth && trueParent )
          plot(delThetaTrue, hrich*100+99, "Ch angle error MCTruth",
               -m_deltaThetaHistoRange, m_deltaThetaHistoRange);

        m_sphMirrorNumberHist[rich]->fill(sphMirNum);
        m_flatMirrorNumberHist[rich]->fill(flatMirNum);
        m_sphMirReflPoint[rich]->fill( gPhoton.sphMirReflectionPoint().x(),
                                       gPhoton.sphMirReflectionPoint().y() );
        m_flatMirReflPoint[rich]->fill( gPhoton.flatMirReflectionPoint().x(),
                                        gPhoton.flatMirReflectionPoint().y() );

        if ( m_histIndex[NO_MC][rich][combiNumber] == -1 ) {  // histo not booked
          int id = (rich+1)*10000 + combiNumber;
          bookPhiHisto(NO_MC, id);
        }

        m_alignmentHist[m_histIndex[NO_MC][rich][combiNumber]]->fill(phiRec, delTheta);
        m_ChAngleErrorvPhiUnamb[rich]->fill(phiRec, delTheta);

        // test to see if this photon was emitted from this track
        if ( m_assocTrackPhoton )
          if ( trueParent ) {
            if ( m_histIndex[TRACK][rich][combiNumber] == -1 ) {  // histo not booked, book it
              int id = (rich+1)*10000 + combiNumber;
              bookPhiHisto(TRACK, id);
            }
            m_alignmentHist[m_histIndex[TRACK][rich][combiNumber]]->fill(phiRec, delTheta);
            debug() << "Filled histo TRACK" << endmsg;
          }

        if ( m_highSatEnergyRich2 && (rich==Rich::Rich2) && (ptot>m_2ndSaturationEnergyR2) ) {
          if ( m_histIndex[NO_MC2_RICH2][rich][combiNumber] == -1 ) {  // histo not booked
            int id = (rich+1)*10000 + combiNumber;
            bookPhiHisto(NO_MC2_RICH2, id);
          }

          m_alignmentHist[m_histIndex[NO_MC2_RICH2][rich][combiNumber]]->fill(phiRec, delTheta);
        }
      }

      // if all tracks are used, use true Ch angle
      if ( m_useAllTracks && trueParent ) {

        if ( m_histIndex[TR_A_ANGLE][rich][combiNumber] == -1 ) {  // histo not booked, book it
          int id = hrich*10000 + combiNumber;
          bookPhiHisto(TR_A_ANGLE, id);
        }

        m_alignmentHist[m_histIndex[TR_A_ANGLE][rich][combiNumber]]->
          fill(phiRec, delThetaTrue);
        debug() << "Filled histo TR_A_ANGLE" << endmsg;
      }

    }

  }


  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichAlignmentMonitor::finalize()
{
  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}

//=========================================================================
//  Book deltaTheta-Phi Histogram
//=========================================================================

StatusCode RichAlignmentMonitor::bookPhiHisto(MCINFO level, int id)
{
  std::string histPth = m_histPth + m_histPthMCLevel[level];
  int rich = id/10000 - 1;
  int combi = id%10000;

  std::string title = "Alignment Histogram: Sph " +
    boost::lexical_cast<std::string>(combi/100) + " flat " +
    boost::lexical_cast<std::string>(combi%100) + " " +
    boost::lexical_cast<std::string>(rich);

  debug() << "Booking histo " << m_histCounter << " in "<< histPth <<" with id:"
          << id << " MC level "<< level <<" and combiNumber " << combi << endreq;
  debug() << "m_histIndex " << m_histIndex[level][rich][combi];


  m_alignmentHist[m_histCounter]=histoSvc()->
    book(histPth,id,title,20,0,6.3,50,-m_deltaThetaHistoRange,m_deltaThetaHistoRange);

  m_histIndex[level][rich][combi] = m_histCounter;
  ++m_histCounter;

  return StatusCode::SUCCESS;

}

//=========================================================================
//  Book Histograms
//=========================================================================

StatusCode RichAlignmentMonitor::bookHistos() {

  debug() << "Booking Histos" << endmsg;

  // book histos
  m_ambigMirrorsHist[0]=histoSvc()->
    book(m_histPth,1,"Ambigious / Unambigious mirrors R1",2,-0.5,2.5);
  m_ambigMirrorsHist[1]=histoSvc()->
    book(m_histPth,2,"Ambigious / Unambigious mirrors R2",2,-0.5,2.5);

  m_sphMirrorNumberHist[0]=histoSvc()->
    book(m_histPth,11,"Sph Mirror Numbers Rich1",4,-0.5,3.5);
  m_sphMirrorNumberHist[1]=histoSvc()->
    book(m_histPth,21,"Sph Mirror Numbers Rich2",60,-0.5,59.5);

  m_flatMirrorNumberHist[0]=histoSvc()->
    book(m_histPth,12,"Flat Mirror Numbers Rich1",4,-0.5,3.5);
  m_flatMirrorNumberHist[1]=histoSvc()->
    book(m_histPth,22,"Flat Mirror Numbers Rich2",60,-0.5,59.5);

  m_ChAngleError[0] = histoSvc()->
    book(m_histPth, 14, "Ch Angle Error (All b=1 tracks R1)",50,-0.005,0.005);
  m_ChAngleError[1] = histoSvc()->
    book(m_histPth, 24, "Ch Angle Error (All b=1 tracks R2)",50,-0.005,0.005);

  m_ChAngleErrorvPhiUnamb[0] = histoSvc()->
    book(m_histPth, 17, "Ch Angle Error (Unamb. b=1 tracks R1)", 20, 0, 6.28,
         40, -0.01, 0.01);
  m_ChAngleErrorvPhiUnamb[1] = histoSvc()->
    book(m_histPth, 27, "Ch Angle Error (Unamb. b=1 tracks R2)", 20, 0, 6.28,
         40, -0.01, 0.01);

  m_sphMirReflPoint[0] = histoSvc()->
    book(m_histPth, 101, "Spherical Mirror Refl Point (Rec) R1", 100, -700,
         700, 100, -800, 800);
  m_sphMirReflPoint[1] = histoSvc()->
    book(m_histPth, 201, "Spherical Mirror Refl Point (Rec) R2", 100, -1800,
         1800, 100, -1500, 1500);

  m_flatMirReflPoint[0] = histoSvc()->
    book(m_histPth, 102, "Flat Mirror Refl Point (Rec) R1", 100, -700,
         700, 100, -1000, 1000);
  m_flatMirReflPoint[1] = histoSvc()->
    book(m_histPth, 202, "Flat Mirror Refl Point (Rec) R2", 100, -3000,
         3000, 100, -1000, 1000);

  if ( m_useMCTruth ) {
    m_trackBeta[0]=histoSvc()->
      book(m_histPth,18,"Track Segment Beta Rich1",100,0.9,1.0);

    m_trackBeta[1]=histoSvc()->
      book(m_histPth,28,"Track Segment Beta Rich2",100,0.9,1.0);
  }

  for ( unsigned int hi=0; hi<m_preBookHistos.size(); ++hi ) {
    bookPhiHisto( NO_MC, m_preBookHistos[hi] );
    if ( m_highSatEnergyRich2 ) bookPhiHisto( NO_MC2_RICH2, m_preBookHistos[hi] );
    if ( m_assocTrackPhoton ) bookPhiHisto( TRACK, m_preBookHistos[hi] );
    if ( m_useAllTracks ) bookPhiHisto( TR_A_ANGLE, m_preBookHistos[hi] );
  }

  debug() << "Finished booking Histos" << endmsg;

  return StatusCode::SUCCESS;
}

