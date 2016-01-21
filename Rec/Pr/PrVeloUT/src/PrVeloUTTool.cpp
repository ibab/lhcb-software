// Include files

#include "TfKernel/RecoFuncs.h"

// local
#include "PrVeloUTTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrVeloUTTool
//
// 2007-05-08 : Mariusz Witek
//-----------------------------------------------------------------------------


// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PrVeloUTTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrVeloUTTool::PrVeloUTTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
 : IPrVeloUTTool ( type, name , parent )
{
  declareInterface<IPrVeloUTTool>(this);

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
  declareProperty("IntraLayerDist"     , m_intraLayerDist   = 15.0 * Gaudi::Units::mm);
  declareProperty("HitTol1"             , m_hitTol1           = 6.0 * Gaudi::Units::mm);
  declareProperty("HitTol2"             , m_hitTol2           = 0.8 * Gaudi::Units::mm);
  declareProperty("DeltaTx1"            , m_deltaTx1           = 0.035);
  declareProperty("DeltaTx2"            , m_deltaTx2           = 0.02);
  declareProperty("PrintVariables"     , m_PrintVariables   = false);
  //Passing Velo tracks
  declareProperty("PassTracks"         , m_PassTracks   = false);
  declareProperty("passHoleSize"       , m_passHoleSize  = 37. * Gaudi::Units::mm);
}
//=============================================================================
// Destructor
//=============================================================================
PrVeloUTTool::~PrVeloUTTool() {}

//=============================================================================
// Main method for VELOUT track reconstruction
//=============================================================================
//=========================================================================
//  Initialisation, check parameters
//=========================================================================
StatusCode PrVeloUTTool::initialize ( ) {
  StatusCode sc = GaudiTool::initialize();
  if ( !sc ) return sc;

  m_PrUTMagnetTool = tool<PrUTMagnetTool>( "PrUTMagnetTool","PrUTMagnetTool");

  // m_zMidUT is a position of normalization plane which should to be close to z middle of UT ( +- 5 cm ).
  // Cashed once in PrVeloUTTool at initialization. No need to update with small UT movement.
  m_zMidUT    = m_PrUTMagnetTool->zMidUT();
  //  zMidField and distToMomentum isproperly recalculated in PrUTMagnetTool when B field changes
  m_distToMomentum = m_PrUTMagnetTool->averageDist2mom();
  
  if(m_PrintVariables){
    info() << " minMomentum        = " << m_minMomentum      << " MeV" << endmsg;
    info() << " minPT              = " << m_minPT            << " MeV" << endmsg;
    info() << " maxPseudoChi2      = " << m_maxPseudoChi2    << "   "  << endmsg;
    info() << " MaxXSlope          = " << m_maxXSlope        << "   "  << endmsg;
    info() << " MaxYSlope          = " << m_maxYSlope        << "   "  << endmsg;
    info() << " centralHoleSize    = " << m_centralHoleSize  << " mm"  << endmsg;
    info() << " yTolerance         = " << m_yTol             << " mm"  << endmsg;
    info() << " YTolSlope          = " << m_yTolSlope                  << endmsg;
    info() << " HitTol1            = " << m_hitTol1       << " mm " << endmsg;
    info() << " HitTol2            = " << m_hitTol2       << " mm " << endmsg;
    info() << " DeltaTx1           = " << m_deltaTx1       << "  " << endmsg;
    info() << " DeltaTx2           = " << m_deltaTx2       << "  " << endmsg;
    info() << " IntraLayerDist     = " << m_intraLayerDist   << " mm " << endmsg;
    info() << " PassTracks         = " << m_PassTracks                 << endmsg;
    info() << " passHoleSize       = " << m_passHoleSize     << " mm " << endmsg;
    
    info() << " zMidUT             = " << m_zMidUT           << " mm"  << endmsg;
    info() << " distToMomentum     = " << m_distToMomentum             << endmsg;
  }
  
  m_utHitManager   = tool<Tf::UTStationHitManager <PrUTHit> >("PrUTStationHitManager");

  // -- Event listener
  m_newEvent = true;
  incSvc()->addListener(this, IncidentType::BeginEvent);
  
  m_normFact.reserve(4);
  m_invNormFact.reserve(4);
  
  m_vuttracks.reserve(50);
  m_bestCand.reserve(1);
  
  m_allClusters.reserve(50);
  m_clusterCandidate.reserve(4);
  
  PrUTHits dummy;
  dummy.reserve(100);
  m_hitsLayers.push_back( dummy );
  m_hitsLayers.push_back( dummy );
  m_hitsLayers.push_back( dummy );
  m_hitsLayers.push_back( dummy );
  m_hitsLayers.push_back( dummy );
  m_hitsLayers.push_back( dummy );
  m_hitsLayers.push_back( dummy );
  m_hitsLayers.push_back( dummy );

  m_allHits.push_back( dummy );
  m_allHits.push_back( dummy );
  m_allHits.push_back( dummy );
  m_allHits.push_back( dummy );

  return StatusCode::SUCCESS;
}


