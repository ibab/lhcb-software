#include "STDet/DeSTDetector.h"

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
{
  declareInterface<IPatVeloTTFit>(this);
}

//=============================================================================
// Destructor
//=============================================================================
PatVeloTTFit::~PatVeloTTFit() = default;

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

/// fit a PatVTTTrack
StatusCode PatVeloTTFit::fitVTT( LHCb::Track& track ) const
{
  double slY = track.stateAt(LHCb::State::EndVelo)->ty();
  double slX = track.stateAt(LHCb::State::EndVelo)->tx();

  // The candidate based on the Velo track
  PatVTTTrack cand( &track );

  std::vector<float> normFact;
  m_PatTTMagnetTool->dxNormFactorsTT( slY,  normFact);

  //--------------------------------------------------------------------------
  // Loop on regions
  //--------------------------------------------------------------------------
  LHCb::STLiteCluster::FastContainer*  m_stLiteContainer =
    get<LHCb::STLiteCluster::FastContainer>(LHCb::STLiteClusterLocation::TTClusters);

  // loop over LHCbIDs, extract T-station measurements
  std::vector< std::unique_ptr<PatTTHit> > hits ;
  std::vector< std::unique_ptr<Tf::STHit> > sthits ;

  for( const auto& id : track.lhcbIDs() ) {
    if(  id.detectorType()!=LHCb::LHCbID::TT ) continue ;
    LHCb::STChannelID stChan = id.stID() ;
    const DeSTSector* stSector = m_ttDet->findSector( stChan );
    if(!stSector) return Error( "No sector found for TT hit!" );

    auto iclus =  m_stLiteContainer->find< LHCb::STLiteCluster::findPolicy >( stChan ) ;
    if( iclus == m_stLiteContainer->end() ) {
      return Error( "Cannot find lite cluster!" );
    }
    sthits.emplace_back( new Tf::STHit(*stSector, *iclus )) ;
    Tf::STHit* sthit = sthits.back().get();
    hits.emplace_back( new PatTTHit( *sthit ) );
    PatTTHit* pattthit = hits.back().get();

    double xOnTrack = cand.xAtZ( pattthit->z() );
    //double yOnTrack = cand.yAtZ( pattthit->z() );
    double yAt0 = cand.yAtZ( 0 );

    double dyDz = cand.slopeY();

    updateTTHitForTrack(pattthit,yAt0, dyDz);
    double dx = pattthit->x() - xOnTrack;

    // Scale to the reference reg	
    dx *= normFact[pattthit->planeCode()];
    cand.storeHit( dx, pattthit );
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

  for(auto& state : track.states())
    (const_cast<LHCb::State*>(state))->setQOverP( qOverP ) ;

  return StatusCode::SUCCESS;
}
