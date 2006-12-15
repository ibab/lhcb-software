// $Id: TrajProjector.cpp,v 1.2 2006-12-15 19:18:51 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from GaudiKernel
#include "GaudiKernel/IMagneticFieldSvc.h"

// from TrackFitEvent
#include "Event/StateTraj.h"

// from TrackInterfaces
#include "Kernel/ITrajPoca.h"

// from LHCbKernel
#include "Kernel/AlignTraj.h"

// local
#include "TrajProjector.h"

using namespace Gaudi;
using namespace LHCb;


// trivial helpers to make code clearer...
namespace {
    typedef Matrix1x3 DualVector ;

    DualVector dual(const XYZVector& v) {
       DualVector d; v.GetCoordinates(d.Array()); return d;
    };

    double dot(const DualVector& a, const XYZVector& b) {
       return a(0,0)*b.X() + a(0,1)*b.Y() + a(0,2)*b.Z();
    };

}

//-----------------------------------------------------------------------------
///  Project a state onto a measurement
/// It returns the chi squared of the projection
//-----------------------------------------------------------------------------
template <typename T>
StatusCode TrajProjector<T>::project( const State& state,
                                      Measurement& meas )
{
  // Set refVector in case it was not set before
  if ( !meas.refIsSet() ) meas.setRefVector( state.stateVector() );

  static XYZVector bfield; // avoid constructing this every call to project...
  const TrackVector& refVec = meas.refVector();
  m_pIMF -> fieldVector( XYZPoint(refVec[0],refVec[1],meas.z()), bfield );

  // Get the reference state trajectory
  const StateTraj refTraj( refVec, meas.z(), bfield ) ; 

  // Get the measurement trajectory representing the centre of gravity
  const Trajectory& measTraj = meas.trajectory();  

  // Determine initial estimates of s1 and s2
  double s1 = 0.0; // Assume state is already close to the minimum
  double s2 = measTraj.arclength( refTraj.position(s1) );

  // Determine the actual minimum with the Poca tool
  static XYZVector dist; // avoid constructing this every call to project...
  StatusCode sc = m_poca -> minimize( refTraj, s1, measTraj, s2, dist, m_tolerance );
  if( sc.isFailure() ) return sc;

  // Set up the vector onto which we project everything
  // note that it is signed!
  DualVector unit = T::sign( dist, measTraj, s2 )*dual( dist.Unit() ) ;

  // compute the projection matrix from parameter space onto the (signed!) unit
  m_H =  unit * refTraj.derivative( s1 ) ;

  // Calculate the residual by projecting the distance onto unit, 
  //    correcting to 1st order for the difference between state and reference
  //    note that this is just - Dot(unit, dist+refTraj.derivative()*delta)
  m_residual = - ( dot(unit,dist)
                          + Vector1( m_H*(state.stateVector() - refVec))(0) ) ;

  // Set the error on the measurement so that it can be used in the fit
  double errMeasure2 = meas.resolution2( refTraj.position(s1), 
                                         refTraj.direction(s1) );
  m_errMeasure  = sqrt( errMeasure2 );

  // Calculate the error on the residual
  m_errResidual = sqrt( errMeasure2 + Similarity( m_H, state.covariance() )(0,0) );

  return StatusCode::SUCCESS;
}


//-----------------------------------------------------------------------------
/// Derivatives wrt. the measurement's aligment...
//-----------------------------------------------------------------------------
template <typename T>
typename TrajProjector<T>::Derivatives
TrajProjector<T>::alignmentDerivatives(const Measurement& meas, const XYZPoint& pivot) const
{
  // create the track trajectory...
  const TrackVector& refVec = meas.refVector();
  static XYZVector bfield;
  m_pIMF -> fieldVector( XYZPoint(refVec(0),refVec(1),meas.z()), bfield );
  const StateTraj refTraj( refVec, meas.z(), bfield ) ; 

  // Get the measurement trajectory
  const Trajectory& measTraj = meas.trajectory();  

  // Determine initial estimates of s1 and s2
  double s1 = 0.0; // Assume state is already close to the minimum
  double s2 = measTraj.arclength( refTraj.position(s1) );

  // Determine the actual minimum with the Poca tool
  static XYZVector dist;
  m_poca -> minimize( refTraj, s1, measTraj, s2, dist, m_tolerance );

  // Set up the vector onto which we project everything
  // note that it is signed, and given that 'dist' 
  DualVector unit = T::sign( dist, measTraj, s2 )*dual( dist.Unit() ) ;

  // compute the projection matrix from parameter space onto the (signed!) unit
  // NOTE: we need an extra minus sign as this is the derivative to the 
  //        2nd traj in poca, not the 1st as in project, and hence 'dist' point towards us...
  return -unit * AlignTraj(measTraj,pivot).derivative( s2 ) ;
}


//-----------------------------------------------------------------------------
/// Initialize
//-----------------------------------------------------------------------------
template <typename T>
StatusCode TrajProjector<T>::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { return Error( "Failed to initialize!", sc ); }

  m_pIMF = svc<IMagneticFieldSvc>( "MagneticFieldSvc",true );
  m_poca = tool<ITrajPoca>( "TrajPoca" );

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
/// Standard constructor, initializes variables
//-----------------------------------------------------------------------------
template <typename T>
TrajProjector<T>::TrajProjector( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent )
  : TrackProjector( type, name , parent )
{
  declareInterface<ITrackProjector>(this);
  declareProperty( "tolerance", m_tolerance = T::defaultTolerance() );
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
template <typename T>
TrajProjector<T>::~TrajProjector() {};



/// declare and instantiate ST and Velo projectors...
struct ST {
   static double defaultTolerance() 
   { return 0.002*Gaudi::Units::mm; }
   static int sign(const Gaudi::XYZVector& delta, const Trajectory&, double) 
   { return ( delta.x() > 0 ) ? 1 : -1 ; };
};


typedef TrajProjector<ST> TrajSTProjector ;
DECLARE_TOOL_FACTORY( TrajSTProjector );

struct Velo {
  static double defaultTolerance() 
  { return 0.0005*Gaudi::Units::mm; }
  static int sign(const XYZVector& delta, const Trajectory& meas, double marclen) 
  { return (delta.Cross(meas.direction(marclen)).z() > 0.0) ? 1 : -1 ; }
};

typedef TrajProjector<Velo> TrajVeloProjector ;
DECLARE_TOOL_FACTORY( TrajVeloProjector );