//=========================================================================
// Main reconstruction method
//=========================================================================
void PrVeloUTTool::recoVeloUT(LHCb::Track & velotrack, std::vector<LHCb::Track*>& outtracks )
{
  //Check if it is a new event
  if(m_newEvent) initEvent();
  
  //Remove backward/invalid tracks
  if(!acceptTrack(velotrack)) return;

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
  if((fabs(m_txVelo) > m_maxXSlope) || (fabs(m_tyVelo) > m_maxYSlope)) return;

  float xAtMidUT = m_xVelo + m_txVelo*(m_zMidUT-m_zVelo);
  m_yAtMidUT = m_yVelo + m_tyVelo*(m_zMidUT-m_zVelo);
  
  // skip tracks pointing into central hole of UT
  if(xAtMidUT*xAtMidUT + m_yAtMidUT*m_yAtMidUT < m_centralHoleSize*m_centralHoleSize) return;

  if(m_PassTracks && xAtMidUT*xAtMidUT + m_yAtMidUT*m_yAtMidUT < m_passHoleSize*m_passHoleSize){
    outtracks.push_back(new LHCb::Track(velotrack));
    return;
  }
  
  //clear vectors
  m_normFact.clear();
  m_invNormFact.clear();
  
  m_vuttracks.clear();
  m_bestCand.clear();
  m_foundCand = false;
  m_fourLayerSolution = false;
  
  for(unsigned int i = 0; i < 4; ++i){
    m_allHits[i].clear();
  }
  m_allClusters.clear();
  
  //Find deflection values
  m_PrUTMagnetTool->dxNormFactorsUT( m_tyVelo,  m_normFact);
  m_invNormFact.resize(m_normFact.size());
  std::transform(m_normFact.begin(),m_normFact.end(),m_invNormFact.begin(),
                 [](float normFact){return 1.0/normFact;});
  
  //Save some variables
  float zOrigin =  m_zVelo-m_yVelo/m_tyVelo;
  m_zKink = m_PrUTMagnetTool->zBdlMiddle(m_tyVelo,zOrigin,m_zVelo);
  m_bdl= m_PrUTMagnetTool->bdlIntegral(m_tyVelo,zOrigin,m_zVelo);
  m_yAt0 = m_yVelo + m_tyVelo*(0. - m_zVelo);
  
  //
  //Find VeloUT track candidates
  //
  
  getCandidates(velotrack,outtracks); 
  
}

