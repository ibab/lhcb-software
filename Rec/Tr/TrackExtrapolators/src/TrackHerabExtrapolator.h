#ifndef TRACKHERABEXTRAPOLATOR_H
#define TRACKHERABEXTRAPOLATOR_H 1

// Include files
#include "TrackFieldExtrapolatorBase.h"

/** @class TrackHerabExtrapolator TrackHerabExtrapolator.h "TrackHerabExtrapolator.h"
 *
 *  A TrackHerabExtrapolator is a ITrackExtrapolator which does a 'HerabRK5'
 *  extrapolation of a State.
 *  It doesn't take into account Multiple Scattering.
 *
 *  @author Edwin Bos
 *  @date   06/07/2005
 *  @author Jose A. Hernando (14-03-05)
 *  @author Matt Needham
 *  @date   22-04-2000
 */

class TrackHerabExtrapolator: public TrackFieldExtrapolatorBase
{

public:
  /// Constructor
  TrackHerabExtrapolator( const std::string& type, 
                          const std::string& name, 
                          const IInterface* parent );

  /// destructor
  virtual ~TrackHerabExtrapolator();

  /// initialize
  virtual StatusCode initialize();

  using TrackExtrapolator::propagate;
  /// Propagate a state vector from zOld to zNew
  /// Transport matrix is calulated when transMat pointer is not NULL
  virtual StatusCode propagate( Gaudi::TrackVector& stateVec,
                                double zOld,
                                double zNew,
                                Gaudi::TrackMatrix* transMat,
                                LHCb::ParticleID pid = LHCb::ParticleID(211) ) const ;

private:
 
  /// interface to Hera-b code
  void extrapolate( double& zIn,
                    double pIn[5], 
                    double& zNew, 
                    double pOut[5],
                    double fQp[25], 
                    int& istat) const ;


  /// Interface to 4th order Runga-Kutta
  void rk4order( double& z_in ,
                 double* p_in	,
                 double& z_out,
                 double* p_out,
                 double* rkd,
                 int& ierror) const ;

  /// Interface to fast 4th order Runga-Kutta  
  void rk4fast(  double& z_in ,
                 double* p_in ,
                 double& z_out ,
                 double* p_out,
                 double* rkd,
                 int& ierror ) const ;

  /// Interface to 5th order Runga-Kutta
  void rk5order( double& z_in ,
                 double* p_in ,
                 double& error ,
                 double& z_out ,
                 double* p_out,
                 double* rkd,
                 int& ierror) const ;

  /// Interface to fast 5th order Runga-Kutta
  void rk5fast(  double& z_in ,
                 double* p_in,
                 double& error,
                 double& z_out,
                 double* p_out,
                 double* rkd,
                 int& ierror) const ;

 /// interface to 5th order with derivatives caculated by numerical derivatives
  void rk5numde( double& z_in ,
                 double* p_in,
                 double& error,
                 double& z_out,
                 double* p_out,
                 double* rkd,
                 int& ierror) const ;

  /// Without derivatives rkd and ierror flag 
  void rk5fast(  double& z_in, 
                 double* p_in, 
                 double& error, 
                 double& z_out,
                 double* p_out) const ;

  /// Without derivatives rkd and ierror flag
  void rk4fast(  double& z_in, 
                 double* p_in, 
                 double& z_out, 
                 double* p_out) const ;
 
  int m_extrapolatorID;

  mutable double           m_error;  ///< Error
  mutable Gaudi::XYZPoint  m_point;  ///< to compute the field
  mutable Gaudi::XYZVector m_B;      ///< returned field
 
  // Parameters for Runge-Kutta
  double             m_qpCurls;    ///< Maximum curvature
  double             m_stepMin;    ///<
  double             m_stepMinRK5; ///<
};

#endif // TRACKHERABEXTRAPOLATOR_H
