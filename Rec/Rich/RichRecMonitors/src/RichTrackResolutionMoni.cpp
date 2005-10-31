
//---------------------------------------------------------------------------
/** @file RichTrackResolutionMoni.cpp
 *
 *  Implementation file for algorithm class : RichTrackResolutionMoni
 *
 *  CVS Log :-
 *  $Id: RichTrackResolutionMoni.cpp,v 1.3 2005-10-31 13:30:58 jonrob Exp $
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
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_richRecMCTruth    ( 0 ),
    m_mcTkInfo          ( 0 )
{
  // track selector
  declareProperty( "TrackSelection", m_trSelector.selectedTrackTypes() );
}

// Destructor
RichTrackResolutionMoni::~RichTrackResolutionMoni() {};

//  Initialize
StatusCode RichTrackResolutionMoni::initialize() 
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool",   m_richRecMCTruth );
  acquireTool( "RichMCTrackInfoTool",  m_mcTkInfo       );

  // Configure track selector
  if ( !m_trSelector.configureTrackTypes() ) return StatusCode::FAILURE;
  m_trSelector.printTrackSelection( info() );

  return sc;
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
  plot2D( trackCreator()->nInputTracks(), richTracks()->size(),
          "nTks", "# RICH tracks V total # tracks", 0,1000,0,300 );

  // Tally number of segments
  unsigned int nSegs[Rich::NRadiatorTypes]   = { 0, 0, 0 };
  unsigned int nMCSegs[Rich::NRadiatorTypes] = { 0, 0, 0 };

  // Histogramming
  const RichHistoID hid;

  // Iterate over segments
  for ( RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg ) 
  {
    RichRecSegment * segment = *iSeg;

    // apply track selection
    if ( !m_trSelector.trackSelected( segment->richRecTrack() ) ) continue;

    // True type ( skip electrons )
    const Rich::ParticleIDType mcType = m_richRecMCTruth->mcParticleType( segment );
    if ( Rich::Electron == mcType || Rich::Unknown == mcType ) continue;

    // track segment
    const RichTrackSegment & tkSeg = segment->trackSegment();
    // entry/exit points
    const HepPoint3D & entP = tkSeg.entryPoint();
    const HepPoint3D & extP = tkSeg.exitPoint();

    //const Rich::DetectorType iRich = tkSeg.rich();    // which rich detector
    const Rich::RadiatorType iRad = tkSeg.radiator();   // which radiator
    ++nSegs[iRad]; // count segments per radiator

    verbose() << "Selected " << segment->richRecTrack()->trackID().trackType()
              << " RichRecSegment in " << iRad << endreq;

    // Ray traced hit point on PDPanel
    const HepPoint3D & pdPoint = segment->pdPanelHitPoint();

    // Angle between entry and exit vectors
    const double recoInOutAng = tkSeg.entryMomentum().angle(tkSeg.exitMomentum());
    plot1D( recoInOutAng, hid(iRad,"recoInOutAng"), "Reco. entry/exit angle",0,0.01);

    // Get associated RichMCSegment
    const MCRichSegment * mcSeg = m_richRecMCTruth->mcRichSegment(segment);
    if ( mcSeg ) 
    {
      ++nMCSegs[iRad]; // count MC segments per radiator

      // MC entry and exit points
      const HepPoint3D & mcEntP = mcSeg->entryPoint();
      const HepPoint3D & mcExtP = mcSeg->exitPoint();

      // number of G4 segments
      plot1D( mcSeg->trajectoryPoints().size(), 
              hid(iRad,"nMCTrajPtns"), "# G4 segments", -0.5,100.5,101 );

      // position resolution plots
      plot1D( entP.x()-mcEntP.x(), hid(iRad,"dTrEntX"), "Rec-MC rad entry X", -5,5 );
      plot1D( entP.y()-mcEntP.y(), hid(iRad,"dTrEntY"), "Rec-MC rad entry Y", -5,5 );
      plot1D( entP.z()-mcEntP.z(), hid(iRad,"dTrEntZ"), "Rec-MC rad entry Z", -5,5 );
      plot1D( extP.x()-mcExtP.x(), hid(iRad,"dTrExtX"), "Rec-MC rad exit X",  -5,5 );
      plot1D( extP.y()-mcExtP.y(), hid(iRad,"dTrExtY"), "Rec-MC rad exit Y",  -5,5 );
      plot1D( extP.z()-mcExtP.z(), hid(iRad,"dTrExtZ"), "Rec-MC rad exit Z",  -5,5 );
      plot1D( tkSeg.pathLength() - mcSeg->pathLength(),
              hid(iRad,"dTrPathL"), "Rec-MC rad pathLength", -5,5 );

      // Panel ray-traced point
      HepPoint3D mcGlobal;
      if ( m_mcTkInfo->panelIntersectGlobal( mcSeg, mcGlobal ) ) 
      {
        plot1D( pdPoint.x()-mcGlobal.x(), hid(iRad,"dTrPanX"), "Rec-MC panel X", -5,5 );
        plot1D( pdPoint.y()-mcGlobal.y(), hid(iRad,"dTrPanY"), "Rec-MC panel Y", -5,5 );
        plot1D( pdPoint.z()-mcGlobal.z(), hid(iRad,"dTrPanZ"), "Rec-MC panel Z", -5,5 );
      }

      // angle between mc and reco vectors
      plot1D( tkSeg.entryMomentum().angle( mcSeg->entryMomentum() ),
              hid(iRad,"dTrAngEnt"), "Angle between Rec/MC dir at entry", 0,0.01 );
      plot1D( tkSeg.bestMomentum().angle( mcSeg->bestMomentum(0.5) ),
              hid(iRad,"dTrAngExt"), "Angle between Rec/MC dir at exit", 0,0.01 );
      plot1D( tkSeg.exitMomentum().angle( mcSeg->exitMomentum() ),
              hid(iRad,"dTrAngMid"), "Angle between Rec/MC dir at mid point", 0,0.01 );

      // direction resolutions
      const double zrecoEnt  = tkSeg.entryMomentum().z();
      const double txrecoEnt = ( fabs(zrecoEnt)>0 ? tkSeg.entryMomentum().x()/zrecoEnt : 0 );
      const double tyrecoEnt = ( fabs(zrecoEnt)>0 ? tkSeg.entryMomentum().y()/zrecoEnt : 0 );
      const double zmcEnt    = mcSeg->entryMomentum().z();
      const double txmcEnt   = ( fabs(zmcEnt)>0 ? mcSeg->entryMomentum().x()/zmcEnt : 0 );
      const double tymcEnt   = ( fabs(zmcEnt)>0 ? mcSeg->entryMomentum().y()/zmcEnt : 0 );
      plot1D( txrecoEnt-txmcEnt, hid(iRad,"txRexEnt"), "Rec-MC tx entry", -0.01,0.01 );
      plot1D( tyrecoEnt-tymcEnt, hid(iRad,"tyRexEnt"), "Rec-MC ty entry", -0.01,0.01 );
      const double zrecoEx  = tkSeg.exitMomentum().z();
      const double txrecoEx = ( fabs(zrecoEx)>0 ? tkSeg.exitMomentum().x()/zrecoEx : 0 );
      const double tyrecoEx = ( fabs(zrecoEx)>0 ? tkSeg.exitMomentum().y()/zrecoEx : 0 );
      const double zmcEx    = mcSeg->exitMomentum().z();
      const double txmcEx   = ( fabs(zmcEx)>0 ? mcSeg->exitMomentum().x()/zmcEx : 0 );
      const double tymcEx   = ( fabs(zmcEx)>0 ? mcSeg->exitMomentum().y()/zmcEx : 0 );
      plot1D( txrecoEx-txmcEx, hid(iRad,"txRexExt"), "Rec-MC tx exit", -0.01,0.01 );
      plot1D( tyrecoEx-tymcEx, hid(iRad,"tyRexExt"), "Rec-MC ty exit", -0.01,0.01 );

      // x and y pull distributions
      const double pullXEnt = ( tkSeg.entryErrors().errX()>0 ?
                                (entP.x()-mcEntP.x())/tkSeg.entryErrors().errX() : -999 );
      const double pullYEnt = ( tkSeg.entryErrors().errY()>0 ?
                                (entP.y()-mcEntP.y())/tkSeg.entryErrors().errY() : -999 );
      const double pullXExt = ( tkSeg.exitErrors().errX()>0 ?
                                (extP.x()-mcExtP.x())/tkSeg.exitErrors().errX() : -999 );
      const double pullYExt = ( tkSeg.exitErrors().errY()>0 ?
                                (extP.y()-mcExtP.y())/tkSeg.exitErrors().errY() : -999 );
      plot1D( pullXEnt, hid(iRad,"pullXEnt"), "Entry x pull", -5,5 );
      plot1D( pullYEnt, hid(iRad,"pullYEnt"), "Entry y pull", -5,5 );
      plot1D( pullXExt, hid(iRad,"pullXExt"), "Exit x pull",  -5,5 );
      plot1D( pullYExt, hid(iRad,"pullYExt"), "Exit y pull",  -5,5 );

      // tx and ty pull distributions
      const double pullTXEnt = ( tkSeg.entryErrors().errTX()>0 ? (txrecoEnt-txmcEnt)/tkSeg.entryErrors().errTX() : -999 );
      const double pullTYEnt = ( tkSeg.entryErrors().errTY()>0 ? (tyrecoEnt-tymcEnt)/tkSeg.entryErrors().errTY() : -999 );
      const double pullTXExt = ( tkSeg.exitErrors().errTX()>0 ? (txrecoEx-txmcEx)/tkSeg.exitErrors().errTX() : -999 );
      const double pullTYExt = ( tkSeg.exitErrors().errTY()>0 ? (tyrecoEx-tymcEx)/tkSeg.exitErrors().errTY() : -999 );
      plot1D( pullTXEnt, hid(iRad,"pullTXEnt"), "Entry tx pull", -5,5 );
      plot1D( pullTYEnt, hid(iRad,"pullTYEnt"), "Entry ty pull", -5,5 );
      plot1D( pullTXExt, hid(iRad,"pullTXExt"), "Exit tx pull", -5,5 );
      plot1D( pullTYExt, hid(iRad,"pullTYExt"), "Exit ty pull", -5,5 );

      // momentum pulls
      const double pullPEnt = ( tkSeg.entryErrors().errP()>0 ?
                                (tkSeg.entryMomentum().mag()-mcSeg->entryMomentum().mag())/tkSeg.entryErrors().errP() : -999 );
      const double pullPExt = ( tkSeg.exitErrors().errP()>0 ?
                                (tkSeg.exitMomentum().mag()-mcSeg->exitMomentum().mag())/tkSeg.exitErrors().errP() : -999 );
      plot1D( pullPEnt, hid(iRad,"pullPEnt"), "Entry P pull", -5,5 );
      plot1D( pullPExt, hid(iRad,"pullPExt"), "Exit P pull", -5,5 );

      // Angle between entry and exit directions
      const double mcInOutAng = mcSeg->entryMomentum().angle( mcSeg->exitMomentum() );
      plot1D( mcInOutAng, hid(iRad,"mcInOutAng"), "MC entry/exit angle", 0,0.01 );
      plot1D( recoInOutAng-mcInOutAng, hid(iRad,"InOutAngRes"), "Reco-MC entry/exit angle", -0.01,0.01);

    }

  } // end segment loop

  // Fill final plots
  for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad ) 
  {
    const Rich::RadiatorType rad = (Rich::RadiatorType)iRad;
    plot1D( nSegs[iRad], hid(rad,"nSegs"), "# segments per event", -0.5,100.5,101 );
    plot1D( 0 == nSegs[iRad] ? 0 : static_cast<float>(nMCSegs[iRad])/static_cast<float>(nSegs[iRad]),
            hid(rad,"segFrMC"), "Fraction of segments with MC info", 0,1 );
  }

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichTrackResolutionMoni::finalize()
{
  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}