//=========================================================================
// Get all the VeloUT track candidates
//=========================================================================
void PrVeloUTTool::getCandidates( LHCb::Track& veloTrack,std::vector<LHCb::Track*>& outtracks ){
  
  LHCb::Track* veloTr = &veloTrack;
  
  // Find hits within a search window
  if(!findHits()) return;
  
  // Form clusters from the selected hits
  clustering();
  
  // Check there is at least one cluster
  int nClusters = m_allClusters.size();
  if(nClusters<1) return;
  
  // Create VeloUT track candidates
  for(PrUTHits theClusters : m_allClusters){

    PrVUTTrack candidate( veloTr );

    for(PrUTHit* hit : theClusters){ 
      candidate.storeHit( hit );
    }
    // Add this solution
    m_vuttracks.push_back(candidate);

  } // loop over the VeloUT track candidates
  for(PrVUTTrack cand : m_vuttracks){
    simpleFit(cand);
  }

  //Write out the best solution
  if(!m_foundCand) return;
  prepareOutputTracks(outtracks);

}

//=========================================================================
//Find hits in a search window
//=========================================================================
bool PrVeloUTTool::findHits(){

  float invTheta = vdt::fast_isqrt(m_txVelo*m_txVelo+m_tyVelo*m_tyVelo);
  // protect against unphysical angles, should not happen
  if(invTheta > 500.0 ) invTheta = 500.0;
  
  float minP = ((m_minPT*invTheta)>m_minMomentum) ? (m_minPT*invTheta):m_minMomentum;
  
  float xTol = fabs(1. / ( m_distToMomentum * minP ));
  float yTol = m_yTol + m_yTolSlope * xTol;

  unsigned int nHits = 0;
  
  //--------------------------------------------------------------------------
  // -- Loop on regions
  // -- If y > 0, only loop over upper half
  // -- If y < 0, only loop over lower half
  //--------------------------------------------------------------------------
  
  unsigned int startLoop = 0;
  unsigned int endLoop = 8;
  
  if( m_yAtMidUT > 0.0 ){
    startLoop = 4;
  }else{
    endLoop = 4;
  }
  
  for(unsigned int i = startLoop ; i < endLoop; ++i){

    if( (i == 6 || i == 2) && nHits == 0){
      return false;
    }
    
    float dxDy   = m_hitsLayers[i].front()->hit()->dxDy();
    float yLayer = 0.0;
    float zLayer =  m_hitsLayers[i].front()->z();
    float xLayer = m_xVelo + m_txVelo*(zLayer - m_zVelo);
    float yAtZ   = m_yVelo + m_tyVelo*(zLayer - m_zVelo);

    if( yAtZ > 0){
      yLayer =  yAtZ + yTol;
    }
    else{
      yLayer =  yAtZ - yTol;
    }
    
    // max distance between strips in a layer => 15mm
    // Hits are sorted at y=0
    float lowerBoundX = 
      xLayer - xTol*m_invNormFact[m_hitsLayers[i].front()->planeCode()] - dxDy*yLayer - fabs(m_txVelo)*m_intraLayerDist;
    
    auto itEnd =  m_hitsLayers[i].end();
    auto itH = std::lower_bound( m_iterators[i], itEnd, lowerBoundX, Tf::compByX_LB< PrUTHit >() );
    
    for ( ; itH != itEnd; ++itH ){
      
      float xOnTrack = m_xVelo + m_txVelo*((*itH)->z() - m_zVelo);
      updateUTHitForTrack((*itH),m_yAt0, m_tyVelo);
      float dx = (*itH)->x() - xOnTrack;
      
      // -- Scale to the reference reg
      float normDx = dx * m_normFact[(*itH)->planeCode()];

      if( normDx < -xTol ) continue;
      if( normDx > xTol ) break;
      
      float fabsdx = fabs(normDx);
      
      if(xTol > fabsdx){
      
        float yOnTrack = m_yVelo + m_tyVelo*((*itH)->z() - m_zVelo);
        
        // -- Now refine the tolerance in Y
        if( yOnTrack + (m_yTol + m_yTolSlope * fabsdx) < (*itH)->hit()->yMin() ||
            yOnTrack - (m_yTol + m_yTolSlope * fabsdx) > (*itH)->hit()->yMax() ) continue;
      
        m_allHits[(*itH)->planeCode()].push_back(*itH);
        ++nHits;

      }
    } // over hits
  } //over layers
  return true;
}

