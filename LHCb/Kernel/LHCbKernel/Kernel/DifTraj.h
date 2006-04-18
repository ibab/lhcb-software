// $Id: DifTraj.h,v 1.6 2006-04-18 16:01:15 erodrigu Exp $
#ifndef KERNEL_DIFTRAJ_H 
#define KERNEL_DIFTRAJ_H 1

// Include files

// local
#include "Kernel/Trajectory.h"

/** @class DifTraj DifTraj.h Kernel/DifTraj.h
 *  
 *  DifTraj is a templated class inheriting from Trajectory.h.
 *  It interfaces the derivative method, which can have
 *  different dimensions for different XxxTraj classes.
 *
 *  @author Edwin Bos
 *  @date   2006-02-03
 */

namespace LHCb
{

  template<unsigned int N>
  class DifTraj : public Trajectory {
    
  public:
    enum { kSize = N};

    /// Constructor taking the values of arclength
    /// at the begin and at the end of the trajectory
    DifTraj(double begin, double end) : Trajectory(begin, end) {}
    
    typedef ROOT::Math::SMatrix<double,3,N> Derivative;
    
    /// Retrieve the derivative of the point at fixed arclength 'arclength' 
    /// with respect to the parameters used to descrive the trajectory
    virtual Derivative derivative( double arclength ) const = 0;
    
  };
  
} // namespace LHCb

#endif // KERNEL_DIFTRAJ_H
