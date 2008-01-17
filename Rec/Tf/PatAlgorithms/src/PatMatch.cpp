// $Id: PatMatch.cpp,v 1.5 2008-01-17 06:57:59 ocallot Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "PatMatch.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatMatch
//
// 2007-02-07 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PatMatch );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatMatch::PatMatch( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "VeloInput"       , m_veloLocation  = LHCb::TrackLocation::Velo );
  declareProperty( "SeedInput"       , m_seedLocation  = LHCb::TrackLocation::Seed );
  declareProperty( "MatchOutput"     , m_matchLocation = LHCb::TrackLocation::Match );
  declareProperty( "zMagnet"         , m_zMagnet       = 5290. * Gaudi::Units::mm   );
  declareProperty( "zMatchY"         , m_zMatchY       = 8420. * Gaudi::Units::mm   );
  declareProperty( "dxTol"           , m_dxTol         =    8. * Gaudi::Units::mm   );
  declareProperty( "dxTolSlope"      , m_dxTolSlope    =   80. * Gaudi::Units::mm   );
  declareProperty( "dyTol"           , m_dyTol         =    8. * Gaudi::Units::mm   );
  declareProperty( "dyTolSlope"      , m_dyTolSlope    =  240. * Gaudi::Units::mm   );
  declareProperty( "momentumScale"   , m_momentumScale = -0.78 / Gaudi::Units::GeV  );
}
//=============================================================================
// Destructor
//=============================================================================
PatMatch::~PatMatch() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatMatch::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_addTTClusterTool = tool<IAddTTClusterTool>( "PatAddTTCoord" );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatMatch::execute() {

  LHCb::Tracks* matchs  = new LHCb::Tracks();
  matchs->reserve(200);
  put(matchs, m_matchLocation);
 
  LHCb::Tracks* velos  = get<LHCb::Tracks>( m_veloLocation );
  LHCb::Tracks* seeds  = get<LHCb::Tracks>( m_seedLocation ); 
  
  std::vector<MatchCandidate> cand;

  LHCb::Tracks::iterator itV, itS;

  double dxTol2 = m_dxTol * m_dxTol;
  double dxTolSlope2 = m_dxTolSlope * m_dxTolSlope;
  
  for ( itV = velos->begin(); velos->end() != itV; ++itV ) {
    if ( (*itV)->checkFlag( LHCb::Track::Invalid  ) ) continue;
    if ( (*itV)->checkFlag( LHCb::Track::Backward ) ) continue;    
    LHCb::State& vState = (*itV)->closestState( 0.);
    double xV = vState.x() + ( m_zMagnet - vState.z() ) * vState.tx();
    double yV = vState.y() + ( m_zMatchY - vState.z() ) * vState.ty();
    double teta2 = vState.tx() * vState.tx() + vState.ty() * vState.ty();
    double tolY  = m_dyTol * m_dyTol + teta2 * m_dyTolSlope * m_dyTolSlope;

    for ( itS = seeds->begin(); seeds->end() != itS; ++itS ) {
      if ( (*itS)->checkFlag( LHCb::Track::Invalid  ) ) continue;
      if ( (*itS)->checkFlag( LHCb::Track::Backward ) ) continue;
      LHCb::State& sState = (*itS)->closestState( m_zMatchY );
      double xS = sState.x() + ( m_zMagnet - sState.z() ) * sState.tx();
      double yS = sState.y() + ( m_zMatchY - sState.z() ) * sState.ty();

      double distX = xS - xV;
      double distY = yS - yV;
      double dslx  = vState.tx() - sState.tx();
      double tolX = dxTol2 + dslx * dslx *  dxTolSlope2;

      double dist = distX * distX / tolX + distY * distY /tolY;

      if ( 9 > dist ) {
        MatchCandidate candidate( *itV, *itS, dist );
        cand.push_back( candidate );
        debug() << "Velo track " << (*itV)->key() << " match Seed track " << (*itS)->key() 
                << " distX " << distX << " distY " << distY << " dist " << dist << endreq;
      }
    }
  }
  std::sort( cand.begin(), cand.end(), MatchCandidate::lowerByChi2() );

  // for each track's key, tag if used or not.
  std::map<int,bool> veloUsed;
  std::map<int,bool> seedUsed;
  for ( itV = velos->begin(); velos->end() != itV; ++itV ) veloUsed[(*itV)->key()] = false;
  for ( itS = seeds->begin(); seeds->end() != itS; ++itS ) seedUsed[(*itS)->key()] = false;
  
  for ( std::vector<MatchCandidate>::iterator itM = cand.begin(); cand.end() != itM; ++itM ) {
    LHCb::Track* vTr = (*itM).vTr();
    LHCb::Track* sTr = (*itM).sTr();

    debug() << "Candidate Velo " << vTr->key() << " used " << veloUsed[ vTr->key()] 
            << " Seed " << sTr->key() << " used " << seedUsed[ sTr->key()] << " dist " << (*itM).dist() << endreq;
   
    if ( veloUsed[ vTr->key()] ) continue;
    if ( seedUsed[ sTr->key()] ) continue;
    
    veloUsed[ vTr->key()] = true;
    seedUsed[ sTr->key()] = true;
    
    LHCb::State& vState = vTr->closestState( 0.);
    LHCb::State& sState = sTr->closestState( m_zMatchY );
    //== Store the track
    debug() << "Store Match track from Velo " << vTr->key() << " and Seed " << sTr->key() << endreq;
    LHCb::Track* match = new LHCb::Track();
    match->addToAncestors( vTr );                // set the velo as ancestor
    match->addToAncestors( sTr );                // set the seed as ancestor
    match->addToStates( vState );   // copy the state of the Velo
    match->addToStates( sState );   // copy the state of the seed
    //== Set momentum to the Velo
    double dSlope = vState.tx() - sState.tx();
    match->closestState( 0. ).setQOverP( -0.0008 * dSlope );
    
    //== Adjust flags
    match->setType(         LHCb::Track::Long  );
    match->setHistory(      LHCb::Track::PatMatch );
    match->setPatRecStatus( LHCb::Track::PatRecIDs   );
    match->setLhcbIDs(  vTr->lhcbIDs()      );   // copy those from the Seed
    //== add new LHCbIDs
    for ( std::vector<LHCb::LHCbID>::const_iterator itH = sTr->lhcbIDs().begin(); 
          sTr->lhcbIDs().end() != itH; ++itH ){
      match->addToLhcbIDs( *itH );
    }      
    m_addTTClusterTool->addTTClusters( *match );
    matchs->insert( match);
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatMatch::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