//=========================================================================
// Select the best list of sorted hits...
//=========================================================================
void PrVeloUTTool::clustering(){

  //Run clustering in forward direction
  formClusters(true);
  
  //Run clustering in backward direction
  if(!m_fourLayerSolution){
    formClusters(false);
  }

  //remove Clusters with 3 hits
  if(m_fourLayerSolution){
    m_allClusters.erase(std::remove_if(m_allClusters.begin(),m_allClusters.end(),
                                       [](PrUTHits clusters){return clusters.size()<4;}),
                        m_allClusters.end());
  }
  
} //clustering

//=========================================================================
// Form clusters
//=========================================================================

void PrVeloUTTool::formClusters(bool forward){
  
  if(!forward){
    std::reverse(m_allHits.begin(),m_allHits.end());
  }
  
  // Loop over First Layer
  for(auto hit0 : m_allHits[0]){
    
    float xhitLayer0 = hit0->x();
    float zhitLayer0 = hit0->z();
    
    // Loop over Second Layer
    for(auto hit1 : m_allHits[1]){
      
      float xhitLayer1 = hit1->x();
      float zhitLayer1 = hit1->z();

      float tx = (xhitLayer1 - xhitLayer0)/(zhitLayer1 - zhitLayer0);

      if(fabs(tx-m_txVelo)>m_deltaTx1) continue;

      float hitTol1 = m_hitTol1;
      float hitTol2 = m_hitTol2;
            
      m_clusterCandidate.clear();
      m_clusterCandidate.push_back(hit0);
      m_clusterCandidate.push_back(hit1);
      
      for( auto hit2 : m_allHits[2]){          
        
        float xhitLayer2 = hit2->x();
        float zhitLayer2 = hit2->z();

        float xextrapLayer2 = xhitLayer1 + tx*(zhitLayer2-zhitLayer1);
        
        if(fabs(xhitLayer2 - xextrapLayer2) < hitTol1){
          
          m_clusterCandidate.push_back(hit2);
          if(!m_fourLayerSolution){  
          m_allClusters.push_back(m_clusterCandidate);
          }
   
          for( auto hit3 : m_allHits[3]){   

            float xhitLayer3 = hit3->x();
            float zhitLayer3 = hit3->z();
            
            float tx2 = (xhitLayer2 - xhitLayer0)/(zhitLayer2 - zhitLayer0);
            if(fabs(tx2-m_txVelo)>m_deltaTx2) continue;
            
            float xextrapLayer3 = xhitLayer2 + tx2*(zhitLayer3-zhitLayer2);
            
            if(fabs(xhitLayer3 - xextrapLayer3) < hitTol2){
              
              if(!m_fourLayerSolution){  
                m_fourLayerSolution = true;
                m_allClusters.pop_back();
              }
              
              m_clusterCandidate.push_back(hit3);
              m_allClusters.push_back(m_clusterCandidate);
              m_clusterCandidate.pop_back();
            }
          }//layer3
          m_clusterCandidate.pop_back();
        }
      }//layer2
      // Loop over Fourth Layer
      
      if(!m_fourLayerSolution){ 
        
        for( auto hit3 : m_allHits[3]){          
        
          float xhitLayer3 = hit3->x();
          float zhitLayer3 = hit3->z();

          float xextrapLayer3 = xhitLayer1 + tx*(zhitLayer3-zhitLayer1);
          
          if(std::abs(xhitLayer3 - xextrapLayer3) < hitTol1){
            m_clusterCandidate.push_back(hit3);
            m_allClusters.push_back(m_clusterCandidate);      
            m_clusterCandidate.pop_back();
          }
        }
      }//layer3
    }
  }
}




