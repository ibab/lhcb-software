// $Id: TrajPoca.cpp,v 1.8 2007-11-19 08:12:57 mneedham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// Math Definitions
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/SystemOfUnits.h"

// local
#include "TrajPoca.h"

DECLARE_TOOL_FACTORY( TrajPoca );

//=============================================================================
/// Standard constructor, initializes variables
//=============================================================================
TrajPoca::TrajPoca( const std::string& type,
                    const std::string& name,
                    const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITrajPoca>(this);

  declareProperty( "MaxnOscillStep",    m_maxnOscillStep = 5    );
  declareProperty( "MaxnDivergingStep", m_maxnDivergingStep = 5 );
  declareProperty( "MaxnStuck",         m_maxnStuck = 3         );
  declareProperty( "MaxnTry",           m_maxnTry = 100         );
  declareProperty( "MaxDist",           m_maxDist = 100000000   );
  declareProperty( "MaxExtrapTolerance", m_maxExtrapTolerance = 1*Gaudi::Units::cm) ;
};

//=============================================================================
// Default destructor
//=============================================================================
TrajPoca::~TrajPoca() {};


//=============================================================================
// Find mus along trajectories having a distance smaller than tolerance
//=============================================================================
StatusCode TrajPoca::minimize( const LHCb::Trajectory& traj1,
                               double& mu1, 
                               bool restrictRange1,
                               const LHCb::Trajectory& traj2,
                               double& mu2, 
                               bool restrictRange2, 
                               Gaudi::XYZVector& distance,
                               double precision ) const
{
  StatusCode status = StatusCode::SUCCESS;

  static Gaudi::XYZPoint newPos1, newPos2;
  double delta(0), prevdelta(0);
  int nOscillStep(0);
  int nDivergingStep(0);
  int nStuck(0);
  bool finished = false;

  for( int istep = 0; istep < m_maxnTry && !finished; ++istep ) {
    double prevflt1      = mu1;
    double prevflt2      = mu2;
    double prevprevdelta = prevdelta;
    prevdelta = delta ;
    status = stepTowardPoca( traj1, mu1, restrictRange1,
                             traj2, mu2, restrictRange2, precision );
    if( !status ) break; // Parallel Trajectories in stepTowardPoca

    newPos1  = traj1.position( mu1 );
    newPos2  = traj2.position( mu2 );
    distance = ( newPos1 - newPos2 );
    delta    = distance.R();
    double step1 = mu1 - prevflt1;
    double step2 = mu2 - prevflt2;
    int pathDir1 = ( step1 > 0. ) ? 1 : -1;
    int pathDir2 = ( step2 > 0. ) ? 1 : -1;
    // Can we stop stepping?
    double distToErr1 = traj1.distTo1stError( prevflt1, precision, pathDir1 );
    double distToErr2 = traj2.distTo1stError( prevflt2, precision, pathDir2 );
    // converged if very small steps
    finished = ( fabs( step1 ) < distToErr1 && fabs( step2 ) < distToErr2 );
    
    // we have to catch some problematic cases
    if( !finished && istep > 2 && delta > prevdelta ) {
      // we can get stuck if a flt range is restricted
      if( restrictRange1 && fabs(step1) > 1.0e-10 || restrictRange2 && fabs(step2) > 1e-10 ) {
        if( ++nStuck > m_maxnStuck ) {
          // downgrade to a point poca
          Gaudi::XYZVector dist = Gaudi::XYZVector( 0., 0., 0. );
          restrictRange2 ? 
            minimize( traj1, mu1, restrictRange1, newPos2, dist, precision )
            : minimize( traj2, mu2, restrictRange2, newPos1, dist, precision );
          if( msgLevel( MSG::DEBUG ) ) {
            debug() << "Minimization got stuck." << endmsg;
          }
          status = StatusCode::SUCCESS; // "Stuck poca"
          finished = true;
        }
      }
      else if( prevdelta > prevprevdelta ) {
        // diverging
        if( ++nDivergingStep > m_maxnDivergingStep ) {
          warning() << "Minimization was diverging." << endreq;
          status = StatusCode::SUCCESS; // "Failed to converge"
          finished = true ;
        }
      }
      else {
        nDivergingStep = 0;
        // oscillating
        if( ++nOscillStep > m_maxnOscillStep ) {
          // bail out of oscillation. since the previous step was
          // better, use that one.
          mu1 = prevflt1;
          mu2 = prevflt2;
          if( msgLevel( MSG::DEBUG ) ) {
            debug() << "Minimization bailed out of oscillation." << endmsg;
          }
          status = StatusCode::SUCCESS; // "Oscillating poca"
          finished = true;
        }
        else {
          // we might be oscillating, but we could also just have
          // stepped over the minimum. choose a solution `in
          // between'.
          mu1 = prevflt1 + 0.5 * step1 ;
          if( restrictRange1 ) restrictToRange(mu1,traj1) ;
          mu2 = prevflt2 + 0.5 * step2 ;
          if( restrictRange2 ) restrictToRange(mu2,traj2) ;
          newPos1    = traj1.position( mu1 );
          newPos2    = traj2.position( mu2 );
          distance   = ( newPos1 - newPos2 ); 
          delta      = distance.R();
        }
      }
    }
  }

  if( !finished ) {
    warning() << "Minimization did not converge." << endmsg;
    status = StatusCode::FAILURE;
  }
  
  return status;
};

