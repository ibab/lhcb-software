// $Id: TrajPoca.cpp,v 1.4 2007-03-02 08:49:27 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// Math Definitions
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

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
};

//=============================================================================
// Default destructor
//=============================================================================
TrajPoca::~TrajPoca() {};


//=============================================================================
// Find arclengths along trajectories having a distance smaller than tolerance
//=============================================================================
StatusCode TrajPoca::minimize( const LHCb::Trajectory& traj1,
                               double& arclength1, 
                               bool restrictRange1,
                               const LHCb::Trajectory& traj2,
                               double& arclength2, 
                               bool restrictRange2, 
                               Gaudi::XYZVector& distance,
                               double precision )
{
  StatusCode status = StatusCode::SUCCESS;

  static Gaudi::XYZPoint newPos1, newPos2;
  double delta(0), prevdelta(0);
  int nOscillStep(0);
  int nDivergingStep(0);
  int nStuck(0);
  bool finished = false;

  for( int istep = 0; istep < m_maxnTry && !finished; ++istep ) {
    double prevflt1      = arclength1;
    double prevflt2      = arclength2;
    double prevprevdelta = prevdelta;
    status = stepTowardPoca( traj1, arclength1, restrictRange1,
                             traj2, arclength2, restrictRange2, precision );
    if( !status ) break; // Parallel Trajectories in stepTowardPoca

    newPos1  = traj1.position( arclength1 );
    newPos2  = traj2.position( arclength2 );
    distance = ( newPos1 - newPos2 );
    delta    = distance.R();
    double step1 = arclength1 - prevflt1;
    double step2 = arclength2 - prevflt2;
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
      if( restrictRange1 && step1 == 0. || restrictRange2 && step2 == 0. ) {
        if( ++nStuck > m_maxnStuck ) {
          // downgrade to a point poca
          Gaudi::XYZVector dist = Gaudi::XYZVector( 0., 0., 0. );
          restrictRange2 ? 
            minimize( traj1, arclength1, restrictRange1, newPos2, dist, precision )
            : minimize( traj2, arclength2, restrictRange2, newPos1, dist, precision );
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
          arclength1 = prevflt1;
          arclength2 = prevflt2;
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
          arclength1 = restrictLen( prevflt1 + 0.5 * step1, traj1, restrictRange1 );
          arclength2 = restrictLen( prevflt2 + 0.5 * step2, traj2, restrictRange2 );
          newPos1    = traj1.position( arclength1 );
          newPos2    = traj2.position( arclength2 );
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
                               double& arclength,
                               bool restrictRange,
                               const Gaudi::XYZPoint& pt,
                               Gaudi::XYZVector& distance,
                               double /*precision*/ )
{
  arclength = restrictLen( traj.arclength( pt ), traj, restrictRange );
  distance = traj.position( arclength ) - pt;

  return StatusCode::SUCCESS;
};

//=============================================================================
// 
//=============================================================================
StatusCode TrajPoca::stepTowardPoca( const LHCb::Trajectory& traj1,
                                     double& arclength1,
                                     bool restrictRange1,
                                     const LHCb::Trajectory& traj2,
                                     double& arclength2,
                                     bool restrictRange2,
                                     double tolerance ) const
{
  // a bunch of ugly, unitialized statics 
  // -- but, believe me, it really is faster this way...
  // (assignment is faster than c'tor...)
  static Gaudi::XYZVector dir1, dir2;
  static Gaudi::XYZVector delDir1, delDir2;
  static Gaudi::XYZPoint  pos1, pos2;

  traj1.expansion( arclength1, pos1, dir1, delDir1 );
  traj2.expansion( arclength2, pos2, dir2, delDir2 );
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
  // allow: Since `tolerance' is large, this cut effectively only
  // takes care that we don't make large jumps past the kink in a
  // piecewise trajectory.

  double distToErr1 = traj1.distTo2ndError( arclength1, tolerance, pathDir1 );
  double distToErr2 = traj2.distTo2ndError( arclength2, tolerance, pathDir2 );

  // Factor to push just over border of piecewise traj (essential!)
  const double smudge = 1.01;
  if( fabs( df1 ) > smudge * distToErr1 ) {
    // choose solution for which df1 steps just over border
    df1 = smudge * distToErr1 * pathDir1;
    // now recalculate df2, given df1:
    df2 = ( ub - df1 * cab ) / cbb;
  }

  if( fabs( df2 ) > smudge * distToErr2 ) {
    // choose solution for which df2 steps just over border
    df2 = smudge * distToErr2 * pathDir2;
    // now recalculate df1, given df2:
    df1 = ( ua - df2 * cab ) / cbb;
    // if still not okay,
    if( fabs( df1 ) > smudge * distToErr1 ) {
      df1 = smudge * distToErr1 * pathDir1;
    }
  }

  arclength1 = restrictLen( arclength1 + df1, traj1, restrictRange1 );
  arclength2 = restrictLen( arclength2 + df2, traj2, restrictRange2 );

  // another check for parallel trajectories
  if( fabs( arclength1 ) > m_maxDist && fabs( arclength2 ) > m_maxDist ) {
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
double TrajPoca::restrictLen( double l,
                              const LHCb::Trajectory& t,
                              bool restrictRange ) const
{
  return ( restrictRange ? t.restrictToRange(l) : l );
};

//=============================================================================
