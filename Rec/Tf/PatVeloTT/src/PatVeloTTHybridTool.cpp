// Include files

// from Gaudi
#include "TfKernel/RecoFuncs.h"

// local
#include "PatVeloTTHybridTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatVeloTTHybridTool
//
// 2007-05-08 : Mariusz Witek
//-----------------------------------------------------------------------------

namespace {

struct  compX  {
  bool operator() (PatTTHit* first, PatTTHit* second ) {
    return first->hit()->xAtYEq0() < second->hit()->xAtYEq0() ;
  }
};
struct  lowerBoundX  {
  bool operator() (const PatTTHit* first, const float value ) const {
    return first->x() < value ;
  }
};

bool acceptTrack(const LHCb::Track& track)
{
  return !track.checkFlag( LHCb::Track::Invalid) 
      && !track.checkFlag( LHCb::Track::Backward);
}

// convert a move-only type into a single element vector...
template <typename T>
auto to_vector(T&& t) -> std::vector<T> {
    std::vector<T> v(1); v[0] = std::forward<T>(t); return v;
}

}


// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PatVeloTTHybridTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatVeloTTHybridTool::PatVeloTTHybridTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
 : base_class( type, name , parent )
{
  declareInterface<ITracksFromTrack>(this);
  
  // Momentum determination
  declareProperty("minMomentum"        , m_minMomentum      = 0*Gaudi::Units::GeV);
  declareProperty("minPT"              , m_minPT            = 0.1*Gaudi::Units::GeV);
  declareProperty("maxPseudoChi2"      , m_maxPseudoChi2    = 1280.);
  // Tolerances for extrapolation
  declareProperty("MaxXSlope"          , m_maxXSlope        = 0.350);
  declareProperty("MaxYSlope"          , m_maxYSlope        = 0.300);
  declareProperty("centralHoleSize"    , m_centralHoleSize  = 33. * Gaudi::Units::mm);
  declareProperty("YTolerance"         , m_yTol             = 0.8  * Gaudi::Units::mm);
  declareProperty("YTolSlope"          , m_yTolSlope        = 0.2);
  // Grouping tolerance
  declareProperty("HitTol1"            , m_hitTol1           = 6.0 * Gaudi::Units::mm);
  declareProperty("HitTol2"            , m_hitTol2           = 0.8 * Gaudi::Units::mm);
  declareProperty("DeltaTx1"           , m_deltaTx1           = 0.035);
  declareProperty("DeltaTx2"           , m_deltaTx2           = 0.02);
  declareProperty("IntraLayerDist"     , m_intraLayerDist   = 15.0 * Gaudi::Units::mm);
  declareProperty("PrintVariables"     , m_printVariables   = false);
  declareProperty("PassTracks"         , m_passTracks   = false);
  declareProperty("PassHoleSize"       , m_passHoleSize  = 40. * Gaudi::Units::mm);
  declareProperty("OverlapTol"         , m_overlapTol  = 0.7 * Gaudi::Units::mm);
  declareProperty("MinHighThreshold"   , m_minHighThres = 1);
  
}
//=============================================================================
// Destructor
//=============================================================================
PatVeloTTHybridTool::~PatVeloTTHybridTool() {}