//=============================================================================
// 
//=============================================================================
StatusCode TrajPoca::minimize( const LHCb::Trajectory& traj,
                               double& mu,
                               bool restrictRange,
                               const Gaudi::XYZPoint& pt,
                               Gaudi::XYZVector& distance,
                               double /*precision*/ ) const
{
  // this does not work for non-linear Trajectories!
  mu = traj.muEstimate( pt ) ;
  if(restrictRange) restrictToRange( mu, traj ) ;
  distance = traj.position( mu ) - pt;
  return StatusCode::SUCCESS;
};

//=============================================================================
// 
//=============================================================================
StatusCode TrajPoca::stepTowardPoca( const LHCb::Trajectory& traj1,
                                     double& mu1,
                                     bool restrictRange1,
                                     const LHCb::Trajectory& traj2,
                                     double& mu2,
                                     bool restrictRange2,
                                     double tolerance ) const
{
  // a bunch of ugly, unitialized statics 
  // -- but, believe me, it really is faster this way...
  // (assignment is faster than c'tor...)
  static Gaudi::XYZVector dir1, dir2;
  static Gaudi::XYZVector delDir1, delDir2;
  static Gaudi::XYZPoint  pos1, pos2;

  traj1.expansion( mu1, pos1, dir1, delDir1 );
  traj2.expansion( mu2, pos2, dir2, delDir2 );
  Gaudi::XYZVector delta( pos1 - pos2 );
  double ua  = -delta.Dot( dir1 );
  double ub  =  delta.Dot( dir2 );
  double caa =  dir1.mag2() + delta.Dot( delDir1 );
  double cbb =  dir2.mag2() - delta.Dot( delDir2 );
  double cab = -dir1.Dot( dir2 );
  double det =  caa * cbb - cab * cab;
  if( det < 0. ) {
    // get rid of second order terms, and try again...
    caa = dir1.mag2();
    cbb = dir2.mag2();
    det = caa * cbb - cab * cab;
  }

  // If they are parallel (in quadratic approximation) give up
  if( det < 1.e-8 ) {
    if( msgLevel( MSG::DEBUG ) ) {
      debug() << "The Trajectories are parallel." << endmsg;    
    }
    return StatusCode::FAILURE;
  }
  
  double df1   = ( ua * cbb - ub * cab ) / det;
  int pathDir1 = ( df1 > 0 ) ? 1 : -1;
  double df2   = ( ub * caa - ua * cab ) / det;
  int pathDir2 = ( df2 > 0 ) ? 1 : -1;
    
  // Don't try going further than worst parabolic approximation will
  // allow. The tolerance is set by 'deltadoca', the expected
  // improvement in the doca. We bound this by tolerance from below
  // and maxExtrapTolerance from above.
  double deltadoca = sqrt( fabs(ua * df1) + fabs(ub * df2) ) ; // fabs only because of machine precision issues.
  double extraptolerance = std::min(std::max(deltadoca,tolerance),m_maxExtrapTolerance) ;
  const double smudge = 1.01; // Factor to push just over border of piecewise traj (essential!)
  double distToErr1 = smudge * traj1.distTo2ndError( mu1, extraptolerance, pathDir1 );
  double distToErr2 = smudge * traj2.distTo2ndError( mu2, extraptolerance, pathDir2 );

  // Factor to push just over border of piecewise traj (essential!)
  if( fabs( df1 ) > distToErr1 ) {
    // choose solution for which df1 steps just over border
    df1 = distToErr1 * pathDir1;
    // now recalculate df2, given df1:
    df2 = ( ub - df1 * cab ) / cbb;
  }

  if( fabs( df2 ) > distToErr2 ) {
    // choose solution for which df2 steps just over border
    df2 = distToErr2 * pathDir2;
    // now recalculate df1, given df2:
    df1 = ( ua - df2 * cab ) / cbb;
    // if still not okay,
    if( fabs( df1 ) > distToErr1 ) {
      df1 = distToErr1 * pathDir1;
    }
  }

  mu1 += df1 ;
  mu2 += df2 ;

  // these do not make any sense here. either we need to merge them with the lines above that restrict to the validity of the
  // expansion, or we need to move them out of here entirely.
  if( restrictRange1 ) restrictToRange(mu1,traj1) ;
  if( restrictRange2 ) restrictToRange(mu2, traj2) ;
  
  // another check for parallel trajectories
  if( fabs( mu1 ) > m_maxDist && fabs( mu2 ) > m_maxDist ) {
    if( msgLevel( MSG::DEBUG ) ) {
      debug() << "Stepped further than MaxDist." << endmsg;    
    }
    return StatusCode::FAILURE;
  }
    
  return StatusCode::SUCCESS;
};

//=============================================================================
// 
//=============================================================================
bool TrajPoca::restrictToRange( double& l, const LHCb::Trajectory& t ) const
{
  double lmax = std::max(t.beginRange(),t.endRange()) ;
  double lmin = std::min(t.beginRange(),t.endRange()) ;
  bool rc = false ;
  if( l > lmax) {
    rc = true ;
    l = lmax ;
  } else if( l < lmin) {
    rc = true ;
    l = lmin ;
  }
  return rc ;
};

//=============================================================================