//=========================================================================
// A kind of global track fit in VELO and UT
// The pseudo chi2 consists of two contributions:
//  - chi2 of Velo track x slope
//  - chi2 of a line in UT
// The two track segments go via the same (x,y) point
// at z corresponding to the half Bdl of the track
//
// Only q/p and chi2 of outTr are modified
//
//=========================================================================
void PrVeloUTTool::simpleFit(PrVUTTrack& vuttr) {
  
  PrUTHits theHits = vuttr.clusters();
  int nHits = theHits.size();

  m_c11 = 0.;
  m_c12 = 0.;
  m_c13 = 0.;
  m_c21 = 0.;
  m_c22 = 0.;
  m_c23 = 0.;

  for ( auto hit : theHits){
    
    float ui = hit->x();

    float ci = hit->hit()->cosT();
    float dz = hit->z() - m_zMidUT;
    float wi = hit->hit()->weight();

    m_c11 += wi * ci;
    m_c12 += wi * ci * dz;
    m_c13 += wi * ui;
    m_c22 += wi * ci * dz * dz;
    m_c23 += wi * ui * dz;
  }
  // add chi2 from VELO slope

  float xmid = m_xVelo + m_txVelo*(m_zKink-m_zVelo);
  // fixed velo slope error. Momentum dependent error + iteration is unstable
  float sigmaVeloSlope = 0.10*Gaudi::Units::mrad;
  float wb = sigmaVeloSlope*(m_zKink - m_zVelo);
  wb=1./(wb*wb);
  m_c11 += wb;
  m_c12 += wb*(m_zKink-m_zMidUT);
  m_c13 += wb*xmid;
  m_c22 += wb*(m_zKink-m_zMidUT)*(m_zKink-m_zMidUT);
  m_c23 += wb*xmid*(m_zKink-m_zMidUT);

  m_c21 = m_c12;

  float xUTFit, xSlopeUTFit;  // x pos and slope of track at UT after fit
  float den=m_c11*m_c22-m_c21*m_c12;
  if(fabs(den)>1.0e-8) {
    xUTFit      = (m_c13*m_c22-m_c23*m_c12)/(m_c11*m_c22-m_c21*m_c12);
    xSlopeUTFit = (m_c13*m_c21-m_c23*m_c11)/(m_c12*m_c21-m_c22*m_c11);
  }
  else  {
    xUTFit = 0.;
    xSlopeUTFit = 0.;
    vuttr.setChi2PerDoF(1.0e19);
    return;
  }

  // new VELO slope x
  float xb = xUTFit+xSlopeUTFit*(m_zKink-m_zMidUT);
  float xSlopeVeloFit = (xb-m_xVelo)/(m_zKink-m_zVelo);

  float chi2VeloSlope = (m_txVelo - xSlopeVeloFit)/sigmaVeloSlope;
  chi2VeloSlope = chi2VeloSlope * chi2VeloSlope;

  // calculate final chi2
  float chi2UT = 0.;

  for( auto hit : theHits ){

    float zd    = hit->z();
    float xd    = xUTFit + xSlopeUTFit*(zd-m_zMidUT);
    float du    = xd - hit->x();
    float chi2p = (du*du)*hit->hit()->weight();
    chi2UT += chi2p;

  }
  float chi2Global  = chi2UT + chi2VeloSlope;
  chi2Global /= float((nHits+1-2));

  // calculate q/p
  float sinInX  = xSlopeVeloFit*vdt::fast_isqrt(1.+xSlopeVeloFit*xSlopeVeloFit);
  float sinOutX = xSlopeUTFit*vdt::fast_isqrt(1.+xSlopeUTFit*xSlopeUTFit);

  float qp=0.;
  if(fabs(m_bdl)>1.e-8) {
    float qpxz2p=vdt::fast_isqrt(1.+m_tyVelo*m_tyVelo);
    qp=-qpxz2p*(sinInX-sinOutX)/m_bdl*3.3356/Gaudi::Units::GeV;
  }

  // update track state (qOverP and chiSquared only)

  vuttr.setQOverP(qp);
  vuttr.setChi2PerDoF(chi2Global);
  vuttr.setXUT(xUTFit);
  vuttr.setXSlopeUT(xSlopeUTFit);

  if(chi2Global<m_maxPseudoChi2){
    if(!m_foundCand){
      m_bestCand.push_back(vuttr);
      m_foundCand = true;
    }
    else if(chi2Global < (*m_bestCand.begin()).chi2PerDoF()){
      m_bestCand.pop_back();
      m_bestCand.push_back(vuttr);
    }
  }

}

