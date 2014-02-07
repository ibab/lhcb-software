#include <algorithm>
#include <boost/array.hpp>

// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
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
  : GaudiTool ( type, name , parent )
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

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Add the TT hits on the track, only the ids.
//=========================================================================
StatusCode PatAddTTCoord::addTTClusters( LHCb::Track& track){

  LHCb::State state = track.closestState( m_zTTProj );
  PatTTHits myTTHits;
  double chi2 = 0;

  double momentum = track.p();

  StatusCode sc = returnTTClusters(state, myTTHits, chi2, momentum);

  if(!sc){

    // ----------------------------------
    if (  msgLevel( MSG::WARNING ) ) Warning("--- Received no hits. Will add no hits to long tracks",sc).ignore();
    // ----------------------------------

    return StatusCode::SUCCESS;
  }

  // -- Only add hits if there are 3 or more
  if( myTTHits.size() < 3 ) return StatusCode::SUCCESS;


  for ( PatTTHits::iterator itT = myTTHits.begin(); myTTHits.end() != itT; ++itT ) {

    // ----------------------------------
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "--- Adding Hit in Layer: " << (*itT)->planeCode()
              << " with projection: " << (*itT)->projection() << endmsg;
    // ----------------------------------

    track.addToLhcbIDs( (*itT)->hit()->lhcbID() );
    (*itT)->hit()->setUsed( true );
  }

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

  m_ttHitManager->prepareHits();

  PatTTHits selected; // -- The container for all the hits compatible with the track
  PatTTHits goodTT; // -- The container with one "group" of hits
  PatTTHits myTtCoords; // -- The container for the final selection of hits

  double bestChi2 = m_maxChi2Tol + m_maxChi2Slope/(p - m_maxChi2POffset);
  double chi2  = 0.;

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "--- Entering addTTClusters ---" << endmsg;

  // -- Get the container with all the hits compatible with the tack
  selectHits(selected, state, p);

  // -- If only two hits are selected, end algorithm
  if ( 3 > selected.size() ){
    ttHits.clear();
    ttHits = selected;
    finalChi2 = 0;
    return StatusCode::SUCCESS;
  }

  // -- Loop over all hits and make "groups" of hits to form a candidate

  for ( PatTTHits::iterator itBeg = selected.begin(); itBeg+2 < selected.end(); ++itBeg ) {
    double firstProj = (*itBeg)->projection();
    goodTT.clear();
    int nbPlane = 0;
    boost::array<int, 4> firedPlanes;
    std::fill(firedPlanes.begin(), firedPlanes.end(), 0);
    PatTTHits::iterator itEnd = itBeg;

    // -- If |firstProj| > m_majAxProj, the sqrt is ill defined
    double maxProj = firstProj;
    if(fabs(firstProj) < m_majAxProj){
      maxProj =  firstProj + sqrt( m_minAxProj * m_minAxProj * (1 - firstProj*firstProj/( m_majAxProj * m_majAxProj )));
    }

    // -- Make "group" of hits which are within a certain distance to the first hit of the group
    while ( itEnd != selected.end() ) {

      if ( (*itEnd)->projection() > maxProj ) break;

      if ( 0 == firedPlanes[ (*itEnd)->planeCode() ] ) {
        firedPlanes[ (*itEnd)->planeCode() ] = 1; // -- Count number of fired planes
        nbPlane++;
      }

      goodTT.push_back( *itEnd++ );
    }


    if ( 3 > nbPlane ) continue; // -- Need at least hits in 3 planes
    // -- group of hits has to be at least as large than best group at this stage
    if ( myTtCoords.size() > goodTT.size() ) continue;

    // ----------------------------------
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "Start fit, first proj " << firstProj << " nbPlane " << nbPlane
              << " size " << goodTT.size() << endmsg;
    // ----------------------------------

    // -- Set variables for the chi2 calculation

    double dist = 0;
    chi2 = 1.e20;
    PatTTHits::iterator worst;


    calculateChi2(goodTT, chi2, bestChi2, dist, p );

    // -- If this group has a better chi2 than all the others
    // -- and is at least as large as all the others, then make this group the new candidate
    if ( bestChi2 > chi2 && goodTT.size() >= myTtCoords.size() ) {

      // ----------------------------------
      if( UNLIKELY( msgLevel(MSG::DEBUG) )) printInfo(goodTT, dist, chi2, state);
      // ----------------------------------
      myTtCoords = goodTT;
      bestChi2 = chi2;
    }
  }

  // -- Assign the final hit container and chi2 to the variables which are returned.
  ttHits = myTtCoords;
  finalChi2 = chi2;

  return StatusCode::SUCCESS;
}


