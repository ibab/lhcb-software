// $Id: TrajPoca.h,v 1.4 2007-10-16 11:53:20 wouter Exp $
#ifndef TRACKTOOLS_TRAJPOCA_H 
#define TRACKTOOLS_TRAJPOCA_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from Tr/TrackInterfaces
#include "Kernel/ITrajPoca.h"

// from Kernel/LHCbKernel
#include "Kernel/Trajectory.h"

/** @class TrajPoca TrajPoca.h
 *  
 *  Calculates points of closest approach
 *  between two trajectories
 *
 *  @author Steve Schaffner (babar implementation)
 *  @author Edwin Bos, Jeroen van Tilburg, Eduardo Rodrigues
 *  @date   2005-11-23
 */

class TrajPoca : public GaudiTool,
                 virtual public ITrajPoca {

public:

  /// Default constructor
  TrajPoca( const std::string& type, 
            const std::string& name,
            const IInterface* parent );

  /// Default destructor
  virtual ~TrajPoca();

  /// Find points along trajectories at which the distance between the
  /// trajectories is at its minimum. The precision parameter is the desired
  /// numerical accuracy of mu1 and mu2. If the restrictrange flag is true, mu
  /// is restricted to the range of the trajectory.
  StatusCode minimize( const LHCb::Trajectory& traj1,
                       double& mu1, 
                       bool restrictRange1,
                       const LHCb::Trajectory& traj2,
                       double& mu2, 
                       bool restrictRange2, 
                       Gaudi::XYZVector& distance,
                       double precision ) const ;

  /// Find point along trajectory at which the distance to point 'p'
  /// is minimum. The precision parameter is the desired numerical accuracy of
  /// the expansion parameter mu. If the restrictrange flag is true, mu is
  /// restricted to the range of the trajectory.
  StatusCode minimize( const LHCb::Trajectory& traj,
                       double& mu,
                       bool restrictRange,
                       const Gaudi::XYZPoint& pt,
                       Gaudi::XYZVector& distance,
                       double precision ) const ;
private:
  StatusCode stepTowardPoca( const LHCb::Trajectory& traj1,
                             double& mu1,
                             bool restrictRange1,
                             const LHCb::Trajectory& traj2,
                             double& mu2, 
                             bool restrictRange2,
                             double precision ) const;

  bool restrictToRange( double& l, const LHCb::Trajectory& t ) const;

private:

  // jobOptions
  int m_maxnOscillStep;
  int m_maxnDivergingStep;
  int m_maxnStuck;
  int m_maxnTry;
  double m_maxDist;
  double m_maxExtrapTolerance;
};

#endif // TRACKTOOLS_TRAJPOCA_H
