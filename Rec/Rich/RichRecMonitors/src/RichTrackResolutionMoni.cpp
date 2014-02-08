
//---------------------------------------------------------------------------
/** @file RichTrackResolutionMoni.cpp
 *
 *  Implementation file for algorithm class : RichTrackResolutionMoni
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichTrackResolutionMoni.h"

// From Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// namespace
using namespace Rich::Rec::MC;

//---------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( TrackResolutionMoni )

// Standard constructor, initializes variables
TrackResolutionMoni::TrackResolutionMoni( const std::string& name,
                                          ISvcLocator* pSvcLocator )
  : Rich::Rec::TupleAlgBase ( name, pSvcLocator ),
    m_richRecMCTruth    ( NULL ),
    m_mcTkInfo          ( NULL ),
    m_trSelector        ( NULL )
{
  // job opts
}

// Destructor
TrackResolutionMoni::~TrackResolutionMoni() {}

//  Initialize
StatusCode TrackResolutionMoni::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = TupleAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool",  m_richRecMCTruth   );
  acquireTool( "RichMCTrackInfoTool", m_mcTkInfo         );
  acquireTool( "TrackSelector",       m_trSelector, this );

  return sc;
}

// Main execution
StatusCode TrackResolutionMoni::execute()
{

  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Make sure all tracks and segments have been formed
  if ( richTracks()->empty() )
  {
    if ( !trackCreator()->newTracks() )
      return Error( "Problem creating RichRecTracks" );
    if ( msgLevel(MSG::DEBUG) )
      debug() << "No tracks found : Created " << richTracks()->size()
              << " RichRecTracks " << richSegments()->size()
              << " RichRecSegments" << endmsg;
  }

  // Tally number of segments
  unsigned int nSegs[Rich::NRadiatorTypes]   = { 0, 0, 0 };
  unsigned int nMCSegs[Rich::NRadiatorTypes] = { 0, 0, 0 };

  // min and max P for histos
  const double maxP = m_trSelector->maxPCut() * Gaudi::Units::GeV;
  const double minP = m_trSelector->minPCut() * Gaudi::Units::GeV;

  // Histogramming
  const Rich::HistoID hid;

  // Iterate over segments
  for ( auto segment : *(richSegments()) )
  {

    // try block for rare exceptions from MCRichSegment
    try
    {

      // apply track selection
      if ( !m_trSelector->trackSelected( segment->richRecTrack() ) ) continue;

      // True type ( skip electrons )
      const Rich::ParticleIDType mcType = m_richRecMCTruth->mcParticleType( segment );
      if ( Rich::Electron == mcType || Rich::Unknown == mcType ) continue;

      // shortcuts
      // track segment
      const LHCb::RichTrackSegment & tkSeg = segment->trackSegment();
      // entry/exit points
      const Gaudi::XYZPoint & entP  = tkSeg.entryPoint();
      const Gaudi::XYZPoint & extP  = tkSeg.exitPoint();
      // entry exit momentum vectors
      const Gaudi::XYZVector & entV = tkSeg.entryMomentum();
      const Gaudi::XYZVector & extV = tkSeg.exitMomentum();

      //const Rich::DetectorType iRich = tkSeg.rich();    // which rich detector
      const Rich::RadiatorType rad = tkSeg.radiator();   // which radiator
      ++nSegs[rad]; // count segments per radiator

      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "Selected " << segment->richRecTrack()->trackID().trackType()
                  << " RichRecSegment in " << rad << endmsg;

      // Ray traced hit point on PDPanel
      const Gaudi::XYZPoint & pdPoint = segment->pdPanelHitPoint();

      // Angle between entry and exit vectors
      const double recoInOutAng = Rich::Geom::AngleBetween( entV, extV );
      plot1D( recoInOutAng, hid(rad,"recoInOutAng"), "Reco. entry/exit angle",0,0.01);

      // Get associated RichMCSegment and MCParticle
      const LHCb::MCRichSegment * mcSeg = m_richRecMCTruth->mcRichSegment(segment);
      if ( mcSeg )
      {
        ++nMCSegs[rad]; // count MC segments per radiator

        const LHCb::MCParticle * mcPart = m_richRecMCTruth->mcParticle(segment);

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
        plot1D( txrecoEnt-txmcEnt, hid(rad,"txResEnt"), "Rec-MC tx entry", -0.01,0.01 );
        plot1D( tyrecoEnt-tymcEnt, hid(rad,"tyResEnt"), "Rec-MC ty entry", -0.01,0.01 );
        const double zrecoEx  = extV.z();
        const double txrecoEx = ( fabs(zrecoEx)>0 ? extV.x()/zrecoEx : 0 );
        const double tyrecoEx = ( fabs(zrecoEx)>0 ? extV.y()/zrecoEx : 0 );
        const double zmcEx    = mcExtV.z();
        const double txmcEx   = ( fabs(zmcEx)>0 ? mcExtV.x()/zmcEx : 0 );
        const double tymcEx   = ( fabs(zmcEx)>0 ? mcExtV.y()/zmcEx : 0 );
        plot1D( txrecoEx-txmcEx, hid(rad,"txResExt"), "Rec-MC tx exit", -0.01,0.01 );
        plot1D( tyrecoEx-tymcEx, hid(rad,"tyResExt"), "Rec-MC ty exit", -0.01,0.01 );

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
        profile1D( sqrt(entV.Mag2()), pullXEnt, hid(rad,"pullXEntVP"), "Entry x pull versus momentum", minP, maxP, 50 );
        plot1D   ( pullYEnt, hid(rad,"pullYEnt"), "Entry y pull", -5,5 );
        profile1D( sqrt(entV.Mag2()), pullYEnt, hid(rad,"pullYEntVP"), "Entry y pull versus momentum", minP, maxP, 50 );
        plot1D   ( pullXExt, hid(rad,"pullXExt"), "Exit x pull",  -5,5 );
        profile1D( sqrt(extV.Mag2()), pullXExt, hid(rad,"pullXExtVP"), "Exit x pull versus momentum", minP, maxP, 50 );
        plot1D   ( pullYExt, hid(rad,"pullYExt"), "Exit y pull",  -5,5 );
        profile1D( sqrt(extV.Mag2()), pullYExt, hid(rad,"pullYExtVP"), "Exit y pull versus momentum", minP, maxP, 50 );

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
        profile1D( sqrt(entV.Mag2()), pullTXEnt, hid(rad,"pullTXEntVP"), "Entry tx pull versus momentum", minP, maxP, 50 );
        plot1D   ( pullTYEnt, hid(rad,"pullTYEnt"), "Entry ty pull", -5,5 );
        profile1D( sqrt(entV.Mag2()), pullTYEnt, hid(rad,"pullTYEntVP"), "Entry ty pull versus momentum", minP, maxP, 50 );
        plot1D   ( pullTXExt, hid(rad,"pullTXExt"), "Exit tx pull", -5,5 );
        profile1D( sqrt(extV.Mag2()), pullTXExt, hid(rad,"pullTXExtVP"), "Exit tx pull versus momentum", minP, maxP, 50 );
        plot1D   ( pullTYExt, hid(rad,"pullTYExt"), "Exit ty pull", -5,5 );
        profile1D( sqrt(extV.Mag2()), pullTYExt, hid(rad,"pullTYExtVP"), "Exit ty pull versus momentum", minP, maxP, 50 );

        // momentum resolutions
        const double recPvert = segment->richRecTrack()->vertexMomentum();
        const double mcPvert  = mcPart->p();
        const double pEntry   = sqrt(entV.Mag2());
        const double pExit    = sqrt(extV.Mag2());
        const double pMcEntry = sqrt(mcEntV.Mag2());
        const double pMcExit  = sqrt(mcExtV.Mag2());
        const double pullPEnt = ( tkSeg.entryErrors().errP()>0 ?
                                  (pEntry-pMcEntry)/tkSeg.entryErrors().errP() : -999 );
        const double pullPExt = ( tkSeg.exitErrors().errP()>0 ?
                                  (pExit-pMcExit)/tkSeg.exitErrors().errP() : -999 );

        plot1D( recPvert-mcPvert, hid(rad,"dTrVertPtot"), "Rec-MC at vertex Ptot", -1*Gaudi::Units::GeV,1*Gaudi::Units::GeV );
        plot1D( pEntry-pMcEntry, hid(rad,"dTrEntPtot"), "Rec-MC rad entry Ptot", -1*Gaudi::Units::GeV,1*Gaudi::Units::GeV );
        plot1D( pExit-pMcExit, hid(rad,"dTrExtPtot"), "Rec-MC rad exit Ptot", -1*Gaudi::Units::GeV,1*Gaudi::Units::GeV );
        plot1D( pullPEnt, hid(rad,"pullPEnt"), "Entry P pull", -5,5 );
        profile1D( pEntry, pullPEnt, hid(rad,"pullPEntVP"), "Entry P pull versus P", minP, maxP, 50 );
        plot1D( pullPExt, hid(rad,"pullPExt"), "Exit P pull",  -5,5 );
        profile1D( pExit, pullPExt, hid(rad,"pullPExtVP"), "Exit P pull versus P", minP, maxP, 50 );
        // dp/p versus p
        const double dpOvPVert  = ( recPvert>0 ? fabs(recPvert-mcPvert)/recPvert : 0 );
        profile1D( recPvert, dpOvPVert, hid(rad,"dPOvPVPVertex"), "Vertex dP/P Versus P", minP, maxP, 50 );
        const double dpOvPEntry = ( pEntry>0 ? fabs(pEntry-pMcEntry)/pEntry : 0 );
        profile1D( pEntry, dpOvPEntry, hid(rad,"dPOvPVPEnt"), "Rad Entry dP/P Versus P", minP, maxP, 50 );
        const double dpOvPExit  = ( pExit>0 ? fabs(pExit-pMcExit)/pExit : 0 );
        profile1D( pExit, dpOvPExit, hid(rad,"dPOvPVPExt"), "Rad Exit dP/P Versus P", minP, maxP, 50 );

        // Angle between entry and exit directions
        const double mcInOutAng = Rich::Geom::AngleBetween( mcEntV, mcExtV );
        plot1D( mcInOutAng, hid(rad,"mcInOutAng"), "MC entry/exit angle", 0,0.01 );
        plot1D( recoInOutAng-mcInOutAng, hid(rad,"InOutAngRes"), "Reco-MC entry/exit angle", -0.01,0.01 );

        // finally, write a tuple with all this info ...

        StatusCode sc = StatusCode::SUCCESS;

        Tuple tuple = nTuple( hid(rad,"tkResTuple"), "TrackGeomTuple" );

        // entry point
        sc = sc && tuple->column( "RecoXEntry" , entP.x() );
        sc = sc && tuple->column( "RecoYEntry" , entP.y() );
        sc = sc && tuple->column( "RecoZEntry" , entP.z() );
        sc = sc && tuple->column( "McXEntry" , mcEntP.x() );
        sc = sc && tuple->column( "McYEntry" , mcEntP.y() );
        sc = sc && tuple->column( "McZEntry" , mcEntP.z() );

        //exit point
        sc = sc && tuple->column( "RecoXExit" , extP.x() );
        sc = sc && tuple->column( "RecoYExit" , extP.y() );
        sc = sc && tuple->column( "RecoZExit" , extP.z() );
        sc = sc && tuple->column( "McXExit" , mcExtP.x() );
        sc = sc && tuple->column( "McYExit" , mcExtP.y() );
        sc = sc && tuple->column( "McZExit" , mcExtP.z() );

        // entry slopes
        sc = sc && tuple->column( "RecoTXEntry" , entV.x() );
        sc = sc && tuple->column( "RecoTYEntry" , entV.y() );
        sc = sc && tuple->column( "McTXEntry" , mcEntV.x() );
        sc = sc && tuple->column( "McTYEntry" , mcEntV.y() );

        // exit slopes
        sc = sc && tuple->column( "RecoTXExit" , extV.x() );
        sc = sc && tuple->column( "RecoTYExit" , extV.y() );
        sc = sc && tuple->column( "McTXExit" , mcExtV.x() );
        sc = sc && tuple->column( "McTYExit" , mcExtV.y() );

        // momentum
        sc = sc && tuple->column( "RecoVertPtot", recPvert );
        sc = sc && tuple->column( "RecoPtotEntry", pEntry );
        sc = sc && tuple->column( "RecoPtotExit",  pExit );
        sc = sc && tuple->column( "McVertPtot",   mcPvert  );
        sc = sc && tuple->column( "McPtotEntry", pMcEntry );
        sc = sc && tuple->column( "McPtotExit",  pMcExit );
        sc = sc && tuple->column( "EntryPerr", tkSeg.entryErrors().errP() );
        sc = sc && tuple->column( "ExitPerr",  tkSeg.exitErrors().errP()  );

        sc = sc && tuple->write();

        if ( sc.isFailure() ) return sc;

      } // mc segment exists

    }
    catch ( const GaudiException & expt )
    {
      Error( expt.what() ).ignore();
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
