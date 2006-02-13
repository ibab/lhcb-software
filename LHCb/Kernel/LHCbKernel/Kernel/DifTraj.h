// $Id: DifTraj.h,v 1.4 2006-02-13 11:00:51 graven Exp $
#ifndef KERNEL_DIFTRAJ_H 
#define KERNEL_DIFTRAJ_H 1

// Include files

// local
#include "Kernel/Trajectory.h"

/** @class DifTraj DifTraj.h Kernel/DifTraj.h
 *  
 *  DifTraj is a templated class enheriting from Trajectory.h.
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
    typedef ROOT::Math::SMatrix<double,3,N> Derivative;
    
    /// Retrieve the derivative of the point at fixed arclength 'arclength' 
    /// with respect to the parameters used to descrive the trajectory
    virtual Derivative derivative( double arclength ) const = 0;
    
  };
  
} // namespace LHCb

#endif // KERNEL_DIFTRAJ_H