//=============================================================================
// Main method for VELOTT track reconstruction
//=============================================================================
//=========================================================================
//  Initialisation, check parameters
//=========================================================================
StatusCode PatVeloTTHybridTool::initialize ( ) {
  StatusCode sc = GaudiTool::initialize();
  if ( !sc ) return sc;

  m_PatTTMagnetTool = tool<PatTTMagnetTool>( "PatTTMagnetTool","PatTTMagnetTool");

  // m_zMidTT is a position of normalization plane which should to be close to z middle of TT ( +- 5 cm ).
  // Cashed once in PatVeloTTHybridTool at initialization. No need to update with small TT movement.
  m_zMidTT    = m_PatTTMagnetTool->zMidTT();
  //  zMidField and distToMomentum isproperly recalculated in PatTTMagnetTool when B field changes
  m_distToMomentum = m_PatTTMagnetTool->averageDist2mom();
  
  if(m_printVariables){
    info() << " minMomentum        = " << m_minMomentum      << " MeV" << endmsg;
    info() << " minPT              = " << m_minPT            << " MeV" << endmsg;
    info() << " maxPseudoChi2      = " << m_maxPseudoChi2    << "   "  << endmsg;
    info() << " MaxXSlope          = " << m_maxXSlope    << "   "  << endmsg;
    info() << " MaxYSlope          = " << m_maxYSlope    << "   "  << endmsg;
    info() << " distToMomentum     = " << m_distToMomentum               << endmsg;
    info() << " yTolerance         = " << m_yTol             << " mm"  << endmsg;
    info() << " YTolSlope          = " << m_yTolSlope                  << endmsg;
    info() << " HitTol1            = " << m_hitTol1       << " mm " << endmsg;
    info() << " HitTol2            = " << m_hitTol2       << " mm " << endmsg;
    info() << " DeltaTx1           = " << m_deltaTx1       << "  " << endmsg;
    info() << " DeltaTx2           = " << m_deltaTx2       << "  " << endmsg;
    info() << " zMidTT             = " << m_zMidTT           << " mm"  << endmsg;
    info() << " IntraLayerDist     = " << m_intraLayerDist << " mm "<<endmsg;
    info() << " PassTracks         = " <<m_passTracks <<endmsg;
    info() << " PassHoleSize       = " <<m_passHoleSize << " mm "<<endmsg;
    info() << " OverlapTol         = " << m_overlapTol       << " mm " << endmsg;
  }
  
  m_utHitManager   = tool<Tf::TTStationHitManager <PatTTHit> >("PatTTStationHitManager");

  // -- Event listener
  m_newEvent = true;
  incSvc()->addListener(this, IncidentType::BeginEvent);
  
  m_clusterCandidate.reserve(4);

  m_sigmaVeloSlope = 0.10*Gaudi::Units::mrad;
  m_invSigmaVeloSlope = 1.0/m_sigmaVeloSlope;
  
  m_zKink = 1780.0;

  return StatusCode::SUCCESS;
}


//=========================================================================
// Main reconstruction method
//=========================================================================
StatusCode PatVeloTTHybridTool::tracksFromTrack(const LHCb::Track & velotrack,
                                                std::vector<LHCb::Track*>& out) const
{
  //Check if it is a new event
  if(m_newEvent) initEvent();
  
  //Remove backward/invalid tracks
  if(!acceptTrack(velotrack)) return StatusCode::SUCCESS;

  //Save some variables
  const LHCb::State& state = velotrack.hasStateAt(LHCb::State::LastMeasurement) ?
    *(velotrack.stateAt(LHCb::State::LastMeasurement)) :
    (velotrack.closestState(LHCb::State::EndVelo)) ;
  
  m_xVelo = state.x();
  m_yVelo = state.y();
  m_zVelo = state.z();
  m_txVelo = state.tx();
  m_tyVelo = state.ty();

  //Skip tracks outside 
  if((fabs(m_txVelo) > m_maxXSlope) || (fabs(m_tyVelo) > m_maxYSlope)) return StatusCode::SUCCESS;

  float xAtMidTT = m_xVelo + m_txVelo*(m_zMidTT-m_zVelo);
  m_yAtMidTT = m_yVelo + m_tyVelo*(m_zMidTT-m_zVelo);
  
  // skip tracks pointing into central hole of TT
  if(sqrt(xAtMidTT*xAtMidTT+m_yAtMidTT*m_yAtMidTT) < m_centralHoleSize) return StatusCode::SUCCESS;
  
  if(m_passTracks && fabs(xAtMidTT)< m_passHoleSize && fabs(m_yAtMidTT) < m_passHoleSize){
    std::unique_ptr<LHCb::Track> outTr{ new LHCb::Track() };
    outTr->reset();
    outTr->copy(velotrack);
    outTr->addToAncestors( velotrack );
    out.push_back(outTr.release());
    return StatusCode::SUCCESS;
  }
  
  //clear vectors
  m_normFact = { 1.0, 1.0, 1.0, 1.0 };
  m_invNormFact = m_normFact;
  m_fourLayerSolution = false;
  
  for(auto& ah : m_allHits) ah.clear();
  
  //Find deflection values
  m_PatTTMagnetTool->dxNormFactorsTT( m_tyVelo,  m_normFact);
  std::transform(m_normFact.begin(),m_normFact.end(),m_invNormFact.begin(),
                 [](float normFact){return 1.0/normFact;});
  
  //Save some variables
  m_yAt0 = m_yVelo + m_tyVelo*(0. - m_zVelo);
  m_xMid = m_xVelo + m_txVelo*(m_zKink-m_zVelo);
  m_wb = m_sigmaVeloSlope*(m_zKink - m_zVelo);
  m_wb=1./(m_wb*m_wb);
  
  m_invKinkVeloDist = 1/(m_zKink-m_zVelo);

  //
  //Find VeloTT track candidates
  //
  auto c = getCandidate(velotrack);
  if (c) out.push_back(c.release());
  return StatusCode::SUCCESS;
}

