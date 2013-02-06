#include "GaudiKernel/ToolFactory.h"
#include "STDet/DeSTDetector.h"

#include <boost/foreach.hpp>

#include "PrVeloUTFit.h"
#include "PrVeloUTTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrVeloUTFit
//
// 2009-01-28 : Pavel Krokovny <krokovny@physi.uni-heidelberg.de>
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PrVeloUTFit )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrVeloUTFit::PrVeloUTFit(const std::string& type,
                       const std::string& name,
                       const IInterface* parent)
  : GaudiTool(type, name, parent)
  , m_patVUTTool(0)
  , m_ttDet(0)
  , m_PrUTMagnetTool(0)
{
  declareInterface<IPrVeloUTFit>(this);
}

//=============================================================================
// Destructor
//=============================================================================
PrVeloUTFit::~PrVeloUTFit() {;}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrVeloUTFit::initialize()
{
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if (sc.isFailure()) return sc;  // error printed already by GaudiTool

  m_patVUTTool = tool<PrVeloUTTool>("PrVeloUTTool");
  m_PrUTMagnetTool = tool<PrUTMagnetTool>( "PrUTMagnetTool","PrUTMagnetTool");

  m_ttDet = getDet<DeSTDetector>(DeSTDetLocation::UT);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode PrVeloUTFit::finalize()
{
  return GaudiTool::finalize();  // must be called after all other actions
}

/// fit a PrVUTTrack
StatusCode PrVeloUTFit::fitVUT( LHCb::Track& track ) const
{
  LHCb::Track* veloTr = &track;
  const LHCb::State& trState = veloTr->hasStateAt(LHCb::State::EndVelo) ?
    *(veloTr->stateAt(LHCb::State::EndVelo)) :
    (veloTr->closestState(LHCb::State::EndVelo)) ;
  double slY = trState.ty();
  double slX = trState.tx();

  // The candidate based on the Velo track
  PrVUTTrack cand( veloTr );

  std::vector<double> normFact;
  m_PrUTMagnetTool->dxNormFactorsUT( slY,  normFact);

  //--------------------------------------------------------------------------
  // Loop on regions
  //--------------------------------------------------------------------------
  LHCb::STLiteCluster::FastContainer*  m_stLiteContainer =
    get<LHCb::STLiteCluster::FastContainer>(LHCb::STLiteClusterLocation::UTClusters);

  // loop over LHCbIDs, extract T-station measurements
  std::vector< PrUTHit* > hits ;
  std::vector< Tf::STHit* > sthits ;
  
  std::vector<LHCb::LHCbID> lhcbIDs = veloTr->lhcbIDs();
  for( std::vector<LHCb::LHCbID>::const_iterator ihit = lhcbIDs.begin(); 
       ihit != lhcbIDs.end(); ++ihit ) { 
    if(  ihit->detectorType()==LHCb::LHCbID::UT ) {
      LHCb::STChannelID stChan = ihit->stID() ;
      const DeSTSector* stSector = m_ttDet->findSector( stChan );
      if(stSector==0) {
        return Error( "No sector found for UT hit!" );
      }

      LHCb::STLiteCluster::FastContainer::const_iterator iclus =  
	m_stLiteContainer->find< LHCb::STLiteCluster::findPolicy >( stChan ) ;

      if( iclus != m_stLiteContainer->end() ) {
	Tf::STHit* sthit = new Tf::STHit(*stSector, *iclus ) ;
	PrUTHit* pruthit = new PrUTHit( *sthit );
	sthits.push_back( sthit ) ;
	hits.push_back( pruthit ) ; 
	
	double xOnTrack = cand.xAtZ( pruthit->z() );
	//double yOnTrack = cand.yAtZ( pruthit->z() );
	double yAt0 = cand.yAtZ( 0 );
	
	double dyDz = cand.slopeY();
	
	updateUTHitForTrack(pruthit,yAt0, dyDz);
	double dx = pruthit->x() - xOnTrack;
	
	// Scale to the reference reg	
	dx = dx * normFact[pruthit->planeCode()];
	cand.storeHit( dx, pruthit );
	
      } else {
        return Error( "Cannot find lite cluster!" );
      }
    }
  }

  cand.storeClusters();

  m_patVUTTool->simpleFit(cand);
  
  double qOverP = cand.qOverP();
  
  double zMidUT    = m_PrUTMagnetTool->zMidUT();
  double zMidField = m_PrUTMagnetTool->zMidField();

  LHCb::State temp;
  temp.setLocation( LHCb::State::AtTT );
  temp.setState( cand.xAtZ( zMidUT ) + cand.Dx(),
		 cand.yAtZ( zMidUT ),
		 zMidUT,
		 slX + cand.Dx()/(zMidUT - zMidField),
		 slY,
		 qOverP);

  track.addToStates( temp );

  LHCb::Track::StateContainer::const_iterator istate;
  for( istate = track.states().begin(); istate != track.states().end(); ++istate)
    (const_cast<LHCb::State*>(*istate))->setQOverP( qOverP ) ;


  
  BOOST_FOREACH( Tf::STHit* ihit, sthits ) delete ihit ;  
  BOOST_FOREACH( PrUTHit* ihit, hits ) delete ihit ; 

  return StatusCode::SUCCESS;
}

