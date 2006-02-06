// $Id: DifTraj.h,v 1.1 2006-02-06 11:11:48 ebos Exp $
#ifndef KERNEL_DIFTRAJ_H 
#define KERNEL_DIFTRAJ_H 1

// Include files

// local
#include "Trajectory.h"

/** @class DifTraj DifTraj.h Kernel/DifTraj.h
 *  
 *  DifTraj is a templated class enheriting from Trajectory.h.
 *  It interfaces the derivative method, which can have
 *  different dimensions for different XxxTraj classes.
 *
 *  @author Edwin Bos
 *  @date   2006-02-03
 */

template<unsigned int N>

 class DifTraj : public Trajectory {

 public:

   enum { kSize = N};

  /// Retrieve the derivative of the parabolic approximation to the trajectory
  /// with respect to the state parameters
  virtual ROOT::Math::SMatrix<double,3,N> derivative( const double& arclength ) const = 0;

};
#endif // KERNEL_DIFTRAJ_H
