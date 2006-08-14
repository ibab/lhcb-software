
//---------------------------------------------------------------------------
/** @file RichTrackResolutionMoni.cpp
 *
 *  Implementation file for algorithm class : RichTrackResolutionMoni
 *
 *  CVS Log :-
 *  $Id: RichTrackResolutionMoni.cpp,v 1.9 2006-08-14 23:39:49 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichTrackResolutionMoni.h"

// namespace
using namespace LHCb;

//---------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichTrackResolutionMoni>          s_factory ;
const        IAlgFactory& RichTrackResolutionMoniFactory = s_factory ;

// Standard constructor, initializes variables
RichTrackResolutionMoni::RichTrackResolutionMoni( const std::string& name,
                                                  ISvcLocator* pSvcLocator )
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_richRecMCTruth    ( NULL ),
    m_mcTkInfo          ( NULL ),
    m_trSelector        ( NULL )
{
  // job opts
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
  acquireTool( "RichRecMCTruthTool",  m_richRecMCTruth   );
  acquireTool( "RichMCTrackInfoTool", m_mcTkInfo         );
  acquireTool( "TrackSelector",       m_trSelector, this );

  return sc;
}

// Main execution
StatusCode RichTrackResolutionMoni::execute()
{
  debug() << "Execute" << endreq;

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Make sure all tracks and segments have been formed
  if ( richTracks()->empty() )
  {
    if ( !trackCreator()->newTracks() )
      return Error( "Problem creating RichRecTracks" );
    debug() << "No tracks found : Created " << richTracks()->size()
            << " RichRecTracks " << richSegments()->size()
            << " RichRecSegments" << endreq;
  }

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
    if ( !m_trSelector->trackSelected( segment->richRecTrack() ) ) continue;

    // True type ( skip electrons )
    const Rich::ParticleIDType mcType = m_richRecMCTruth->mcParticleType( segment );
    if ( Rich::Electron == mcType || Rich::Unknown == mcType ) continue;

    // shortcuts
    // track segment
    const RichTrackSegment & tkSeg = segment->trackSegment();
    // entry/exit points
    const Gaudi::XYZPoint & entP  = tkSeg.entryPoint();
    const Gaudi::XYZPoint & extP  = tkSeg.exitPoint();
    // entry exit momentum vectors
    const Gaudi::XYZVector & entV = tkSeg.entryMomentum();
    const Gaudi::XYZVector & extV = tkSeg.exitMomentum();

    //const Rich::DetectorType iRich = tkSeg.rich();    // which rich detector
    const Rich::RadiatorType rad = tkSeg.radiator();   // which radiator
    ++nSegs[rad]; // count segments per radiator

    verbose() << "Selected " << segment->richRecTrack()->trackID().trackType()
              << " RichRecSegment in " << rad << endreq;

    // Ray traced hit point on PDPanel
    const Gaudi::XYZPoint & pdPoint = segment->pdPanelHitPoint();

    // Angle between entry and exit vectors
    const double recoInOutAng = Rich::Geom::AngleBetween( entV, extV );
    plot1D( recoInOutAng, hid(rad,"recoInOutAng"), "Reco. entry/exit angle",0,0.01);

    // Get associated RichMCSegment and MCParticle
    const MCRichSegment * mcSeg = m_richRecMCTruth->mcRichSegment(segment);
    if ( mcSeg )
    {
      ++nMCSegs[rad]; // count MC segments per radiator
      
      const MCParticle * mcPart = m_richRecMCTruth->mcParticle(segment);

      // shortcuts
      // MC entry and exit points
      const Gaudi::XYZPoint & mcEntP = mcSeg->entryPoint();
      const Gaudi::XYZPoint & mcExtP = mcSeg->exitPoint();
      // MC entry/exit momentum vectors
      const Gaudi::XYZVector & mcEntV = mcSeg->entryMomentum();
      const Gaudi::XYZVector & mcExtV = mcSeg->exitMomentum();

      // number of G4 segments
      plot1D( mcSeg->trajectoryPoints().size(),
              hid(rad,"nMCTrajPtns"), "# G4 segments", -0.5,100.5,101 );

      // position resolution plots
      plot1D( entP.x()-mcEntP.x(), hid(rad,"dTrEntX"), "Rec-MC rad entry X", -5,5 );
      plot1D( entP.y()-mcEntP.y(), hid(rad,"dTrEntY"), "Rec-MC rad entry Y", -5,5 );
      plot1D( entP.z()-mcEntP.z(), hid(rad,"dTrEntZ"), "Rec-MC rad entry Z", -5,5 );
      plot1D( extP.x()-mcExtP.x(), hid(rad,"dTrExtX"), "Rec-MC rad exit X",  -5,5 );
      plot1D( extP.y()-mcExtP.y(), hid(rad,"dTrExtY"), "Rec-MC rad exit Y",  -5,5 );
      plot1D( extP.z()-mcExtP.z(), hid(rad,"dTrExtZ"), "Rec-MC rad exit Z",  -5,5 );
      plot1D( tkSeg.pathLength() - mcSeg->pathLength(),
              hid(rad,"dTrPathL"), "Rec-MC rad pathLength", -5,5 );

      // Panel ray-traced point
      Gaudi::XYZPoint mcGlobal;
      if ( m_mcTkInfo->panelIntersectGlobal( mcSeg, mcGlobal ) )
      {
        plot1D( pdPoint.x()-mcGlobal.x(), hid(rad,"dTrPanX"), "Rec-MC panel X", -5,5 );
        plot1D( pdPoint.y()-mcGlobal.y(), hid(rad,"dTrPanY"), "Rec-MC panel Y", -5,5 );
        plot1D( pdPoint.z()-mcGlobal.z(), hid(rad,"dTrPanZ"), "Rec-MC panel Z", -5,5 );
      }

      // angle between mc and reco vectors
      plot1D( Rich::Geom::AngleBetween( entV, mcEntV ),
              hid(rad,"dTrAngEnt"), "Angle between Rec/MC dir at entry", 0,0.01 );
      plot1D( Rich::Geom::AngleBetween( tkSeg.bestMomentum(), mcSeg->bestMomentum(0.5) ),
              hid(rad,"dTrAngExt"), "Angle between Rec/MC dir at exit", 0,0.01 );
      plot1D( Rich::Geom::AngleBetween( extV, mcExtV ),
              hid(rad,"dTrAngMid"), "Angle between Rec/MC dir at mid point", 0,0.01 );

      // direction resolutions
      const double zrecoEnt  = entV.z();
      const double txrecoEnt = ( fabs(zrecoEnt)>0 ? entV.x()/zrecoEnt : 0 );
      const double tyrecoEnt = ( fabs(zrecoEnt)>0 ? entV.y()/zrecoEnt : 0 );
      const double zmcEnt    = mcEntV.z();
      const double txmcEnt   = ( fabs(zmcEnt)>0 ? mcEntV.x()/zmcEnt : 0 );
      const double tymcEnt   = ( fabs(zmcEnt)>0 ? mcEntV.y()/zmcEnt : 0 );
      plot1D( txrecoEnt-txmcEnt, hid(rad,"txRexEnt"), "Rec-MC tx entry", -0.01,0.01 );
      plot1D( tyrecoEnt-tymcEnt, hid(rad,"tyRexEnt"), "Rec-MC ty entry", -0.01,0.01 );
      const double zrecoEx  = extV.z();
      const double txrecoEx = ( fabs(zrecoEx)>0 ? extV.x()/zrecoEx : 0 );
      const double tyrecoEx = ( fabs(zrecoEx)>0 ? extV.y()/zrecoEx : 0 );
      const double zmcEx    = mcExtV.z();
      const double txmcEx   = ( fabs(zmcEx)>0 ? mcExtV.x()/zmcEx : 0 );
      const double tymcEx   = ( fabs(zmcEx)>0 ? mcExtV.y()/zmcEx : 0 );
      plot1D( txrecoEx-txmcEx, hid(rad,"txRexExt"), "Rec-MC tx exit", -0.01,0.01 );
      plot1D( tyrecoEx-tymcEx, hid(rad,"tyRexExt"), "Rec-MC ty exit", -0.01,0.01 );

      // x and y pull distributions
      const double pullXEnt = ( tkSeg.entryErrors().errX()>0 ?
                                (entP.x()-mcEntP.x())/tkSeg.entryErrors().errX() : -999 );
      const double pullYEnt = ( tkSeg.entryErrors().errY()>0 ?
                                (entP.y()-mcEntP.y())/tkSeg.entryErrors().errY() : -999 );
      const double pullXExt = ( tkSeg.exitErrors().errX()>0 ?
                                (extP.x()-mcExtP.x())/tkSeg.exitErrors().errX() : -999 );
      const double pullYExt = ( tkSeg.exitErrors().errY()>0 ?
                                (extP.y()-mcExtP.y())/tkSeg.exitErrors().errY() : -999 );
      plot1D   ( pullXEnt, hid(rad,"pullXEnt"), "Entry x pull", -5,5 );
      profile1D( sqrt(entV.Mag2()), pullXEnt, hid(rad,"pullXEntVP"), "Entry x pull versus momentum", 0, 100*GeV, 50 );
      plot1D   ( pullYEnt, hid(rad,"pullYEnt"), "Entry y pull", -5,5 );
      profile1D( sqrt(entV.Mag2()), pullYEnt, hid(rad,"pullYEntVP"), "Entry y pull versus momentum", 0, 100*GeV, 50 );
      plot1D   ( pullXExt, hid(rad,"pullXExt"), "Exit x pull",  -5,5 );
      profile1D( sqrt(extV.Mag2()), pullXExt, hid(rad,"pullXExtVP"), "Exit x pull versus momentum", 0, 100*GeV, 50 );
      plot1D   ( pullYExt, hid(rad,"pullYExt"), "Exit y pull",  -5,5 );
      profile1D( sqrt(extV.Mag2()), pullYExt, hid(rad,"pullYExtVP"), "Exit y pull versus momentum", 0, 100*GeV, 50 );

      // tx and ty pull distributions
      const double pullTXEnt =
        ( tkSeg.entryErrors().errTX()>0 ? (txrecoEnt-txmcEnt)/tkSeg.entryErrors().errTX() : -999 );
      const double pullTYEnt =
        ( tkSeg.entryErrors().errTY()>0 ? (tyrecoEnt-tymcEnt)/tkSeg.entryErrors().errTY() : -999 );
      const double pullTXExt =
        ( tkSeg.exitErrors().errTX()>0 ? (txrecoEx-txmcEx)/tkSeg.exitErrors().errTX() : -999 );
      const double pullTYExt =
        ( tkSeg.exitErrors().errTY()>0 ? (tyrecoEx-tymcEx)/tkSeg.exitErrors().errTY() : -999 );
      plot1D   ( pullTXEnt, hid(rad,"pullTXEnt"), "Entry tx pull", -5,5 );
      profile1D( sqrt(entV.Mag2()), pullTXEnt, hid(rad,"pullTXEntVP"), "Entry tx pull versus momentum", 0, 100*GeV, 50 );
      plot1D   ( pullTYEnt, hid(rad,"pullTYEnt"), "Entry ty pull", -5,5 );
      profile1D( sqrt(entV.Mag2()), pullTYEnt, hid(rad,"pullTYEntVP"), "Entry ty pull versus momentum", 0, 100*GeV, 50 );
      plot1D   ( pullTXExt, hid(rad,"pullTXExt"), "Exit tx pull", -5,5 );
      profile1D( sqrt(extV.Mag2()), pullTXExt, hid(rad,"pullTXExtVP"), "Exit tx pull versus momentum", 0, 100*GeV, 50 );
      plot1D   ( pullTYExt, hid(rad,"pullTYExt"), "Exit ty pull", -5,5 );
      profile1D( sqrt(extV.Mag2()), pullTYExt, hid(rad,"pullTYExtVP"), "Exit ty pull versus momentum", 0, 100*GeV, 50 );

      // momentum resolutions
      const double pullPEnt = ( tkSeg.entryErrors().errP()>0 ?
                                (sqrt(entV.Mag2())-sqrt(mcEntV.Mag2()))/tkSeg.entryErrors().errP() : -999 );
      const double pullPExt = ( tkSeg.exitErrors().errP()>0 ?
                                (sqrt(extV.Mag2())-sqrt(mcExtV.Mag2()))/tkSeg.exitErrors().errP() : -999 );
      plot1D( segment->richRecTrack()->vertexMomentum()-mcPart->p(), hid(rad,"dTrVertPtot"), "Rec-MC rad at vertex Ptot", -1*GeV,1*GeV );
      plot1D( sqrt(entV.Mag2())-sqrt(mcEntV.Mag2()), hid(rad,"dTrEntPtot"), "Rec-MC rad entry Ptot", -1*GeV,1*GeV );
      plot1D( sqrt(extV.Mag2())-sqrt(mcExtV.Mag2()), hid(rad,"dTrEntPtot"), "Rec-MC rad exit Ptot", -1*GeV,1*GeV );
      plot1D( pullPEnt, hid(rad,"pullPEnt"), "Entry P pull", -5,5 );
      profile1D( sqrt(entV.Mag2()), pullPEnt, hid(rad,"pullPEntVP"), "Entry P pull versus P", 0, 100*GeV, 50 );
      plot1D( pullPExt, hid(rad,"pullPExt"), "Exit P pull",  -5,5 );
      profile1D( sqrt(extV.Mag2()), pullPExt, hid(rad,"pullPExtVP"), "Exit P pull versus P", 0, 100*GeV, 50 );

      // Angle between entry and exit directions
      const double mcInOutAng = Rich::Geom::AngleBetween( mcEntV, mcExtV );
      plot1D( mcInOutAng, hid(rad,"mcInOutAng"), "MC entry/exit angle", 0,0.01 );
      plot1D( recoInOutAng-mcInOutAng, hid(rad,"InOutAngRes"), "Reco-MC entry/exit angle", -0.01,0.01 );

    }

  } // end segment loop

  // Fill final plots
  for ( int irad = 0; irad < Rich::NRadiatorTypes; ++irad )
  {
    const Rich::RadiatorType rad = (Rich::RadiatorType)irad;
    plot1D( nSegs[rad], hid(rad,"nSegs"), "# segments per event", -0.5,100.5,101 );
    plot1D( 0 == nSegs[rad] ? 0 : static_cast<float>(nMCSegs[rad])/static_cast<float>(nSegs[rad]),
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
