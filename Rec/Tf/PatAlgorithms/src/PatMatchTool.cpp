// $Id: PatMatchTool.cpp,v 1.6 2008-07-18 16:10:14 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "PatMatchTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatMatchTool
//
// 2008-04-25 : Johannes Albrecht
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PatMatchTool );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatMatchTool::PatMatchTool( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IMatchTool>(this);
  declareInterface<ITrackMatch>(this);
 
  declareProperty( "zMagnet"         , m_zMagnet       = 5290. * Gaudi::Units::mm   );
  declareProperty( "zMatchY"         , m_zMatchY       = 8420. * Gaudi::Units::mm   );
  declareProperty( "dxTol"           , m_dxTol         =    8. * Gaudi::Units::mm   );
  declareProperty( "dxTolSlope"      , m_dxTolSlope    =   80. * Gaudi::Units::mm   );
  declareProperty( "dyTol"           , m_dyTol         =    8. * Gaudi::Units::mm   );
  declareProperty( "dyTolSlope"      , m_dyTolSlope    =  240. * Gaudi::Units::mm   );
  declareProperty( "maxMatchChi2"    , m_maxChi2       = 9                          );
  declareProperty( "FastMomentumToolName",	m_fastMomentumToolName	= "FastMomentumEstimate" );

}
//=============================================================================
// Destructor
//=============================================================================
PatMatchTool::~PatMatchTool() {} 

//=============================================================================

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatMatchTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_addTTClusterTool = tool<IAddTTClusterTool>( "PatAddTTCoord" );

  m_fastMomentumTool = tool<ITrackMomentumEstimate>( m_fastMomentumToolName );

  return StatusCode::SUCCESS;
}

StatusCode PatMatchTool::matchSingle(const LHCb::Track& velo,
                                     const LHCb::Track& seed , 
                                     LHCb::Track& output,
                                     double& chi2 )
{
  
  //calculate quality of match between tracks
  chi2 = getChi2Match(velo, seed );
  
  if ( m_maxChi2 > chi2 ) {
    
    //set states and flags of output track 
    makeTrack(velo, seed, output, chi2 );
    return StatusCode::SUCCESS;
  }
  
  return StatusCode::FAILURE;
  
}



