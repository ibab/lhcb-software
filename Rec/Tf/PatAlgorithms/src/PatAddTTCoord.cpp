#include <algorithm>
#include <array>

// Include files
// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "PatAddTTCoord.h"


//-----------------------------------------------------------------------------
// Implementation file for class : PatAddTTCoord
//
// 2010-02-12 : Michel De Cian, based on Olivier Callot's PatAddTTCoord
//
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PatAddTTCoord )

using ROOT::Math::CholeskyDecomp; // -- Maybe copy locally if not running at CERN?
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatAddTTCoord::PatAddTTCoord( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_bendProtoParam(0.0)
{
  declareInterface<IAddTTClusterTool>(this);
  declareProperty( "ZTTField"  , m_zTTField            =  1740. * Gaudi::Units::mm );
  declareProperty( "ZMSPoint"  , m_zMSPoint            =  400. * Gaudi::Units::mm  );
  declareProperty( "TTParam"   , m_ttParam             =  29.                      );
  declareProperty( "ZTTProj"   , m_zTTProj             =  2500. * Gaudi::Units::mm );
  declareProperty( "MaxChi2Tol" , m_maxChi2Tol         =  2.0                      );
  declareProperty( "MaxChi2Slope", m_maxChi2Slope      =  25000                    );
  declareProperty( "MaxChi2POffset", m_maxChi2POffset  =  100                      );
  declareProperty( "YTolSlope" , m_yTolSlope           =  20000.                   );
  declareProperty( "XTol"      , m_xTol                =  1.0                      );
  declareProperty( "XTolSlope" , m_xTolSlope           =  30000.0                  );
  declareProperty( "MajAxProj" , m_majAxProj           =  20.0  * Gaudi::Units::mm   );
  declareProperty( "MinAxProj" , m_minAxProj           =  2.0   * Gaudi::Units::mm   );

}
//=============================================================================
// Destructor
//=============================================================================
PatAddTTCoord::~PatAddTTCoord() {}

//=========================================================================
//
//=========================================================================
StatusCode PatAddTTCoord::initialize ( ) {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;

  m_ttHitManager = tool<Tf::TTStationHitManager <PatTTHit> >("PatTTStationHitManager");

  m_magFieldSvc = svc<ILHCbMagnetSvc>( "MagneticFieldSvc", true );

  incSvc()->addListener(this, IncidentType::BeginEvent);
  m_newEvent = true;

  m_invMajAxProj2 = 1/(m_majAxProj*m_majAxProj);

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Add the TT hits on the track, only the ids.
//=========================================================================
StatusCode PatAddTTCoord::addTTClusters( LHCb::Track& track){

  LHCb::State state = track.closestState( m_zTTProj );
  PatTTHits myTTHits;
  myTTHits.reserve(4);
  double chi2 = 0;

  StatusCode sc = returnTTClusters(state, myTTHits, chi2, track.p());

  if(!sc){

    // ----------------------------------
    if (  msgLevel( MSG::WARNING ) ) Warning("--- Received no hits. Will add no hits to long tracks",sc).ignore();
    // ----------------------------------

    return StatusCode::SUCCESS;
  }

  // -- Only add hits if there are 3 or more
  if( myTTHits.size() < 3 ) return StatusCode::SUCCESS;

  PatTTHits::const_iterator itEnd = myTTHits.end();
  for ( PatTTHits::const_iterator itT = myTTHits.begin(); itEnd != itT; ++itT ) {

    // ----------------------------------
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "--- Adding Hit in Layer: " << (*itT)->planeCode()
              << " with projection: " << (*itT)->projection() << endmsg;
    // ----------------------------------

    track.addToLhcbIDs( (*itT)->hit()->lhcbID() );
    (*itT)->hit()->setUsed( true );
  }

  counter("#TT hits added") += myTTHits.size();
  counter("#tracks with hits added")++;

  return StatusCode::SUCCESS;

}
//=========================================================================
//  Return the TT hits
//=========================================================================
StatusCode PatAddTTCoord::returnTTClusters( LHCb::State& state, PatTTHits& ttHits, double& finalChi2, double p){

  // -- If no momentum is given, use the one from the state
  if(p < 1e-10){
    p = state.p();
  }

  m_selected.clear(); // -- The container for all the hits compatible with the track
  m_goodTT.clear(); // -- The container with one "group" of hits
  ttHits.clear(); // -- The container for the final selection of hits

  double bestChi2 = m_maxChi2Tol + m_maxChi2Slope/(p - m_maxChi2POffset);
  double chi2  = 0.;

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "--- Entering addTTClusters ---" << endmsg;

  if(m_newEvent) initEvent();

  // -- Get the container with all the hits compatible with the tack
  selectHits(state, p);

  // -- If only two hits are selected, end algorithm
  if ( 3 > m_selected.size() ){
    ttHits = m_selected; 
    finalChi2 = 0;
    return StatusCode::SUCCESS;
  }

  // -- Loop over all hits and make "groups" of hits to form a candidate
  std::array<int, 4> firedPlanes;
  PatTTHits::const_iterator end = m_selected.end();
  for ( PatTTHits::iterator itBeg = m_selected.begin(); itBeg+2 < end; ++itBeg ) {
    const double firstProj = (*itBeg)->projection();
    m_goodTT.clear();
    int nbPlane = 0;
    firedPlanes.fill(0);
    PatTTHits::iterator itEnd = itBeg;

    // -- If |firstProj| > m_majAxProj, the sqrt is ill defined
    double maxProj = firstProj;
    if(fabs(firstProj) < m_majAxProj){
      // -- m_invMajAxProj2 = 1/(m_majAxProj*m_majAxProj), but it's faster like this
      maxProj =  firstProj + sqrt( m_minAxProj * m_minAxProj * (1 - firstProj*firstProj * m_invMajAxProj2 ));
    }
    
    // -- This means that there would be less than 3 hits, which does not work, so we can skip this right away
    if( (*(itBeg+2))->projection() > maxProj ) continue;

    // -- Make "group" of hits which are within a certain distance to the first hit of the group
    while ( itEnd != m_selected.end() ) {

      if ( (*itEnd)->projection() > maxProj ) break;

      if ( 0 == firedPlanes[ (*itEnd)->planeCode() ] ) {
        firedPlanes[ (*itEnd)->planeCode() ] = 1; // -- Count number of fired planes
        ++nbPlane;
      }

      m_goodTT.push_back( *itEnd++ );
    }


    if ( 3 > nbPlane ) continue; // -- Need at least hits in 3 planes
    // -- group of hits has to be at least as large than best group at this stage
    if ( ttHits.size() > m_goodTT.size() ) continue;

    // ----------------------------------
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "Start fit, first proj " << firstProj << " nbPlane " << nbPlane
              << " size " << m_goodTT.size() << endmsg;
    // ----------------------------------

    // -- Set variables for the chi2 calculation

    double dist = 0;
    chi2 = 1.e20;
    
    calculateChi2(chi2, bestChi2, dist, p );

    // -- If this group has a better chi2 than all the others
    // -- and is at least as large as all the others, then make this group the new candidate
    if ( bestChi2 > chi2 && m_goodTT.size() >= ttHits.size() ) {

      // ----------------------------------
      if( UNLIKELY( msgLevel(MSG::DEBUG) )) printInfo(dist, chi2, state);
      // ----------------------------------
      ttHits = m_goodTT;
      bestChi2 = chi2;
    }
  }

  // -- Assign the final hit container and chi2 to the variables which are returned.
  finalChi2 = bestChi2;

  return StatusCode::SUCCESS;
}
//=========================================================================
// Select the hits in a certain window
//=========================================================================
void PatAddTTCoord::selectHits(const LHCb::State& state, const double p){

  // -- Define the tolerance parameters
  const double yTol = m_yTolSlope / p ;
  const double xTol = m_xTol + m_xTolSlope / p;
  m_selected.clear();

  // -- Define the parameter that describes the bending
  const double bendParam = m_bendProtoParam * state.qOverP();
  
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "State z " << state.z() << " x " << state.x() 
            << " y " << state.y() << " tx " << state.tx() << " ty " 
            << state.ty() << " p " << p << endmsg;

  const double stateX = state.x();
  const double stateZ = state.z();
  const double stateTy = state.ty();
  const double stateY = state.y();
  const double stateTx = state.tx();
  
  for(int iStation = 0; iStation < 2; ++iStation){
    for(int iLayer = 0; iLayer < 2; ++iLayer){
  
      Tf::TTStationHitManager<PatTTHit>::HitRange range =  m_ttHitManager->sortedLayerHits<Tf::increasingByXAtYEq0<PatTTHit>>(iStation,iLayer);
      
      const double zLayer = range.front()->z();
      const double yPredLay = stateY + ( zLayer -stateZ ) * stateTy;
      const double xPredLay = stateX + ( zLayer -stateZ ) * stateTx + bendParam * ( zLayer - m_zTTField );
      const double dxDy = range.front()->hit()->dxDy();
      
      // -- this should sort of take the stereo angle and some tolerance into account.
      const double lowerBoundX = xPredLay - xTol - dxDy*yPredLay - 2.0;
      
      PatTTHits::const_iterator itHit = std::lower_bound( range.begin(), 
                                                          range.end(), 
                                                          lowerBoundX, 
                                                          Tf::compByX_LB< PatTTHit >() );
    
      for( ; itHit != range.end(); ++itHit){
        
        PatTTHit* hit = *itHit;
        
        const double yPred = stateY + ( hit->z() -stateZ ) * stateTy;
        
        if( !hit->hit()->isYCompatible(yPred, yTol)) continue;
        
        // -- only do this if it's a stereo layer
        if( iLayer+iStation == 1){
          updateTTHitForTrack( hit, stateY-stateZ*stateTy, stateTy );
        }
        
        const double xPred = stateX + ( hit->z()-stateZ ) * stateTx + bendParam * ( hit->z() - m_zTTField );
        if( hit->x() > xPred + xTol ) break;
        if( hit->x() < xPred - xTol ) continue;
        
        const double projDist = ( xPred - hit->x() ) * ( m_zTTProj - m_zMSPoint ) / ( hit->z() - m_zMSPoint );
        hit->setProjection( projDist );
        m_selected.push_back( hit );
      }
    
      // -- would not have hits in 3 layers like this
      if( iStation == 1 && m_selected.empty() ) break;
      
    }
  }
  
  // -- No need to sort if we anyway don't have enough hits
  if( m_selected.size() < 3 ) return;
  
  std::sort( m_selected.begin(), m_selected.end(), Tf::increasingByProjection<PatTTHit>() );
  
}
//=========================================================================
// Calculate Chi2
//=========================================================================
void PatAddTTCoord::calculateChi2(double& chi2, const double& bestChi2,
                                  double& finalDist, const double& p){

  // -- Fit a straight line to the points and calculate the chi2 of the hits with respect to the fitted track

  PatTTHits::iterator worst;

  double dist = 0;
  chi2 = 1.e20;

  const double xTol = m_xTol + m_xTolSlope / p;
  const double fixedWeight = 9./(xTol*xTol);

  unsigned int nHits = m_goodTT.size();
  const unsigned int maxIterations = nHits;
  unsigned int counter = 0;

  // -- Loop until chi2 has a reasonable value or no more outliers can be removed to improve it 
  // -- (with the counter as a sanity check to avoid infinite loops).

  unsigned int nDoF = 0;
  std::array<unsigned int, 4> differentPlanes;
  differentPlanes.fill(0);
  double worstDiff = -1.0;
  double mat[6], rhs[3];
  
  mat[0] = fixedWeight; // -- Fix X = 0 with fixedWeight
  mat[1] = 0.;
  mat[2] = fixedWeight * (m_zTTProj-m_zMSPoint)*(m_zTTProj-m_zMSPoint); // -- Fix slope by point at multiple scattering point
  mat[3] = 0.;
  mat[4] = 0.;
  mat[5] = fixedWeight;  // -- Fix Y = 0 with fixedWeight
  rhs[0] = 0.;
  rhs[1] = 0.;
  rhs[2] = 0.;

  PatTTHits::const_iterator itEnd = m_goodTT.end();
  for ( PatTTHits::iterator itSel = m_goodTT.begin(); itEnd != itSel; ++itSel ) {
    const PatTTHit* tt = *itSel;
    const double w     = tt->hit()->weight();
    const double dz    = tt->z() - m_zTTProj;
    const double t     = tt->hit()->sinT();
    const double dist2 = tt->projection();
    mat[0] += w;
    mat[1] += w * dz;
    mat[2] += w * dz * dz;
    mat[3] += w * t;
    mat[4] += w * dz * t ;
    mat[5] += w * t  * t ;
    rhs[0] += w * dist2;
    rhs[1] += w * dist2 * dz;
    rhs[2] += w * dist2 * t ;
    
    if ( 0 ==  differentPlanes[tt->planeCode()]++ ) ++nDoF;
    
  }

  // -- Loop to remove outliers
  // -- Don't loop more often than number of hits in the selection
  // -- The counter protects infinite loops in very rare occasions.
  while( chi2 > 1e10 && counter < maxIterations){

    worstDiff = -1.0;
    
    // -- This is needed since 'CholeskyDecomp' overwrites rhs
    // -- which is needed later on
    const double saveRhs[3] = { rhs[0], rhs[1], rhs[2] };
    
    CholeskyDecomp<double, 3> decomp(mat);
    if (!decomp) {
      chi2 = 1e42;
      break;
    } else {
      decomp.Solve(rhs);
    }
    
    const double offset  = rhs[0];
    const double slope   = rhs[1];
    const double offsetY = rhs[2];
    
    rhs[0] = saveRhs[0];
    rhs[1] = saveRhs[1];
    rhs[2] = saveRhs[2];

    chi2 = fixedWeight * ( offset * offset +
                           offsetY * offsetY +
                           (m_zTTProj-m_zMSPoint)*(m_zTTProj-m_zMSPoint)*slope * slope );
    
    
    itEnd = m_goodTT.end();
    for ( PatTTHits::iterator itSel = m_goodTT.begin(); itEnd!= itSel; ++itSel ) {
      const PatTTHit* tt = *itSel;
      const double w    = tt->hit()->weight();
      const double dz   = tt->z() - m_zTTProj;
      dist = tt->projection() - offset - slope * dz - offsetY * tt->hit()->sinT();
      if ( ( 1 < differentPlanes[tt->planeCode()] || nDoF == nHits ) && worstDiff < w * dist * dist ) {
        worstDiff = w * dist * dist;
        worst = itSel;
      }
      chi2 += w * dist * dist;
    }
    
    chi2 /= nDoF;
    
    if (   UNLIKELY( msgLevel( MSG::DEBUG )) && worstDiff > 0. ) {
      info() << format( " chi2 %10.2f nDoF%2d wors %8.2f proj %6.2f offset %8.3f slope %10.6f offsetY %10.6f",
                        chi2, nDoF, worstDiff, (*worst)->projection(), offset, slope, offsetY)
             << endmsg;
    }
    
    // -- Remove last point (outlier) if bad fit...
    if ( worstDiff > 0. && bestChi2 < chi2 ) {
      
      const PatTTHit* tt = *worst;
      const double w     = tt->hit()->weight();
      const double dz    = tt->z() - m_zTTProj;
      const double t     = tt->hit()->sinT();
      const double dist2 = tt->projection();
      mat[0] -= w;
      mat[1] -= w * dz;
      mat[2] -= w * dz * dz;
      mat[3] -= w * t;
      mat[4] -= w * dz * t ;
      mat[5] -= w * t  * t ;
      rhs[0] -= w * dist2;
      rhs[1] -= w * dist2 * dz;
      rhs[2] -= w * dist2 * t ;
    
      if ( 1 ==  differentPlanes[tt->planeCode()]-- ) --nDoF;
      --nHits;
      
      m_goodTT.erase( worst );
      chi2 = 1.e11;  // -- Start new iteration
    }
    
    // -- Increase the sanity check counter
    ++counter;
  }
  


  finalDist = dist;


}
//=========================================================================
// Print out info
//=========================================================================
void PatAddTTCoord::printInfo(double dist, double chi2, const LHCb::State& state){

  // -- Print some information at the end

  info() << "*** Store this candidate, nbTT = " << m_goodTT.size() << " chi2 " << chi2 << endmsg;
  for ( PatTTHits::const_iterator itSel = m_goodTT.begin(); m_goodTT.end() != itSel; ++itSel ) {
    PatTTHit* tt = *itSel;
    double z     = tt->z();
    double mPred = tt->x() + dist;
    info() << tt->planeCode()
           << format( " z%7.0f  x straight %7.2f pred %7.2f  x %7.2f diff %7.2f ",
                      z, state.x() + state.tx() * ( z-state.z() ), mPred, tt->hit()->xMid(), dist )
           << endmsg;
  }

}


//=============================================================================
// -- Check if new event has occurred. If yes, set flag
// -- Note: The actions of initEvent cannot be executed here,
// -- as this handle runs before decoding the clusters
//=============================================================================
void PatAddTTCoord::handle ( const Incident& incident ) {
  
  if ( IncidentType::BeginEvent == incident.type() ) m_newEvent = true;
  
}
 
//=============================================================================
// -- Init event: Get the new hits and sort them
//=============================================================================
void PatAddTTCoord::initEvent () {
 
  m_ttHitManager->prepareHits();
  
  // -- This does not change between events
  m_bendProtoParam = m_ttParam * -1 * m_magFieldSvc->signedRelativeCurrent() ;
  
  m_newEvent = false;
  
  
}
