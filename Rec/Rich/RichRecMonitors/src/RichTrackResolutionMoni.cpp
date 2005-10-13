
//---------------------------------------------------------------------------
/** @file RichTrackResolutionMoni.cpp
 *
 *  Implementation file for algorithm class : RichTrackResolutionMoni
 *
 *  CVS Log :-
 *  $Id: RichTrackResolutionMoni.cpp,v 1.2 2005-10-13 15:45:45 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichTrackResolutionMoni.h"

//---------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichTrackResolutionMoni>          s_factory ;
const        IAlgFactory& RichTrackResolutionMoniFactory = s_factory ;

// Standard constructor, initializes variables
RichTrackResolutionMoni::RichTrackResolutionMoni( const std::string& name,
                                                  ISvcLocator* pSvcLocator )
  : RichRecAlgBase ( name, pSvcLocator ),
    m_richRecMCTruth ( 0 ),
    m_mcTkInfo       ( 0 ),
    m_chargeSel      ( 0 )
{

  // histogram locations
  declareProperty( "MCHistoPath", m_mcHistPth = "RICH/RecTrRes/MC/" );
  declareProperty( "HistoPath",   m_histPth   = "RICH/RecTrRes/" );

  // track selector
  declareProperty( "TrackSelection", m_trSelector.selectedTrackTypes() );

  // charge selection
  declareProperty( "TrackChargeSel", m_chargeSel = 0 );

}

// Destructor
RichTrackResolutionMoni::~RichTrackResolutionMoni() {};

//  Initialize
StatusCode RichTrackResolutionMoni::initialize() 
{
  // Sets up various tools and services
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth );
  acquireTool( "RichMCTrackInfoTool",  m_mcTkInfo       );

  // Configure track selector
  if ( !m_trSelector.configureTrackTypes() ) return StatusCode::FAILURE;
  m_trSelector.printTrackSelection( info() );

  // Book histograms
  if ( !bookHistograms() || !bookMCHistograms() ) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

const StatusCode RichTrackResolutionMoni::bookHistograms() {

  std::string title;
  int id;
  const int nBins = 100;

  // Defines for various parameters
  RAD_HISTO_OFFSET;
  RADIATOR_NAMES;

  // Selected track types
  const std::string selTks = m_trSelector.selectedTracksAsString();

  title = "# RICH tracks V total # tracks";
  id = 1;
  m_nTks = histoSvc()->book(m_histPth,id,title,nBins,0,1000,nBins,0,300);

  for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad ) {

    title = "# segments per event : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 1;
    m_nSegsRad[iRad] = histoSvc()->book(m_histPth,id,title,101,-0.5,100.5);

    title = "Reco. entry/exit angle : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 2;
    m_recAngInOut[iRad] = histoSvc()->book(m_histPth,id,title,nBins,0,0.01);

  } // end radiator loop

  return StatusCode::SUCCESS;
}

const StatusCode RichTrackResolutionMoni::bookMCHistograms() {

  std::string title;
  int id;
  const int nBins = 100;

  // Defines for various parameters
  RAD_HISTO_OFFSET;
  RADIATOR_NAMES;

  // Selected track types
  const std::string selTks = m_trSelector.selectedTracksAsString();

  for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad ) {

    title = "Fraction of segments with MC info : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 1;
    m_segWithMc[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,0,1);

    title = "MC entry/exit angle : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 2;
    m_mcAngInOut[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,0,0.01);

    title = "Reco-MC entry/exit angle : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 3;
    m_angInOutRes[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,-0.01,0.01);

    title = "# G4 segments : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 4;
    m_nMCTrajPoints[iRad] = histoSvc()->book(m_mcHistPth,id,title,101,-0.5,100.5);

    title = "Rec-MC rad entry X : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 11;
    m_dTrEntrX[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,-5,5);

    title = "Rec-MC rad entry Y : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 12;
    m_dTrEntrY[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,-5,5);

    title = "Rec-MC rad entry Z : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 13;
    m_dTrEntrZ[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,-5,5);

    title = "Rec-MC rad exit X : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 21;
    m_dTrExitX[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,-5,5);

    title = "Rec-MC rad exit Y : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 22;
    m_dTrExitY[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,-5,5);

    title = "Rec-MC rad exit Z : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 23;
    m_dTrExitZ[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,-5,5);

    title = "Rec-MC rad pathLength : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 31;
    m_dTrPathL[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,-5,5);

    title = "Rec-MC panel X : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 41;
    m_dTrPanelX[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,-5,5);

    title = "Rec-MC panel Y : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 42;
    m_dTrPanelY[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,-5,5);

    title = "Rec-MC panel Z : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 43;
    m_dTrPanelZ[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,-5,5);

    title = "Angle between Rec/MC dir at entry : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 51;
    m_dTrAngleEntry[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,0,0.01);

    title = "Angle between Rec/MC dir at middle point : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 52;
    m_dTrAngleMid[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,0,0.01);

    title = "Angle between Rec/MC dir at exit : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 53;
    m_dTrAngleExit[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,0,0.01);

    title = "Rec-MC tx entry : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 61;
    m_txResEntry[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,-0.01,0.01);

    title = "Rec-MC ty entry : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 62;
    m_tyResEntry[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,-0.01,0.01);

    title = "Rec-MC tx exit : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 63;
    m_txResExit[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,-0.01,0.01);

    title = "Rec-MC ty exit : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 64;
    m_tyResExit[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,-0.01,0.01);

    title = "entry x pull : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 71;
    m_xPullEntry[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,-10,10);

    title = "entry y pull : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 72;
    m_yPullEntry[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,-10,10);

    title = "entry tx pull : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 73;
    m_txPullEntry[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,-10,10);

    title = "entry ty pull : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 74;
    m_tyPullEntry[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,-10,10);

    title = "exit x pull : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 75;
    m_xPullExit[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,-10,10);

    title = "exit y pull : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 76;
    m_yPullExit[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,-10,10);

    title = "exit tx pull : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 77;
    m_txPullExit[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,-10,10);

    title = "exit ty pull : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 78;
    m_tyPullExit[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,-10,10);

    title = "entry P pull : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 81;
    m_pPullEntry[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,-10,10);

    title = "exit P pull : " + selTks + " : " + radiator[iRad];
    id = radOffset*(iRad+1) + 82;
    m_pPullExit[iRad] = histoSvc()->book(m_mcHistPth,id,title,nBins,-10,10);

  } // radiator loop

  return StatusCode::SUCCESS;
}

// Main execution
StatusCode RichTrackResolutionMoni::execute() {

  debug() << "Execute" << endreq;

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Make sure all tracks and segments have been formed
  if ( richTracks()->empty() ) {
    if ( !trackCreator()->newTracks() ) return StatusCode::FAILURE;
    debug() << "No tracks found : Created " << richTracks()->size()
            << " RichRecTracks " << richSegments()->size()
            << " RichRecSegments" << endreq;
  }

  // Number of tracks
  m_nTks->fill( trackCreator()->nInputTracks(), richTracks()->size() );

  // Tally number of segments
  unsigned int nSegs[Rich::NRadiatorTypes]   = { 0, 0, 0 };
  unsigned int nMCSegs[Rich::NRadiatorTypes] = { 0, 0, 0 };

  // Iterate over segments
  for ( RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg ) {
    RichRecSegment * segment = *iSeg;

    // apply track selection
    if ( !m_trSelector.trackSelected( segment->richRecTrack() ) ) continue;

    // apply charge selection
    if ( m_chargeSel && m_chargeSel * segment->richRecTrack()->charge() < 0 ) continue;

    // True type ( skip electrons )
    const Rich::ParticleIDType mcType = m_richRecMCTruth->mcParticleType( segment );
    if ( Rich::Electron == mcType || Rich::Unknown == mcType ) continue;

    // track segment
    const RichTrackSegment & trackSeg = segment->trackSegment();

    //const Rich::DetectorType iRich = trackSeg.rich();    // which rich detector
    const Rich::RadiatorType iRad = trackSeg.radiator();   // which radiator
    ++nSegs[iRad]; // count segments per radiator

    verbose() << "Selected " << segment->richRecTrack()->trackID().trackType()
              << " RichRecSegment in " << iRad << endreq;

    // Ray traced hit point on PDPanel
    const HepPoint3D & pdPoint = segment->pdPanelHitPoint();

    // Angle between entry and exit vectors
    const double recoInOutAng = trackSeg.entryMomentum().angle(trackSeg.exitMomentum());
    m_recAngInOut[iRad]->fill( recoInOutAng );

    // Get associated RichMCSegment
    const MCRichSegment * mcSegment = m_richRecMCTruth->mcRichSegment(segment);
    if ( mcSegment ) {
      ++nMCSegs[iRad]; // count MC segments per radiator

      m_nMCTrajPoints[iRad]->fill( mcSegment->trajectoryPoints().size() );

      // position resolution plots
      m_dTrEntrX[iRad]->fill( trackSeg.entryPoint().x() - mcSegment->entryPoint().x() );
      m_dTrEntrY[iRad]->fill( trackSeg.entryPoint().y() - mcSegment->entryPoint().y() );
      m_dTrEntrZ[iRad]->fill( trackSeg.entryPoint().z() - mcSegment->entryPoint().z() );
      m_dTrExitX[iRad]->fill( trackSeg.exitPoint().x() - mcSegment->exitPoint().x() );
      m_dTrExitY[iRad]->fill( trackSeg.exitPoint().y() - mcSegment->exitPoint().y() );
      m_dTrExitZ[iRad]->fill( trackSeg.exitPoint().z() - mcSegment->exitPoint().z() );
      m_dTrPathL[iRad]->fill( trackSeg.pathLength() - mcSegment->pathLength() );

      // Panel ray-traced point
      HepPoint3D mcGlobal;
      if ( m_mcTkInfo->panelIntersectGlobal( mcSegment, mcGlobal ) ) {
        m_dTrPanelX[iRad]->fill( pdPoint.x()-mcGlobal.x() );
        m_dTrPanelY[iRad]->fill( pdPoint.y()-mcGlobal.y() );
        m_dTrPanelZ[iRad]->fill( pdPoint.z()-mcGlobal.z() );
      }

      // angle between mc and reco vectors
      m_dTrAngleEntry[iRad]->fill( trackSeg.entryMomentum().angle( mcSegment->entryMomentum() ) );
      m_dTrAngleMid[iRad]->fill( trackSeg.bestMomentum().angle( mcSegment->bestMomentum(0.5) ) );
      m_dTrAngleExit[iRad]->fill( trackSeg.exitMomentum().angle( mcSegment->exitMomentum() ) );

      // direction resolutions
      const double zrecoEnt  = trackSeg.entryMomentum().z();
      const double txrecoEnt = ( fabs(zrecoEnt)>0 ? trackSeg.entryMomentum().x()/zrecoEnt : 0 );
      const double tyrecoEnt = ( fabs(zrecoEnt)>0 ? trackSeg.entryMomentum().y()/zrecoEnt : 0 );
      const double zmcEnt    = mcSegment->entryMomentum().z();
      const double txmcEnt   = ( fabs(zmcEnt)>0 ? mcSegment->entryMomentum().x()/zmcEnt : 0 );
      const double tymcEnt   = ( fabs(zmcEnt)>0 ? mcSegment->entryMomentum().y()/zmcEnt : 0 );
      m_txResEntry[iRad]->fill( txrecoEnt - txmcEnt );
      m_tyResEntry[iRad]->fill( tyrecoEnt - tymcEnt );
      const double zrecoEx  = trackSeg.exitMomentum().z();
      const double txrecoEx = ( fabs(zrecoEx)>0 ? trackSeg.exitMomentum().x()/zrecoEx : 0 );
      const double tyrecoEx = ( fabs(zrecoEx)>0 ? trackSeg.exitMomentum().y()/zrecoEx : 0 );
      const double zmcEx    = mcSegment->exitMomentum().z();
      const double txmcEx   = ( fabs(zmcEx)>0 ? mcSegment->exitMomentum().x()/zmcEx : 0 );
      const double tymcEx   = ( fabs(zmcEx)>0 ? mcSegment->exitMomentum().y()/zmcEx : 0 );
      m_txResExit[iRad]->fill( txrecoEx - txmcEx );
      m_tyResExit[iRad]->fill( tyrecoEx - tymcEx );

      // x and y pull distributions
      m_xPullEntry[iRad]->fill( trackSeg.entryErrors().errX()>0 ?
                                (trackSeg.entryPoint().x()-mcSegment->entryPoint().x())/trackSeg.entryErrors().errX() : -999 );
      m_yPullEntry[iRad]->fill( trackSeg.entryErrors().errY()>0 ?
                                (trackSeg.entryPoint().y()-mcSegment->entryPoint().y())/trackSeg.entryErrors().errY() : -999 );
      m_xPullExit[iRad]->fill(  trackSeg.exitErrors().errX()>0 ?
                                (trackSeg.exitPoint().x()-mcSegment->exitPoint().x())/trackSeg.exitErrors().errX() : -999 );
      m_yPullExit[iRad]->fill(  trackSeg.exitErrors().errY()>0 ?
                                (trackSeg.exitPoint().y()-mcSegment->exitPoint().y())/trackSeg.exitErrors().errY() : -999 );

      // tx and ty pull distributions
      m_txPullEntry[iRad]->fill( trackSeg.entryErrors().errTX()>0 ? (txrecoEnt-txmcEnt)/trackSeg.entryErrors().errTX() : -999 );
      m_tyPullEntry[iRad]->fill( trackSeg.entryErrors().errTY()>0 ? (tyrecoEnt-tymcEnt)/trackSeg.entryErrors().errTY() : -999 );
      m_txPullExit[iRad]->fill(  trackSeg.exitErrors().errTX()>0 ? (txrecoEx-txmcEx)/trackSeg.exitErrors().errTX() : -999 );
      m_tyPullExit[iRad]->fill(  trackSeg.exitErrors().errTY()>0 ? (tyrecoEx-tymcEx)/trackSeg.exitErrors().errTY() : -999 );

      // momentum pulls
      m_pPullEntry[iRad]->fill( trackSeg.entryErrors().errP()>0 ?
                                (trackSeg.entryMomentum().mag()-mcSegment->entryMomentum().mag())/trackSeg.entryErrors().errP() : -999 );
      m_pPullExit[iRad]->fill(  trackSeg.exitErrors().errP()>0 ?
                                (trackSeg.exitMomentum().mag()-mcSegment->exitMomentum().mag())/trackSeg.exitErrors().errP() : -999 );

      // Angle between entry and exit directions
      const double mcInOutAng = mcSegment->entryMomentum().angle( mcSegment->exitMomentum() );
      m_mcAngInOut[iRad]->fill( mcInOutAng );
      m_angInOutRes[iRad]->fill( recoInOutAng - mcInOutAng );

    }

  } // end segment loop

  // Fill final plots
  for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad ) {
    m_nSegsRad[iRad]->fill( nSegs[iRad] );
    m_segWithMc[iRad]->fill( 0 == nSegs[iRad] ? 0 :
                             static_cast<float>(nMCSegs[iRad])/static_cast<float>(nSegs[iRad]) );
  }

  return StatusCode::SUCCESS;
};

//  Finalize
StatusCode RichTrackResolutionMoni::finalize()
{
  // Execute base class method
  return RichRecAlgBase::finalize();
}
