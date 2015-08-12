#include "STDet/DeSTDetector.h"

#include <boost/foreach.hpp>

#include "PatVeloTTFit.h"
#include "PatVeloTTTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatVeloTTFit
//
// 2009-01-28 : Pavel Krokovny <krokovny@physi.uni-heidelberg.de>
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PatVeloTTFit )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatVeloTTFit::PatVeloTTFit(const std::string& type,
                       const std::string& name,
                       const IInterface* parent)
  : GaudiTool(type, name, parent)
  , m_patVTTTool(0)
  , m_ttDet(0)
  , m_PatTTMagnetTool(0)
{
  declareInterface<IPatVeloTTFit>(this);
}

//=============================================================================
// Destructor
//=============================================================================
PatVeloTTFit::~PatVeloTTFit() {;}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatVeloTTFit::initialize()
{
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if (sc.isFailure()) return sc;  // error printed already by GaudiTool

  m_patVTTTool = tool<PatVeloTTTool>("PatVeloTTTool");
  m_PatTTMagnetTool = tool<PatTTMagnetTool>( "PatTTMagnetTool","PatTTMagnetTool");

  m_ttDet = getDet<DeSTDetector>(DeSTDetLocation::TT);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode PatVeloTTFit::finalize()
{
  return GaudiTool::finalize();  // must be called after all other actions
}

/// fit a PatVTTTrack
StatusCode PatVeloTTFit::fitVTT( LHCb::Track& track ) const
{
  LHCb::Track* veloTr = &track;
  double slY = veloTr->stateAt(LHCb::State::EndVelo)->ty();
  double slX = veloTr->stateAt(LHCb::State::EndVelo)->tx();

  // The candidate based on the Velo track
  PatVTTTrack cand( veloTr );

  std::vector<float> normFact;
  m_PatTTMagnetTool->dxNormFactorsTT( slY,  normFact);

  //--------------------------------------------------------------------------
  // Loop on regions
  //--------------------------------------------------------------------------
  LHCb::STLiteCluster::FastContainer*  m_stLiteContainer =
    get<LHCb::STLiteCluster::FastContainer>(LHCb::STLiteClusterLocation::TTClusters);

  // loop over LHCbIDs, extract T-station measurements
  std::vector< PatTTHit* > hits ;
  std::vector< Tf::STHit* > sthits ;
  
  std::vector<LHCb::LHCbID> lhcbIDs = veloTr->lhcbIDs();
  for( std::vector<LHCb::LHCbID>::const_iterator ihit = lhcbIDs.begin(); 
       ihit != lhcbIDs.end(); ++ihit ) { 
    if(  ihit->detectorType()==LHCb::LHCbID::TT ) {
      LHCb::STChannelID stChan = ihit->stID() ;
      const DeSTSector* stSector = m_ttDet->findSector( stChan );
      if(stSector==0) {
        return Error( "No sector found for TT hit!" );
      }

      LHCb::STLiteCluster::FastContainer::const_iterator iclus =  
	m_stLiteContainer->find< LHCb::STLiteCluster::findPolicy >( stChan ) ;

      if( iclus != m_stLiteContainer->end() ) {
	Tf::STHit* sthit = new Tf::STHit(*stSector, *iclus ) ;
	PatTTHit* pattthit = new PatTTHit( *sthit );
	sthits.push_back( sthit ) ;
	hits.push_back( pattthit ) ; 
	
	double xOnTrack = cand.xAtZ( pattthit->z() );
	//double yOnTrack = cand.yAtZ( pattthit->z() );
	double yAt0 = cand.yAtZ( 0 );
	
	double dyDz = cand.slopeY();
	
	updateTTHitForTrack(pattthit,yAt0, dyDz);
	double dx = pattthit->x() - xOnTrack;
	
	// Scale to the reference reg	
	dx = dx * normFact[pattthit->planeCode()];
	cand.storeHit( dx, pattthit );
	
      } else {
        return Error( "Cannot find lite cluster!" );
      }
    }
  }

  cand.storeClusters();

  m_patVTTTool->simpleFit(cand);
  
  double qOverP = cand.qOverP();
  
  double zMidTT    = m_PatTTMagnetTool->zMidTT();
  double zMidField = m_PatTTMagnetTool->zMidField();

  LHCb::State temp;
  temp.setLocation( LHCb::State::AtTT );
  temp.setState( cand.xAtZ( zMidTT ) + cand.Dx(),
		 cand.yAtZ( zMidTT ),
		 zMidTT,
		 slX + cand.Dx()/(zMidTT - zMidField),
		 slY,
		 qOverP);

  track.addToStates( temp );

  LHCb::Track::StateContainer::const_iterator istate;
  for( istate = track.states().begin(); istate != track.states().end(); ++istate)
    (const_cast<LHCb::State*>(*istate))->setQOverP( qOverP ) ;


  
  BOOST_FOREACH( Tf::STHit* ihit, sthits ) delete ihit ;  
  BOOST_FOREACH( PatTTHit* ihit, hits ) delete ihit ; 

  return StatusCode::SUCCESS;
}