//=========================================================================
// Create the Velo-UT tracks
//=========================================================================
void PrVeloUTTool::prepareOutputTracks(std::vector<LHCb::Track*>& outtracks){

  PrVUTTrack cand = *m_bestCand.begin();
  PrUTHits candClusters = cand.clusters();
  LHCb::Track* veloTr = cand.track();
  
  //== Handle states. copy Velo one, add UT.
  
  float qop = cand.qOverP();
  float xUT = cand.xUT();
  float txUT = cand.xSlopeUT();
    
  LHCb::Track* outTr = new LHCb::Track();
  
  // reset the track
  outTr->reset();
  
  outTr->copy(*veloTr);
  
  // set q/p in all of the existing states
  const std::vector< LHCb::State * > states = outTr->states();
  std::vector< LHCb::State * >::const_iterator iState;
  for ( iState = states.begin() ; iState != states.end() ; ++iState ){
    (*iState)->setQOverP(qop);
  }

  //Add UT hits to track
  for( auto hit : candClusters){
    outTr->addToLhcbIDs( hit->hit()->lhcbID() );
  }
  
  
  //== Add a new state...
  LHCb::State temp;
  temp.setLocation( LHCb::State::AtTT );
  temp.setState( xUT,
                 cand.yAtZ( m_zMidUT ),
                 m_zMidUT,
                 txUT,
                 m_tyVelo,
                 qop );
  
  
  outTr->addToStates( temp );
  
  outTr->setType( LHCb::Track::Upstream );
  outTr->setHistory( LHCb::Track::PrVeloUT );
  outTr->addToAncestors( veloTr );
  outTr->setPatRecStatus( LHCb::Track::PatRecIDs );
  outTr->setChi2PerDoF(cand.chi2PerDoF());
  
  outtracks.push_back(outTr);

}

//=============================================================================
// -- Check if new event has occurred. If yes, set flag
// -- Note: The actions of initEvent cannot be executed here,
// -- as this handle runs before decoding the clusters
//=============================================================================
void PrVeloUTTool::handle ( const Incident& incident ) {

  if ( IncidentType::BeginEvent == incident.type() ) m_newEvent = true;
  
}

//=============================================================================
// -- Init event: Get the new hits and sort them
//=============================================================================
void PrVeloUTTool::initEvent () {
  
  for(unsigned int i = 0; i < 8; ++i){
    m_hitsLayers[i].clear();
  }
  
  Tf::UTStationHitManager<PrUTHit>::HitRange range = m_utHitManager->hits();
  
  for(PrUTHit* hit : range){
        
    if(hit->hit()->yMax() > 0){
      m_hitsLayers[hit->planeCode() + 4].push_back( hit );
    }
    if(hit->hit()->yMin() < 0){
      m_hitsLayers[hit->planeCode()].push_back( hit );
    }
  }
  
  for(unsigned int i = 0; i < 8; ++i){
    std::sort(m_hitsLayers[i].begin(), m_hitsLayers[i].end(), compX() );
    m_iterators[i] = m_hitsLayers[i].begin();
  }
  
  m_newEvent = false;
  

}

//=============================================================================
bool PrVeloUTTool::acceptTrack(const LHCb::Track& track) 
{
  bool ok = !(track.checkFlag( LHCb::Track::Invalid) );
  ok = ok && (!(track.checkFlag( LHCb::Track::Backward) ));
  return ok;
}
