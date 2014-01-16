// $Id: 
// from Gaudi
#include <GaudiKernel/ToolFactory.h>

// local
#include "PatMatchL0MuonTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatMatchTool
//
// 2008-04-25 : Johannes Albrecht
// 2010-11-17 : Roel Aaij
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PatMatchL0MuonTool );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatMatchL0MuonTool::PatMatchL0MuonTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
   : GaudiTool ( type, name , parent )
{
   declareInterface< ITrackMatch >( this );
 
   declareProperty( "zMagnet"         , m_zMagnet       =  5200. * Gaudi::Units::mm   );
   declareProperty( "zMagnetTx2"      , m_zMagnetTx2    = -1700. * Gaudi::Units::mm   );
   declareProperty( "zMagnetDsl2"     , m_zMagnetDsl2   =   500. * Gaudi::Units::mm   );
   declareProperty( "zMatchY"         , m_zMatchY       = 11900. * Gaudi::Units::mm   );
   declareProperty( "dxTol"           , m_dxTol         =    16. * Gaudi::Units::mm   );
   declareProperty( "dxTolSlope"      , m_dxTolSlope    =   160. * Gaudi::Units::mm   );
   declareProperty( "dyTol"           , m_dyTol         =    12. * Gaudi::Units::mm   );
   declareProperty( "dyTolSlope"      , m_dyTolSlope    =   600. * Gaudi::Units::mm   );
   declareProperty( "MagnetBend"      , m_magnetBend    = -1000. * Gaudi::Units::mm   );
   declareProperty( "maxMatchChi2"    , m_maxChi2       =     9                       );
   declareProperty( "FastMomentumToolName",	m_fastMomentumToolName	= "FastMomentumEstimate" );

}
//=============================================================================
// Destructor
//=============================================================================
PatMatchL0MuonTool::~PatMatchL0MuonTool()
{

} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatMatchL0MuonTool::initialize()
{
   StatusCode sc = GaudiTool::initialize();
   if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

   debug() << "==> Initialize" << endmsg;

   m_fastMomentumTool = tool<ITrackMomentumEstimate>( m_fastMomentumToolName );

   return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatMatchL0MuonTool::finalize()
{
   debug() << "==> Finalize" << endmsg;
  
   return GaudiTool::finalize();
}

//=============================================================================
double PatMatchL0MuonTool::getChi2Match(const LHCb::Track& velo,
                                        const LHCb::Track& seed ) 
{
  
   double chi2 = 1e6;
   if ( velo.checkFlag( LHCb::Track::Invalid  ) ) return chi2;
   if ( velo.checkFlag( LHCb::Track::Backward ) ) return chi2;
   if ( seed.checkFlag( LHCb::Track::Invalid  ) ) return chi2;
  
   const LHCb::State& vState = velo.closestState( 0.);
   const LHCb::State& sState = seed.closestState( m_zMatchY );

   double dSlope = vState.tx() - sState.tx();
   double zForX = m_zMagnet 
      + m_zMagnetTx2 * vState.tx() * vState.tx()
      + m_zMagnetDsl2 * dSlope * dSlope;
   double dxTol2 = m_dxTol * m_dxTol;
   double dxTolSlope2 = m_dxTolSlope * m_dxTolSlope;
  
   double xV = vState.x() + ( zForX     - vState.z() ) * vState.tx();
   double yV = vState.y() + ( m_zMatchY - vState.z() ) * vState.ty();
   yV += vState.ty() * dSlope * dSlope * m_magnetBend;

   double xS = sState.x() + ( zForX     - sState.z() ) * sState.tx();
   double yS = sState.y() + ( m_zMatchY - sState.z() ) * sState.ty();
  
   double distX = xS - xV;
   double distY = yS - yV;
   double dslx  = vState.tx() - sState.tx();
   double tolX = dxTol2 + dslx * dslx *  dxTolSlope2;
   double teta2 = vState.tx() * vState.tx() + vState.ty() * vState.ty();
   double tolY  = m_dyTol * m_dyTol + teta2 * m_dyTolSlope * m_dyTolSlope;  

   chi2 = distX * distX / tolX + distY * distY /tolY;

   double dSlY = sState.ty() - vState.ty();
   chi2 += dSlY * dSlY / sState.errTy2() / 16;
  
   return chi2;
}

//=============================================================================
StatusCode PatMatchL0MuonTool::match( const LHCb::Track& velo,
                                      const LHCb::Track& seed,
                                      LHCb::Track& matchedTrack,
                                      double& quality, double& quality2 ) {

   quality2 = 0;
   //calculate quality of match between tracks
   double chi2 = getChi2Match(velo, seed );

   if ( m_maxChi2 > chi2 ) {
      //set states and flags of output track 
      makeTrack(velo, seed, matchedTrack, chi2 );
      quality = chi2;
      return StatusCode::SUCCESS;
   }
   if(msgLevel( MSG::DEBUG )) debug()<<"matching failed !"<<endmsg;
   return StatusCode::FAILURE;
}

//=============================================================================
void PatMatchL0MuonTool::makeTrack( const LHCb::Track& velo,
                                    const LHCb::Track& seed,
                                    LHCb::Track& output,
                                    double chi2              )
{
   const LHCb::State& vState = velo.closestState( 0.);
   const LHCb::State& sState = seed.closestState( m_zMatchY );
   //== Store the track
   output.addToAncestors( velo );  // set the velo as ancestor
   output.addToAncestors( seed );  // set the seed as ancestor
   output.addToStates( vState );   // copy the state of the Velo
   output.addToStates( sState );   // copy the state of the seed

   //get closest state at 0 and set q over p
   LHCb::State& oState = output.closestState( 0. ); 
  
   double qOverP, sigmaQOverP;
   //FIXME true or false??  
   StatusCode sc = m_fastMomentumTool->calculate( &vState, &sState, 
                                                  qOverP, sigmaQOverP, true );

   if( sc.isFailure() ) Warning( "momentum determination failed!", sc).ignore();
  
   Gaudi::TrackSymMatrix& cov = oState.covariance();
   cov( 4, 4 ) = sigmaQOverP * sigmaQOverP;
   oState.setQOverP( qOverP );
  
   //== Adjust flags
   output.setType(         LHCb::Track::Long  );
   output.setHistory(      LHCb::Track::PatMatch );
   output.setPatRecStatus( LHCb::Track::PatRecIDs   );
   output.setLhcbIDs( velo.lhcbIDs() );   // copy those from the Seed
   output.addInfo( LHCb::Track::MatchChi2, chi2 );

   //== add new LHCbIDs
   for( const LHCb::LHCbID& id: seed.lhcbIDs() ) output.addToLhcbIDs( id );
        
}