//=========================================================================
// Get all the VeloTT track candidates
//=========================================================================
std::unique_ptr<LHCb::Track> PatVeloTTHybridTool::getCandidate(const LHCb::Track& veloTrack) const {
  
  // Find hits within a search window
  if(!findHits()) return {};
  
  m_bestParams = { 0.0, m_maxPseudoChi2, 0.0, 0.0 };
  m_bestCandHits.clear();

  // -- Run clustering in forward direction
  formClusters();

  // -- Run clustering in backward direction
  if(!m_fourLayerSolution){
    std::reverse(m_allHits.begin(),m_allHits.end());
    formClusters();
    std::reverse(m_allHits.begin(),m_allHits.end());
  }

  //Write out the best solution
  return (!m_bestCandHits.empty()) ? prepareOutputTrack(veloTrack) : nullptr ;

}

//=========================================================================
//Find hits in a search window
//=========================================================================
bool PatVeloTTHybridTool::findHits() const {

  // protect against unphysical angles, should not happen
  auto invTheta = std::min(500.,vdt::fast_isqrt(m_txVelo*m_txVelo+m_tyVelo*m_tyVelo));
  auto minP = ((m_minPT*invTheta)>m_minMomentum) ? (m_minPT*invTheta):m_minMomentum;
  auto xTol = std::abs(1. / ( m_distToMomentum * minP ));
  auto yTol = m_yTol + m_yTolSlope * xTol;

  unsigned int nHits = 0;
  
  //--------------------------------------------------------------------------
  // -- Loop on regions
  // -- If y > 0, only loop over upper half
  // -- If y < 0, only loop over lower half
  //--------------------------------------------------------------------------
  
  unsigned int startLoop = 0;
  unsigned int endLoop = 8;
  
  if( m_yAtMidTT > 0.0 ){
    startLoop = 4;
  }else{
    endLoop = 4;
  }
  
  for(unsigned int i = startLoop ; i < endLoop; ++i){

    if( (i == 6 || i == 2) && nHits == 0){
      return false;
    }

    //Protect against empty layers
    if(m_hitsLayers[i].empty()) continue;
    
    const float dxDy   = m_hitsLayers[i].front()->hit()->dxDy();
    float yLayer = 0.0;
    const float zLayer =  m_hitsLayers[i].front()->z();
    const float xLayer = m_xVelo + m_txVelo*(zLayer - m_zVelo);
    const float yAtZ   = m_yVelo + m_tyVelo*(zLayer - m_zVelo);

    yLayer =  yAtZ + std::copysign(yTol, yAtZ);
    
    // max distance between strips in a layer => 15mm
    // Hits are sorted at y=0
    const float lowerBoundX = 
      xLayer - xTol*m_invNormFact[m_hitsLayers[i].front()->planeCode()] - dxDy*yLayer - fabs(m_txVelo)*m_intraLayerDist;
    
    const auto itEnd = m_hitsLayers[i].end();
    auto itH = std::lower_bound( m_hitsLayers[i].begin(), itEnd, lowerBoundX, Tf::compByX_LB< PatTTHit >() );
    
    for ( ; itH != itEnd; ++itH ){
      
      const float xOnTrack = m_xVelo + m_txVelo*((*itH)->z() - m_zVelo);
      updateTTHitForTrack((*itH),m_yAt0, m_tyVelo);
      const float dx = (*itH)->x() - xOnTrack;
      
      // -- Scale to the reference reg
      const float normDx = dx * m_normFact[(*itH)->planeCode()];

      if( normDx < -xTol ) continue;
      if( normDx > xTol ) break;
      
      const float fabsdx = std::abs(normDx);
      
      if(xTol > fabsdx){
      
        const float yOnTrack = m_yVelo + m_tyVelo*((*itH)->z() - m_zVelo);
        
        // -- Now refine the tolerance in Y
        if( yOnTrack + (m_yTol + m_yTolSlope * fabsdx) < (*itH)->hit()->yMin() ||
            yOnTrack - (m_yTol + m_yTolSlope * fabsdx) > (*itH)->hit()->yMax() ) continue;
      
        m_allHits[(*itH)->planeCode()].push_back(*itH);
        ++nHits;

      }
    } // over hits
  } //over layers
  return nHits > 2;
}
//=========================================================================
// Form clusters
//=========================================================================
void PatVeloTTHybridTool::formClusters() const {
  
  // Loop over First Layer
  for(auto hit0 : m_allHits[0]){
    
    const float xhitLayer0 = hit0->x();
    const float zhitLayer0 = hit0->z();
    
    // Loop over Second Layer
    for(auto hit1 : m_allHits[1]){
      
      const float xhitLayer1 = hit1->x();
      const float zhitLayer1 = hit1->z();

      const float tx = (xhitLayer1 - xhitLayer0)/(zhitLayer1 - zhitLayer0);

      if( std::abs(tx-m_txVelo) > m_deltaTx1 ) continue;
      
      m_clusterCandidate.clear();
      m_clusterCandidate.push_back(hit0);
      m_clusterCandidate.push_back(hit1);
      
      for( auto hit2 : m_allHits[2]){          
        
        const float xhitLayer2 = hit2->x();
        const float zhitLayer2 = hit2->z();

        const float xextrapLayer2 = xhitLayer1 + tx*(zhitLayer2-zhitLayer1);
        if(std::abs(xhitLayer2 - xextrapLayer2) > m_hitTol1)continue;
        
        const float tx2 = (xhitLayer2 - xhitLayer0)/(zhitLayer2 - zhitLayer0);
        if(std::abs(tx2-m_txVelo)>m_deltaTx2) continue;
        
        
        m_clusterCandidate.push_back(hit2);
        

        for( auto hit3 : m_allHits[3]){   
          
          const float xhitLayer3 = hit3->x();
          const float zhitLayer3 = hit3->z();
          
          const float xextrapLayer3 = xhitLayer2 + tx2*(zhitLayer3-zhitLayer2);
          
          if(std::abs(xhitLayer3 - xextrapLayer3) > m_hitTol2) continue;
          
          if(!m_fourLayerSolution){  
            m_fourLayerSolution = true;
            m_bestParams = { 0.0, m_maxPseudoChi2, 0.0, 0.0 };
            m_bestCandHits.clear();
          }
          
          m_clusterCandidate.push_back(hit3);
          simpleFit<4>( m_clusterCandidate );
          m_clusterCandidate.pop_back();
          
        }//layer3

        if(!m_fourLayerSolution){ 
          simpleFit<3>( m_clusterCandidate );
        }

        m_clusterCandidate.pop_back();
      }//layer2
      // Loop over Fourth Layer
      
      if(!m_fourLayerSolution){ 
        
        for( auto hit3 : m_allHits[3]){          
          
          const float xhitLayer3 = hit3->x();
          const float zhitLayer3 = hit3->z();

          const float xextrapLayer3 = xhitLayer1 + tx*(zhitLayer3-zhitLayer1);
          
          if(std::abs(xhitLayer3 - xextrapLayer3) > m_hitTol1) continue;
          
          m_clusterCandidate.push_back(hit3);
          simpleFit<3>( m_clusterCandidate );
          m_clusterCandidate.pop_back();
          
        }
      }//layer3
    }
  }
}
//=========================================================================
// Create the Velo-TT tracks
//=========================================================================
std::unique_ptr<LHCb::Track> PatVeloTTHybridTool::prepareOutputTrack(const LHCb::Track& veloTrack) const{
  
  //== Handle states. copy Velo one, add TT.
  const float zOrigin =  m_zVelo-m_yVelo/m_tyVelo;
  const float bdl= m_PatTTMagnetTool->bdlIntegral(m_tyVelo,zOrigin,m_zVelo);
  const float qpxz2p=-1*vdt::fast_isqrt(1.+m_tyVelo*m_tyVelo)/bdl*3.3356/Gaudi::Units::GeV;
  
  float qop = m_bestParams[0]*qpxz2p;
  if(std::abs(bdl) < 1.e-8 ) qop = 0.0;
  
  const float xTT = m_bestParams[2];
  const float txTT = m_bestParams[3];
 
  std::unique_ptr<LHCb::Track> outTr{ new LHCb::Track() };
  
  // reset the track
  outTr->reset();
  outTr->copy(veloTrack);
 
  // Adding overlap hits
  for( auto hit : m_bestCandHits){ 
  
    outTr->addToLhcbIDs( hit->hit()->lhcbID() );
    
    const float xhit = hit->x(); 
    const float zhit = hit->z();
    
    for( auto ohit : m_allHits[hit->planeCode()]){
      const float zohit = ohit->z();
      if(zohit==zhit) continue;
      
      const float xohit = ohit->x();
      const float xextrap = xhit + txTT*(zhit-zohit);
      if( xohit-xextrap < -m_overlapTol) continue;
      if( xohit-xextrap > m_overlapTol) break;
      outTr->addToLhcbIDs( ohit->hit()->lhcbID() );
      // -- only one overlap hit
      //break;
      
    }
  }

 
  
  // set q/p in all of the existing states
  const std::vector< LHCb::State * > states = outTr->states();
  for (auto& state : states) state->setQOverP(qop);

  //== Add a new state...
  LHCb::State temp;
  temp.setLocation( LHCb::State::AtTT );
  temp.setState( xTT,
                 m_yAtMidTT,
                 m_zMidTT,
                 txTT,
                 m_tyVelo,
                 qop );
  
  
  outTr->addToStates( std::move(temp) );
  
  outTr->setType( LHCb::Track::Upstream );
  outTr->setHistory( LHCb::Track::PatVeloTT );
  outTr->addToAncestors( veloTrack );
  outTr->setPatRecStatus( LHCb::Track::PatRecIDs );
  outTr->setChi2PerDoF( m_bestParams[1]);
  
  return std::move(outTr);

}
//=============================================================================
// -- Check if new event has occurred. If yes, set flag
// -- Note: The actions of initEvent cannot be executed here,
// -- as this handle runs before decoding the clusters
//=============================================================================
void PatVeloTTHybridTool::handle ( const Incident& incident ) {

  if ( IncidentType::BeginEvent == incident.type() ) m_newEvent = true;
  
}
//=============================================================================
// -- Init event: Get the new hits and sort them
//=============================================================================
void PatVeloTTHybridTool::initEvent () const {
  
  for(auto& hl : m_hitsLayers ) hl.clear();
  
  for(int iStation = 0; iStation < 2; ++iStation){
    for(int iLayer = 0; iLayer < 2; ++iLayer){
      
      Tf::TTStationHitManager<PatTTHit>::HitRange range =  m_utHitManager->sortedLayerHits<Tf::increasingByXAtYEq0<PatTTHit>>(iStation,iLayer);
      
      for( PatTTHit* hit : range){
        if(hit->hit()->yMax() > 0){
          m_hitsLayers[2*iStation + iLayer + 4].push_back( hit );
        }
        if(hit->hit()->yMin() < 0){
          m_hitsLayers[2*iStation + iLayer].push_back( hit );
        }
      }
    }
  }
  
  m_newEvent = false;

}

//=============================================================================