//=========================================================================
// Select the hits in a certain window
//=========================================================================
void PatAddTTCoord::selectHits(PatTTHits& selected, const LHCb::State& state, const double p){

  // -- Define the tolerance parameters
  double yTol = m_yTolSlope / p ;
  double xTol = m_xTol + m_xTolSlope / p;
  selected.clear();

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "State z " << state.z() << " x " << state.x() 
            << " y " << state.y() << " tx " << state.tx() << " ty " 
            << state.ty() << " p " << p << endmsg;

  Tf::TTStationHitManager<PatTTHit>::HitRange hits = m_ttHitManager->hits();

  // -- Loop over all the TT hits, get all TT hits whose projection is compatible with the track
  for ( PatTTHits::const_iterator itTT = hits.begin(); hits.end() != itTT; ++itTT ) {
    PatTTHit* tt = *itTT;

    double z = tt->z();
    double yPred = state.y() + ( z - state.z() ) * state.ty();

    // -- Check if hit is compatible in y with the track
    if( !tt->hit()->isYCompatible( yPred, yTol) ) continue ;

    double tyTr = state.ty();
    updateTTHitForTrack( tt, state.y()-state.z()*state.ty(), tyTr );

    double magScaleFactor = -1 * m_magFieldSvc->signedRelativeCurrent();

    // -- Extrapolate the state to the acutal position of the hit, using a two staight line segments and a kink
    double xPred = state.x() + ( z-state.z() ) * state.tx() + m_ttParam * magScaleFactor*state.qOverP() * ( z - m_zTTField );

    if ( fabs( xPred - tt->x() ) < xTol ) {

      // -- Calculate the projection, which takes possible multiple scattering into account
      double projDist = ( xPred - tt->x() ) * ( m_zTTProj - m_zMSPoint ) / ( z - m_zMSPoint );
      tt->setProjection( projDist );
      selected.push_back( tt );
    }
  }


  std::sort( selected.begin(), selected.end(), Tf::increasingByProjection<PatTTHit>() );

}
//=========================================================================
// Calculate Chi2
//=========================================================================
void PatAddTTCoord::calculateChi2(PatTTHits& goodTT, double& chi2, const double& bestChi2,
                                  double& finalDist, const double& p ){

  // -- Fit a straight line to the points and calculate the chi2 of the hits with respect to the fitted track

  PatTTHits::iterator worst;

  double dist = 0;
  chi2 = 1.e20;

  double xTol = m_xTol + m_xTolSlope / p;
  double fixedWeight = 9./(xTol*xTol);

  const unsigned int nHits = goodTT.size();
  unsigned int counter = 0;

  // -- Loop until chi2 has a reasonable value or no more outliers can be removed to improve it 
  // -- (with the counter as a sanity check to avoid infinite loops).

  while ( 1.e10 < chi2 && counter < nHits) {

    int nDoF = 0;
    boost::array<int, 4> differentPlanes;
    std::fill(differentPlanes.begin(), differentPlanes.end(), 0);
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

    for ( PatTTHits::iterator itSel = goodTT.begin(); goodTT.end() != itSel; ++itSel ) {
      PatTTHit* tt = *itSel;
      double w    = tt->hit()->weight();
      double dz   = tt->z() - m_zTTProj;
      double t    = tt->hit()->sinT();
      double dist2= tt->projection();
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

    CholeskyDecomp<double, 3> decomp(mat);
    if (!decomp) {
      chi2 = 1e42;
      break;
    } else {
      decomp.Solve(rhs);
    }

    double offset = rhs[0];
    double slope = rhs[1];
    double offsetY = rhs[2];

    chi2 = fixedWeight * ( offset * offset +
                           offsetY * offsetY +
                           (m_zTTProj-m_zMSPoint)*(m_zTTProj-m_zMSPoint)*slope * slope );

    unsigned int nIndep = nDoF;


    for ( PatTTHits::iterator itSel = goodTT.begin(); goodTT.end() != itSel; ++itSel ) {
      PatTTHit* tt = *itSel;
      double w    = tt->hit()->weight();
      double dz   = tt->z() - m_zTTProj;
      dist = tt->projection() - offset - slope * dz - offsetY * tt->hit()->sinT();
      if ( ( 1 < differentPlanes[tt->planeCode()] || nIndep == goodTT.size() ) && worstDiff < w * dist * dist ) {
        worstDiff = w * dist * dist;
        worst = itSel;
      }
      chi2 += w * dist * dist;
    }


    chi2 /= nDoF;

    if (  msgLevel( MSG::DEBUG ) && worstDiff > 0. ) {
      info() << format( " chi2 %10.2f nDoF%2d wors %8.2f proj %6.2f offset %8.3f slope %10.6f offsetY %10.6f",
                        chi2, nDoF, worstDiff, (*worst)->projection(), offset, slope, offsetY)
             << endmsg;
    }

    // -- Remove last point (outlier) if bad fit...
    if ( worstDiff > 0. && bestChi2 < chi2 && goodTT.size() > 3  ) {

      goodTT.erase( worst );
      chi2 = 1.e11;  // -- Start new iteration

    }


    // -- Increase the sanity check counter
    counter++;
  }


  finalDist = dist;


}

//=========================================================================
// Print out info
//=========================================================================
void PatAddTTCoord::printInfo(const PatTTHits& goodTT, double dist, double chi2, const LHCb::State& state){

  // -- Print some information at the end

  info() << "*** Store this candidate, nbTT = " << goodTT.size() << " chi2 " << chi2 << endmsg;
  for ( PatTTHits::const_iterator itSel = goodTT.begin(); goodTT.end() != itSel; ++itSel ) {
    PatTTHit* tt = *itSel;
    double z     = tt->z();
    double mPred = tt->x() + dist;
    info() << tt->planeCode()
           << format( " z%7.0f  x straight %7.2f pred %7.2f  x %7.2f diff %7.2f ",
                      z, state.x() + state.tx() * ( z-state.z() ), mPred, tt->hit()->xMid(), dist )
           << endmsg;
  }

}