//=============================================================================
// Main execution
//=============================================================================
StatusCode PatMatchTool::match(const LHCb::Tracks& velos,
                               const LHCb::Tracks& seeds,
                               LHCb::Tracks& matchs ) {
  
  std::vector<MatchCandidate> cand;

  LHCb::Tracks::const_iterator itV, itS;

  for ( itV = velos.begin(); velos.end() != itV; ++itV ) {
   
    for ( itS = seeds.begin(); seeds.end() != itS; ++itS ) {
     
      double dist = getChi2Match(*(*itV), *(*itS) );

      if ( m_maxChi2 > dist ) {
        MatchCandidate candidate( *itV, *itS, dist );
        cand.push_back( candidate );
      }
    }
  }
  std::sort( cand.begin(), cand.end(), MatchCandidate::lowerByChi2() );

  // for each track's key, tag if used or not.
  std::map<int,bool> veloUsed;
  std::map<int,bool> seedUsed;
  for ( itV = velos.begin(); velos.end() != itV; ++itV ) veloUsed[(*itV)->key()] = false;
  for ( itS = seeds.begin(); seeds.end() != itS; ++itS ) seedUsed[(*itS)->key()] = false;
  
  for ( std::vector<MatchCandidate>::iterator itM = cand.begin(); cand.end() != itM; ++itM ) {
    LHCb::Track* vTr = (*itM).vTr();
    LHCb::Track* sTr = (*itM).sTr();
    
    if(msgLevel( MSG::DEBUG )) 
      debug() << "Candidate Velo " << vTr->key() << " used " << veloUsed[ vTr->key()] 
              << " Seed " << sTr->key() << " used " << seedUsed[ sTr->key()] 
              << " dist " << (*itM).dist() << endreq;
    
    if ( veloUsed[ vTr->key()] ) continue;
    if ( seedUsed[ sTr->key()] ) continue;
    
    veloUsed[ vTr->key()] = true;
    seedUsed[ sTr->key()] = true;

    LHCb::Track* match = new LHCb::Track();
    
    makeTrack(*vTr, *sTr, *match, (*itM).dist() );

    m_addTTClusterTool->addTTClusters( *match );
    matchs.insert( match);
  
  }//end loop match cands

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatMatchTool::finalize() {

  debug() << "==> Finalize" << endmsg;
  
  return GaudiTool::finalize();
}

//=============================================================================

double PatMatchTool::getChi2Match(const LHCb::Track& velo,
                                  const LHCb::Track& seed ) 
{
  
  double chi2 = 1e6;
  double dxTol2 = m_dxTol * m_dxTol;
  double dxTolSlope2 = m_dxTolSlope * m_dxTolSlope;
  if ( velo.checkFlag( LHCb::Track::Invalid  ) ) return chi2;
  if ( velo.checkFlag( LHCb::Track::Backward ) ) return chi2;
  if ( seed.checkFlag( LHCb::Track::Invalid  ) ) return chi2;
  if ( seed.checkFlag( LHCb::Track::Backward ) ) return chi2;
  
  const LHCb::State& vState = velo.closestState( 0.);
  double xV = vState.x() + ( m_zMagnet - vState.z() ) * vState.tx();
  double yV = vState.y() + ( m_zMatchY - vState.z() ) * vState.ty();
  double teta2 = vState.tx() * vState.tx() + vState.ty() * vState.ty();
  double tolY  = m_dyTol * m_dyTol + teta2 * m_dyTolSlope * m_dyTolSlope;
  
  const LHCb::State& sState = seed.closestState( m_zMatchY );
  double xS = sState.x() + ( m_zMagnet - sState.z() ) * sState.tx();
  double yS = sState.y() + ( m_zMatchY - sState.z() ) * sState.ty();
      
  double distX = xS - xV;
  double distY = yS - yV;
  double dslx  = vState.tx() - sState.tx();
  double tolX = dxTol2 + dslx * dslx *  dxTolSlope2;
  
  chi2 = distX * distX / tolX + distY * distY /tolY;
  
  return chi2;
}

//=============================================================================
void PatMatchTool::makeTrack( const LHCb::Track& velo,
                              const LHCb::Track& seed,
                              LHCb::Track& output,
                              double chi2              )
{

  
  const LHCb::State& vState = velo.closestState( 0.);
  const LHCb::State& sState =seed.closestState( m_zMatchY );
  //== Store the track
  output.addToAncestors( velo );                // set the velo as ancestor
  output.addToAncestors( seed );                // set the seed as ancestor
  output.addToStates( vState );   // copy the state of the Velo
  output.addToStates( sState );   // copy the state of the seed

  //get closest state at 0 and set q over p
  LHCb::State& oState = output.closestState( 0.); 
  
  double qOverP, sigmaQOverP;
  //FIXME true or false??  
  StatusCode sc = m_fastMomentumTool->calculate(&vState,&sState, 
                                                qOverP, sigmaQOverP, true);

  if(sc.isFailure()) warning()<<"momentum determination failed!"<<endmsg;
  
  Gaudi::TrackSymMatrix& cov = oState.covariance();
  cov(4,4) = sigmaQOverP * sigmaQOverP;
  oState.setQOverP(qOverP);
  
  //== Adjust flags
  output.setType(         LHCb::Track::Long  );
  output.setHistory(      LHCb::Track::PatMatch );
  output.setPatRecStatus( LHCb::Track::PatRecIDs   );
  output.setLhcbIDs(  velo.lhcbIDs()      );   // copy those from the Seed
  output.addInfo(LHCb::Track::MatchChi2, chi2);
  

  //== add new LHCbIDs
  for ( std::vector<LHCb::LHCbID>::const_iterator itH = seed.lhcbIDs().begin(); 
        seed.lhcbIDs().end() != itH; ++itH ){
    output.addToLhcbIDs( *itH );
  }      
        
  return;
}

//=============================================================================
  StatusCode PatMatchTool::match( const LHCb::Track& veloTrack,
                                  const LHCb::Track& tTrack,
                                  LHCb::Track& matchedTrack,
                                  double& quality, double& quality2 ) {

    quality2=0;
    StatusCode sc = this->matchSingle( veloTrack, tTrack, matchedTrack, quality );
    if(sc.isFailure()) 
      if(msgLevel( MSG::DEBUG )) debug()<<"matching failed !"<<endmsg;

    return sc;
  }
