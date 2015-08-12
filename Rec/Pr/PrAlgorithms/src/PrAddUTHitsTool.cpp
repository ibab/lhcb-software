#include <algorithm>
#include <boost/array.hpp>

// Include files
// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"


#include "PrAddUTHitsTool.h"


//-----------------------------------------------------------------------------
// Implementation file for class : PrAddUTHitsTool
//
// 2013-11-13 : Michel De Cian, based on PrAddUTHits
//
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PrAddUTHitsTool )

using ROOT::Math::CholeskyDecomp; // -- Maybe copy locally if not running at CERN?
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrAddUTHitsTool::PrAddUTHitsTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IPrAddUTHitsTool>(this);
  declareProperty( "ZUTField"  , m_zUTField            =  1740. * Gaudi::Units::mm );
  declareProperty( "ZMSPoint"  , m_zMSPoint            =  400. * Gaudi::Units::mm  );
  declareProperty( "UTParam"   , m_utParam             =  29.                      );
  declareProperty( "ZUTProj"   , m_zUTProj             =  2500. * Gaudi::Units::mm );
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
PrAddUTHitsTool::~PrAddUTHitsTool() {}

//=========================================================================
//
//=========================================================================
StatusCode PrAddUTHitsTool::initialize ( ) {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;

  m_utHitManager = tool<Tf::UTStationHitManager <PrUTHit> >("PrUTStationHitManager");

  m_magFieldSvc = svc<ILHCbMagnetSvc>( "MagneticFieldSvc", true );

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Add the UT hits on the track, only the ids.
//=========================================================================
StatusCode PrAddUTHitsTool::addUTHits( LHCb::Track& track){

  LHCb::State state = track.closestState( m_zUTProj );
  PrUTHits myUTHits;
  double chi2 = 0;

  double momentum = track.p();

  StatusCode sc = returnUTHits(state, myUTHits, chi2, momentum);

  if(!sc){

    // ----------------------------------
    if (  msgLevel( MSG::WARNING ) ) Warning("--- Received no hits. Will add no hits to long tracks",sc).ignore();
    // ----------------------------------

    return StatusCode::SUCCESS;
  }

  // -- Only add hits if there are 3 or more
  if( myUTHits.size() < 3 ) return StatusCode::SUCCESS;


  for ( PrUTHits::iterator itT = myUTHits.begin(); myUTHits.end() != itT; ++itT ) {

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
//  Return the UT hits
//=========================================================================
StatusCode PrAddUTHitsTool::returnUTHits( LHCb::State& state, PrUTHits& utHits, double& finalChi2, double p){

  // -- If no momentum is given, use the one from the state
  if(p < 1e-10){
    p = state.p();
  }

  m_utHitManager->prepareHits();

  PrUTHits selected; // -- The container for all the hits compatible with the track
  PrUTHits goodUT; // -- The container with one "group" of hits
  PrUTHits myUtCoords; // -- The container for the final selection of hits

  double bestChi2 = m_maxChi2Tol + m_maxChi2Slope/(p - m_maxChi2POffset);
  double chi2  = 0.;

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "--- Entering returnUTHits ---" << endmsg;

  // -- Get the container with all the hits compatible with the tack
  selectHits(selected, state, p);

  // -- If only two hits are selected, end algorithm
  if ( 3 > selected.size() ){
    utHits.clear();
    utHits = selected;
    finalChi2 = 0;
    return StatusCode::SUCCESS;
  }

  // -- Loop over all hits and make "groups" of hits to form a candidate

  for ( PrUTHits::iterator itBeg = selected.begin(); itBeg+2 < selected.end(); ++itBeg ) {
    double firstProj = (*itBeg)->projection();
    goodUT.clear();
    int nbPlane = 0;
    boost::array<int, 4> firedPlanes;
    std::fill(firedPlanes.begin(), firedPlanes.end(), 0);
    PrUTHits::iterator itEnd = itBeg;

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

      goodUT.push_back( *itEnd++ );
    }


    if ( 3 > nbPlane ) continue; // -- Need at least hits in 3 planes
    // -- group of hits has to be at least as large than best group at this stage
    if ( myUtCoords.size() > goodUT.size() ) continue;

    // ----------------------------------
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "Start fit, first proj " << firstProj << " nbPlane " << nbPlane
              << " size " << goodUT.size() << endmsg;
    // ----------------------------------

    // -- Set variables for the chi2 calculation

    double dist = 0;
    chi2 = 1.e20;
    PrUTHits::iterator worst;


    calculateChi2(goodUT, chi2, bestChi2, dist, p );

    // -- If this group has a better chi2 than all the others
    // -- and is at least as large as all the others, then make this group the new candidate
    if ( bestChi2 > chi2 && goodUT.size() >= myUtCoords.size() ) {

      // ----------------------------------
      if( UNLIKELY( msgLevel(MSG::DEBUG) )) printInfo(goodUT, dist, chi2, state);
      // ----------------------------------
      myUtCoords = goodUT;
      bestChi2 = chi2;
    }
  }

  // -- Assign the final hit container and chi2 to the variables which are returned.
  utHits = myUtCoords;
  finalChi2 = chi2;

  return StatusCode::SUCCESS;
}


//=========================================================================
// Select the hits in a certain window
//=========================================================================
void PrAddUTHitsTool::selectHits(PrUTHits& selected, const LHCb::State& state, const double p){

  // -- Define the tolerance parameters
  double yTol = m_yTolSlope / p ;
  double xTol = m_xTol + m_xTolSlope / p;
  selected.clear();
 
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "State z " << state.z() << " x " << state.x() 
            << " y " << state.y() << " tx " << state.tx() << " ty " 
            << state.ty() << " p " << p << endmsg;

  Tf::UTStationHitManager<PrUTHit>::HitRange hits = m_utHitManager->hits();

  // -- Loop over all the UT hits, get all UT hits whose projection is compatible with the track
  for ( PrUTHits::const_iterator itUT = hits.begin(); hits.end() != itUT; ++itUT ) {
    PrUTHit* ut = *itUT;

    double z = ut->z();
    double yPred = state.y() + ( z - state.z() ) * state.ty();

    // -- Check if hit is compatible in y with the track
    if( !ut->hit()->isYCompatible( yPred, yTol) ) continue ;

    double tyTr = state.ty();
    updateUTHitForTrack( ut, state.y()-state.z()*state.ty(), tyTr );

    double magScaleFactor = -1 * m_magFieldSvc->signedRelativeCurrent();

    // -- Extrapolate the state to the acutal position of the hit, using a two staight line segments and a kink
    double xPred = state.x() + ( z-state.z() ) * state.tx() + m_utParam * magScaleFactor*state.qOverP() * ( z - m_zUTField );

    if ( fabs( xPred - ut->x() ) < xTol ) {

      // -- Calculate the projection, which takes possible multiple scattering into account
      double projDist = ( xPred - ut->x() ) * ( m_zUTProj - m_zMSPoint ) / ( z - m_zMSPoint );
      ut->setProjection( projDist );
      selected.push_back( ut );
    }
  }


  std::sort( selected.begin(), selected.end(), Tf::increasingByProjection<PrUTHit>() );

}
//=========================================================================
// Calculate Chi2
//=========================================================================
void PrAddUTHitsTool::calculateChi2(PrUTHits& goodUT, double& chi2, const double& bestChi2,
                                  double& finalDist, const double& p ){

  // -- Fit a straight line to the points and calculate the chi2 of the hits with respect to the fitted track

  PrUTHits::iterator worst;

  double dist = 0;
  chi2 = 1.e20;

  double xTol = m_xTol + m_xTolSlope / p;
  double fixedWeight = 9./(xTol*xTol);

  const unsigned int nHits = goodUT.size();
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
    mat[2] = fixedWeight * (m_zUTProj-m_zMSPoint)*(m_zUTProj-m_zMSPoint); // -- Fix slope by point at multiple scattering point
    mat[3] = 0.;
    mat[4] = 0.;
    mat[5] = fixedWeight;  // -- Fix Y = 0 with fixedWeight
    rhs[0] = 0.;
    rhs[1] = 0.;
    rhs[2] = 0.;

    for ( PrUTHits::iterator itSel = goodUT.begin(); goodUT.end() != itSel; ++itSel ) {
      PrUTHit* ut = *itSel;
      double w    = ut->hit()->weight();
      double dz   = ut->z() - m_zUTProj;
      double t    = ut->hit()->sinT();
      double dist2= ut->projection();
      mat[0] += w;
      mat[1] += w * dz;
      mat[2] += w * dz * dz;
      mat[3] += w * t;
      mat[4] += w * dz * t ;
      mat[5] += w * t  * t ;
      rhs[0] += w * dist2;
      rhs[1] += w * dist2 * dz;
      rhs[2] += w * dist2 * t ;

      if ( 0 ==  differentPlanes[ut->planeCode()]++ ) ++nDoF;

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
                           (m_zUTProj-m_zMSPoint)*(m_zUTProj-m_zMSPoint)*slope * slope );

    unsigned int nIndep = nDoF;


    for ( PrUTHits::iterator itSel = goodUT.begin(); goodUT.end() != itSel; ++itSel ) {
      PrUTHit* ut = *itSel;
      double w    = ut->hit()->weight();
      double dz   = ut->z() - m_zUTProj;
      dist = ut->projection() - offset - slope * dz - offsetY * ut->hit()->sinT();
      if ( ( 1 < differentPlanes[ut->planeCode()] || nIndep == goodUT.size() ) && worstDiff < w * dist * dist ) {
        worstDiff = w * dist * dist;
        worst = itSel;
      }
      chi2 += w * dist * dist;
    }


    // -- Fix a coverity warning (division by zero)
    if( nDoF > 0 ){
      chi2 /= nDoF;
    }else{
      if (  msgLevel( MSG::DEBUG ) ) debug() << "nDoF is zero" << endmsg;
    }
    
    if (  msgLevel( MSG::DEBUG ) && worstDiff > 0. ) {
      info() << format( " chi2 %10.2f nDoF%2d wors %8.2f proj %6.2f offset %8.3f slope %10.6f offsetY %10.6f",
                        chi2, nDoF, worstDiff, (*worst)->projection(), offset, slope, offsetY)
             << endmsg;
    }

    // -- Remove last point (outlier) if bad fit...
    if ( worstDiff > 0. && bestChi2 < chi2 && goodUT.size() > 3  ) {

      goodUT.erase( worst );
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
void PrAddUTHitsTool::printInfo(const PrUTHits& goodUT, double dist, double chi2, const LHCb::State& state){

  // -- Print some information at the end

  info() << "*** Store this candidate, nbUT = " << goodUT.size() << " chi2 " << chi2 << endmsg;
  for ( PrUTHits::const_iterator itSel = goodUT.begin(); goodUT.end() != itSel; ++itSel ) {
    PrUTHit* ut = *itSel;
    double z     = ut->z();
    double mPred = ut->x() + dist;
    info() << ut->planeCode()
           << format( " z%7.0f  x straight %7.2f pred %7.2f  x %7.2f diff %7.2f ",
                      z, state.x() + state.tx() * ( z-state.z() ), mPred, ut->hit()->xMid(), dist )
           << endmsg;
  }

}
